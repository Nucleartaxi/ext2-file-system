#include "util.h"

int get_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   read(dev, buf, BLKSIZE);
}   

int put_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   write(dev, buf, BLKSIZE);
}   

int tokenize(char *pathname)
{
  int i;
  char *s;
  printf("tokenize %s\n", pathname);

  strcpy(gpath, pathname);   // tokens are in global gpath[ ]
  n = 0;

  s = strtok(gpath, "/");
  while(s){
    name[n] = s;
    n++;
    s = strtok(0, "/");
  }
  name[n] = 0;
  
  for (i= 0; i<n; i++)
    printf("%s  ", name[i]);
  printf("\n");
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
   MINODE *mip;
   MTABLE *mp;
   INODE *ip;
   int i, blk, offset;
   char buf[BLKSIZE];

   //search in-memory minodes first
   for (i=0; i<NMINODE; i++){
      mip = &minode[i];
      if (mip->refCount && mip->dev == dev && mip->ino == ino){
         mip->refCount++;
         //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
         return mip;
      }
   }

   // needed INODE=(dev,ino) not in memory
   mip = mialloc();        //allocate a free minode
   mip->dev = dev;         // assign to (dev, ino)
   mip->ino = ino; 
   blk = (ino-1)/8 + iblk; //disk block containing this inode
   offset= (ino-1)%8;      //inode in this block
   get_block(dev, blk, buf);
   ip = (INODE *)buf + offset;
   mip->INODE = *ip;       //copy inode to minode.inode
   //initalize minode
   mip->refCount = 1;
   mip->mounted = 0;
   mip->dirty = 0;
   mip->mptr = 0;
   return mip;
}

void iput(MINODE *mip)
{
   INODE *ip;
   int i, block, offset;
   char buf[BLKSIZE];

   if (mip==0){ //exits if minode doesn't exist
      return;
   }
   mip->refCount--; //decreases reference count
   if (mip->refCount > 0){ //exits if no user
      return;
   }
   if (!mip->dirty){ //exits if nothing new to write back
      return;
   }
   mip->dirty = 0; //if it's dirty and we're writing it back, it shouldn't be dirty anymore
 
   //write INODE back to disk
   block = (mip->ino - 1) / 8 + iblk;
   offset = (mip->ino - 1) % 8;

   //get block containing this inode 
   get_block(mip->dev, block, buf); 
   ip = (INODE*)buf + offset;       //ip points at INODE
   *ip = mip->INODE;                //copy INODE in block
   put_block(mip->dev, block, buf); //write back to disk 
   midalloc(mip);                   //mip->refCount = 0
} 

int search(MINODE *mip, char *name)
{
   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;

   printf("search for %s in MINODE = [%d, %d]\n", name,mip->dev,mip->ino);
   ip = &(mip->INODE);

   /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;
   printf("  ino   rlen  nlen  name\n");

   while (cp < sbuf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     printf("%4d  %4d  %4d    %s\n", 
           dp->inode, dp->rec_len, dp->name_len, dp->name);
     if (strcmp(temp, name)==0){
        printf("found %s : ino = %d\n", temp, dp->inode);
        return dp->inode;
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;
}

int getino(char *pathname)
{
  int i, ino, blk, offset;
  char buf[BLKSIZE];
  INODE *ip;
  MINODE *mip;

  printf("getino: pathname=%s\n", pathname);
  if (strcmp(pathname, "/")==0)
      return 2;
  
  // starting mip = root OR CWD
  if (pathname[0]=='/')
     mip = root;
  else
     mip = running->cwd;

  mip->refCount++;         // because we iput(mip) later
  
  tokenize(pathname);

  for (i=0; i<n; i++){
      printf("===========================================\n");
      printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);
 
      ino = search(mip, name[i]);

      if (ino==0){
         iput(mip);
         printf("name %s does not exist\n", name[i]);
         return 0;
      }
      iput(mip);
      mip = iget(dev, ino);
   }

   iput(mip);
   return ino;
}


int findmyname(MINODE *parent, u32 myino, char myname[ ]) 
{
   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;

   printf("search for ino %d in MINODE = [%d, %d]\n", myino,parent->dev,parent->ino);
   ip = &(parent->INODE);

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;
   printf("  ino   rlen  nlen  name\n");

   while (cp < sbuf + BLKSIZE){
     printf("%4d  %4d  %4d    %s\n", 
           dp->inode, dp->rec_len, dp->name_len, dp->name);
     if (dp->inode == myino){ //we found the node
        strncpy(myname, dp->name, dp->name_len);
        myname[dp->name_len] = 0;
        printf("found %s : ino = %d\n", myname, dp->inode);
        return dp->inode;
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;

}

int findino(MINODE *mip, u32 *myino) // myino = i# of . return i# of ..    //this is the get_myino(MINODE *mip, int * parent_ino) function from the textbook
{
  *myino = search(mip, "..");
  return mip->ino;
}

int pathname_to_fd(char* pathname) { //gets the fd for a given pathname. Returns -1 if not found.
   printf("pathname=%s strlen(pathname)=%d\n", pathname, strlen(pathname));
   if (strlen(pathname) == 0) {
      printf("pathname_to_fd error: empty pathname\n");
      return -1;
   }
   int ino = getino(pathname); //gets the ino of pathname
   if (ino == 0) {
      printf("pathname_to_fd error: file at pathname=%s does not exist\n", pathname);
      return -1;
   }
   for (int i = 0; i < NFD; ++i) {
      printf("ino=%d\n", ino);
      if (proc[0].fd[i]->minodePtr->ino == ino) {
         printf("found fd=%d for pathname=%s\n", i, pathname);
         return i;
      }
   }
   printf("Error: no fd for pathname=%s\n", pathname);
   return -1;
}