/************* cd_ls_pwd.c file **************/
int cd()
{
  // printf("cd: under construction READ textbook!!!!\n");
  if (!strcmp(pathname, "")) {
    printf("cd no pathname given, returning to root\n");
    running->cwd = root;
    return;
  }
  int ino = getino(pathname); //return error if ino == 0
  MINODE* mip = iget(dev, ino); 
  if (S_ISDIR(mip->INODE.i_mode)) {
    iput(running->cwd); //release old cwd
    running->cwd = mip; //change cwd to mip
  } else {
    printf("ERROR: not a directory\n");
    return -1;
  }


  // READ Chapter 11.7.3 HOW TO chdir
}

int ls_file(MINODE *mip, char *name)
{
  // printf("ls_file: to be done: READ textbook!!!!\n");
  char *t1 = "xwrxwrxwr-------";
  char *t2 = "----------------";
  // printf("%s  ", name);
  int r, i;
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
  for (i=8; i >= 0; i--){
      if (mip->INODE.i_mode & (1 << i)){ // print r|w|x
          printf("%c", t1[i]);
      } else{
          printf("%c", t2[i]); // or print -
      }
  }
  printf("%4d ",mip->INODE.i_links_count); // link count
  printf("%4d ",mip->INODE.i_gid); // gid
  printf("%4d ",mip->INODE.i_uid); // uid
  printf("%8d ",mip->INODE.i_size); // file size
  // print time
  // printf("%s ", ctime(&mip->INODE.i_mtime));
  strcpy(ftime, ctime(&mip->INODE.i_mtime)); // print time in calendar form
  ftime[strlen(ftime)-1] = 0; // kill \n at end
  printf("%s ",ftime);
  // print name
  printf("%s", name); // print file basename
  // // print -> linkname if symbolic file
  if ((mip->INODE.i_mode & 0xF000)== 0xA000){
      char linkname[FILENAME_MAX];
      readlink(mip, linkname, FILENAME_MAX);
      printf(" -> %s", linkname); // print linked name
  }
  printf("\n");
  // READ Chapter 11.7.3 HOW TO ls
}

int ls_dir(MINODE *mip)
{
  printf("ls_dir\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){
     //gets the name of each file
     mip = iget(dev, dp->inode);
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
	
     ls_file(mip, temp);

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}

int ls()
{
  if (!strcmp(pathname, "")) { //if no path is specified, use cwd by default
    printf("No path specified, using cwd\n");
    ls_dir(running->cwd);
  } else {
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

char* rpwd(MINODE *wd) {
  if (wd == root) {
    return;
  } 
  int parent_ino;
  int current_ino = findino(wd, &parent_ino);
  printf("current_ino=%d parent_ino=%d\n", current_ino, parent_ino);
  MINODE* pip = iget(dev, parent_ino); //parent inode pointer 
  char* myname[64];
  findmyname(pip, current_ino, myname);
  rpwd(pip);
  printf("/%s", myname);
  return;
}
char* pwd(MINODE *wd)
{
  // printf("pwd: READ HOW TO pwd in textbook!!!!\n");
  if (wd == root){
    printf("/\n");
  } else {
    rpwd(wd);
    printf("\n");
  }
}



