#include "rmdir.h"
#include "string.h"

//checks if directory is empty
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

//removes a named dir entry from the parent dir
int rm_child(MINODE *pmip, char* myname){
    //pg 355 in pdf textbook
    char buf[BLKSIZE];
    char temp[256];

    get_block(dev, pmip->INODE.i_block[0], buf); //get parent's data block into a buf
    DIR* dp = (DIR*) buf;
    char* cp = buf;
    //step to the last entry in the data block
    while (cp + dp->rec_len < buf + BLKSIZE) {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        printf("%4d  %4d  %4d    %s\n", 
            dp->inode, dp->rec_len, dp->name_len, dp->name);
        if (strcmp(temp, myname)==0){
            printf("found %s : ino = %d\n", temp, dp->inode);
            break;
            //these 2 lines are for the actual stepping
            cp += dp->rec_len; 
            dp = (DIR*) cp;
        }
    }
    printf("name=%s ino=%d \n", temp, dp->inode);
    printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
}

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
    findino(mip, &parentino);
    MINODE *pmip = iget(mip->dev, parentino);

    //get name from parent ino and remove it
    char myname[64];
    findmyname(pmip, ino, myname);
    rm_child(pmip, myname); //make rm_child function

    //parent node housekeeping
    pmip->INODE.i_links_count--;
    pmip->dirty = 1;
    iput(pmip);

    //deallocate the data blocks and inode
    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);
}

