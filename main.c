#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

#include "type.h"
#include "util.h"
#include "globals.h"
#include "cd_ls_pwd.h"
#include "mkdir_creat.h"
#include "rmdir.h"
#include "link_unlink.h"
#include "symlink.h"
#include "misc.h"
#include "open_close.h"
#include "read_cat.h"
#include "write_cp.h"

int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->uid = p->gid = 0;
    p->cwd = 0;
  }
}

// load root INODE and set root pointer to it
int mount_root()
{  
  char buf[BLKSIZE];
  printf("mount_root()\n");
  root = iget(dev, 2);
  
  //records info of root device
  mountTable[0].dev = root->dev;
  mountTable[0].ninodes = sp->s_inodes_count;
  mountTable[0].nblocks = sp->s_blocks_count;
  mountTable[0].bmap = gp->bg_block_bitmap;
  mountTable[0].imap = gp->bg_inode_bitmap;
  mountTable[0].iblk = gp->bg_inode_table;
}

char *disk = "diskimage";
int quit(); //forward declaration
int main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];
  struct stat myst;

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){
    printf("open %s failed\n", disk);
    exit(1);
  }

  dev = fd;    // global dev same as this fd   

  //read super block
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  //verify it's an ext2 file system
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, iblk);

  init();  
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  // WRTIE code here to create P1 as a USER process
  //I commented this out because I don't think we need this yet 
  // printf("creating P1 as a USER process\n");
  // proc[1].pid = 1;
  // proc[1].uid = 1;
  // proc[1].cwd = iget(dev, 2);
  
  while(1){
    printf("input command : [ls|cd|pwd|mkdir|creat|rmdir|link|symlink|unlink|chmod|utime\n");
    printf("                 open|close|lseek|read|cat|write|cp|mv|quit] ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    if (line[0]==0)
       continue;
    pathname[0] = 0;

    //clears pathname2
    pathname2[0] = 0;
    sscanf(line, "%s %s %s", cmd, pathname, pathname2);
    printf("cmd=%s pathname=%s\n", cmd, pathname);
    if (!pathname2[0]==0){
      printf("pathname2=%s\n", pathname2);
    }
  
    if (strcmp(cmd, "ls")==0)
       ls();
    else if (strcmp(cmd, "cd")==0)
       cd();
    else if (strcmp(cmd, "pwd")==0)
       pwd(running->cwd);
    else if (strcmp(cmd, "mkdir")==0)
      my_mkdir();
    else if (strcmp(cmd, "creat")==0)
      my_creat();
    else if (strcmp(cmd, "rmdir")==0)
      my_rmdir();
    else if (strcmp(cmd, "link")==0)
      my_link();
    else if (strcmp(cmd, "unlink")==0)
      my_unlink();
    else if (strcmp(cmd, "symlink")==0)
      my_symlink();
    else if (strcmp(cmd, "open")==0)
      my_open();
    else if (strcmp(cmd, "close")==0)
      my_close_pathname();
    else if (strcmp(cmd, "lseek")==0)
      my_lseek();
    else if (strcmp(cmd, "read")==0)
      read_file();
      //will need to change when adding readdir
    else if (strcmp(cmd, "write")==0)
      write_file();
    else if (strcmp(cmd, "cat")==0)
      my_cat();
    else if (strcmp(cmd, "cp")==0)
      cp();
    else if (strcmp(cmd, "chmod")==0)
      my_chmod();
    else if (strcmp(cmd, "utime")==0)
      utime();
    else if (strcmp(cmd, "quit")==0)
      quit();
  }
}

int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    // printf("quit: ino=%d refcount=%d dirty=%d\n", mip->ino, mip->refCount, mip->dirty);
    mip->refCount = 0; //makes sure all refCounts are 0 
    if (mip->dirty) { //if mip was modified, write it 
      // printf("iput\n");
      iput(mip);
    }
  }
  exit(0);
}
