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
    printf("rm_child\n");
    //pg 355 in pdf textbook
    char buf[BLKSIZE];
    char temp[256];

    get_block(dev, pmip->INODE.i_block[0], buf); //get parent's data block into a buf
    DIR* dp = (DIR*) buf;
    DIR* dp_prev = NULL;
    char* cp = buf;
    int size = 0; //records the size up to, but not including, the entry we want to remove
    //step to the last entry in the data block
    int size2 = 0;
    char* cp2 = cp;
    DIR* dp2 = dp;
    while (cp2 + dp2->rec_len < buf + BLKSIZE) {
        strncpy(temp, dp2->name, dp2->name_len);
        temp[dp2->name_len] = 0;
        // printf("%4d  %4d  %4d    %s\n", 
        //     dp->inode, dp->rec_len, dp->name_len, dp->name);
        printf("temp=%s\n", temp);
        //these lines are for the actual stepping
        size2 += dp2->rec_len; //increments the size by the rec_len of this entry
        cp2 += dp2->rec_len; 
        dp2 = (DIR*) cp2;
    }
    printf("temp!!!!!!=%s\n", dp2->name);
    printf("size2=%d\n", size2);
    printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp2->inode, dp2->rec_len, dp2->name_len, dp2->name);
    int size2_plus_dp = size2 + dp2->rec_len;
    printf("size2=%d\n", size2_plus_dp);
    while (cp + dp->rec_len < buf + BLKSIZE) {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        // printf("%4d  %4d  %4d    %s\n", 
        //     dp->inode, dp->rec_len, dp->name_len, dp->name);
        printf("temp=%s\n", temp);
        if (strcmp(temp, myname)==0){
            printf("found %s : ino = %d\n", temp, dp->inode);
            break;
        }
        //these lines are for the actual stepping
        size += dp->rec_len; //increments the size by the rec_len of this entry
        dp_prev = dp; //keep the previous entry 
        cp += dp->rec_len; 
        dp = (DIR*) cp;
    }
    printf("AFTER LOOP\n");
    //entry to remove points to the entry to remove 
    //dp points to last entry 
    printf("TEST\n");
    printf("size=%d\n", size);
    printf("dp_prev inode=%d rec_len=%d name_len=%d name=%s\n", dp_prev->inode, dp_prev->rec_len, dp_prev->name_len, dp_prev->name);
    printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
    if (dp->rec_len == BLKSIZE) { //case 1, first and only entry of data block
        printf("case 1, first and only entry of data block\n");
        bdalloc(dev, pmip->INODE.i_block[0]); //deallocate the block
        pmip->INODE.i_size -= BLKSIZE; //decrement by BLKSIZE
        pmip->dirty = 1; //mark pmip modified 
    } else if (size + dp->rec_len == BLKSIZE) { //case 2, LAST entry in block. if the entry to remove equals dp because dp points to the last entry
        printf("case 2, LAST entry in block\n");
        dp_prev->rec_len += dp->rec_len;
    } else { //case 3, entry is first but not the only entry or in the middle of a block 
        printf("case 3, first or middle entry\n");
        int rlen = dp->rec_len; //stores the length of the entry to remove so we can add it to the last entry later 
        memcpy(dp, cp, BLKSIZE - size); //copy everything after the entry we want to remove (BLKSIZE - size) into the space previously occupied by the entry we are removing
        printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
        printf("dp->rec_len=%d rlen=%d size=%d\n", dp->rec_len, rlen, size);
        while (cp + dp->rec_len < buf + BLKSIZE) {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            // printf("%4d  %4d  %4d    %s\n", 
            //     dp->inode, dp->rec_len, dp->name_len, dp->name);
            printf("temp=%s\n", temp);
            //these lines are for the actual stepping
            cp += dp->rec_len; 
            dp = (DIR*) cp;
        }
        printf("end of loop, found last entry\n");
        printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
        dp->rec_len += rlen;
        printf("dp->rec_len=%d rlen=%d size=%d\n", dp->rec_len, rlen, size);
    }
    put_block(dev, pmip->INODE.i_block[0], buf); //put block back to disk
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
    // if(mip->refCount != 1){
    //     printf("ERROR: MINODE is busy\n");
    //     return -1;
    // }
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

