#include "rmdir.h"

//rmdir function
int rmdir(){
    //get in-memory inode of path
    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);

    //error checking
    if((mip->INODE.i_mode & 0xF000) != 0x4000){
        printf("ERROR: not a DIR\n");
        return -1;
    }
    if(mip->refCount != 1){
        printf("ERROR: MINODE is busy\n");
        return -1;
    }
    if(!emptydir(mip)){
        printf("ERROR: DIR is not empty\n");
        return -1;
    }

    //get parent's ino and INODE
    int parentino;
    findino(mip, parentino);
    MINODE *pmip = iget(mip->dev, parentino);

    //get name from parent ino
    char myname[64];
    findmyname(pmip, ino, myname);

    //remove name from parent directory
    rm_child(pmip, name); //cont here, make rm_child function
}

int emptydir(MINODE *mip){
    char buf[BLKSIZE], temp[256];
    DIR *dp;
    char *cp;
    int fileCnt = 0;

    if(mip->INODE.i_links_count > 2){
        return 0;
    }
    
    get_block(dev, mip->INODE.i_block[0], buf);
    dp = (DIR *)buf;
    cp = buf;
  
    while (cp < buf + BLKSIZE){ //steps through all files in directory
        mip = iget(dev, dp->inode); //gets  each file
        fileCnt++;

        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    if(fileCnt > 2){
        return 0;
    }
    else{
        return 1;
    }
}