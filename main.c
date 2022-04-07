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
#include "links.h"

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
  printf("mount_root()\n");
  root = iget(dev, 2);
}

char *disk = "diskimage";
int quit(); //forward declaration
int main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];

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
    printf("input command : [ls|cd|pwd|mkdir|creat|rmdir|link|unlink|quit] ");
    //may want to add memset to clear line and/or the pathname variables
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    if (line[0]==0)
       continue;
    pathname[0] = 0;

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
      rmdir();
    else if (strcmp(cmd, "link")==0)
      link();
    else if (strcmp(cmd, "unlink")==0)
      unlink();
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
