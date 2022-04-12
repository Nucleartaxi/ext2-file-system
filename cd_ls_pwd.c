#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "util.h"
#include "globals.h"
#include <sys/types.h>
#include <unistd.h>

//basic change directory
int cd()
{
  if (!strcmp(pathname, "")) { //for no pathname
    printf("cd no pathname given, returning to root\n");
    running->cwd = root; //sets cwd to root
    return 0;
  }
  int ino = getino(pathname);
  if(ino == 0){ //error if ino doesn't exist
    printf("ERROR: ino doesn't exist\n");
    return -1;
  }
  MINODE* mip = iget(dev, ino); 
  if (S_ISDIR(mip->INODE.i_mode)) {
    iput(running->cwd); //release old cwd
    running->cwd = mip; //change cwd to mip
  } else {
    printf("ERROR: not a directory\n");
    return -1;
  }
}

//ls for single file
int ls_file(MINODE *mip, char *name)
{
  char *t1 = "xwrxwrxwr-------";
  char *t2 = "----------------";
  char ftime[64];
	
  if ((mip->INODE.i_mode & 0xF000) == 0x8000) { // if (S_ISREG())
      printf("%c",'-');
  }
  if ((mip->INODE.i_mode & 0xF000) == 0x4000){ // if (S_ISDIR())
      printf("%c",'d');
  }
  if ((mip->INODE.i_mode & 0xF000) == 0xA000){ // if (S_ISLNK())
      printf("%c",'l');
  }
  for (int i=8; i >= 0; i--){
      if (mip->INODE.i_mode & (1 << i)){ // print r|w|x
          printf("%c", t1[i]);
      } 
      else{
          printf("%c", t2[i]); // or print -
      }
  }
  printf("%4d ",mip->INODE.i_links_count); // link count
  printf("%4d ",mip->INODE.i_gid); // gid
  printf("%4d ",mip->INODE.i_uid); // uid
  printf("%8d ",mip->INODE.i_size); // file size
	
  // print time
  strcpy(ftime, ctime((time_t*)&mip->INODE.i_mtime)); //copy time in calendar form
  ftime[strlen(ftime)-1] = 0; //kill \n at end
  printf("%s ",ftime);
	
  // print name
  printf("%s", name); //print file basename
  //print -> linkname if symbolic file
  if ((mip->INODE.i_mode & 0xF000)== 0xA000){
      char linkname[BLKSIZE];
      get_block(dev, mip->INODE.i_block[0], linkname); //gets the linkname
      printf(" -> %s", linkname); // print linked name
  }
	
  printf("\n");
}

//ls for a directory
int ls_dir(MINODE *mip)
{
  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){ //steps through all files in directory
     mip = iget(dev, dp->inode);
     //gets the name of each file
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
	
     ls_file(mip, temp); //does ls for each file

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}

//basic ls
int ls()
{
  if (!strcmp(pathname, "")) { //if no path is specified, use cwd by default
    printf("No path specified, using cwd\n");
    ls_dir(running->cwd);
  }
  else {
    int pathname_ino = getino(pathname);
    MINODE* mip = iget(dev, pathname_ino); //opening iget
    if (!S_ISDIR(mip->INODE.i_mode)) { //checks to make sure the path is a directory, not a file
      printf("Error, pathname: %s is not a directory\n", pathname);
      return -1;
    }
    ls_dir(mip);
    iput(mip); //closing iput
  }
  return 0;
}

//print working directory helper //continue from here
char* rpwd(MINODE *wd) {
  if (wd == root) { //exits recursion on root
    return 0;
  } 
  int parent_ino;
  int current_ino = findino(wd, &parent_ino); //gets parent and ino number
  printf("current_ino=%d parent_ino=%d\n", current_ino, parent_ino);
  MINODE* pip = iget(dev, parent_ino); //gets parent inone into pip
  char myname[64];
  findmyname(pip, current_ino, myname); //gets name of current ino
  rpwd(pip);
  printf("/%s", myname);
  return 0;
}

char* pwd(MINODE *wd)
{
  if (wd == root){ //if root do not enter recursion
    printf("/\n");
  } else {
    rpwd(wd);
    printf("\n");
  }
}



