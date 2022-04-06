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
    DIR* prev_entry = NULL;
    DIR* entry_to_remove = NULL;
    DIR* dp_prev = NULL;
    char* cp = buf;
    //step to the last entry in the data block
    while (cp + dp->rec_len < buf + BLKSIZE) {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        // printf("%4d  %4d  %4d    %s\n", 
        //     dp->inode, dp->rec_len, dp->name_len, dp->name);
        printf("temp=%s\n", temp);
        if (strcmp(temp, myname)==0){
            printf("found %s : ino = %d\n", temp, dp->inode);
            prev_entry = dp_prev;
            entry_to_remove = dp;
        }
        //these 2 lines are for the actual stepping
        dp_prev = dp; //keep the previous entry 
        cp += dp->rec_len; 
        dp = (DIR*) cp;
    }
    printf("AFTER LOOP\n");
    //entry to remove points to the entry to remove 
    //dp points to last entry 
    printf("TEST\n");
    printf("prev_entry inode=%d rec_len=%d name_len=%d name=%s\n", prev_entry->inode, prev_entry->rec_len, prev_entry->name_len, prev_entry->name);
    printf("entry_to_remove inode=%d rec_len=%d name_len=%d name=%s\n", entry_to_remove->inode, entry_to_remove->rec_len, entry_to_remove->name_len, entry_to_remove->name);
    printf("dp_prev inode=%d rec_len=%d name_len=%d name=%s\n", dp_prev->inode, dp_prev->rec_len, dp_prev->name_len, dp_prev->name);
    printf("dp->inode=%d dp->rec_len=%d dp->name_len=%d dp->name=%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
    if (entry_to_remove->rec_len == BLKSIZE) { //case 1, first and only entry of data block
        printf("case 1, first and only entry of data block\n");
        bdalloc(dev, pmip->INODE.i_block[0]); //deallocate the block
        pmip->INODE.i_size -= BLKSIZE; //decrement by BLKSIZE
        pmip->dirty = 1; //mark pmip modified 
    } else if (entry_to_remove == dp) { //case 2, LAST entry in block. if the entry to remove equals dp because dp points to the last entry
        printf("case 2, LAST entry in block\n");
        prev_entry->rec_len += entry_to_remove->rec_len;
    } else { //case 3, entry is first but not the only entry or in the middle of a block 
        printf("case 3, first or middle entry\n");
        char* next = (char*) entry_to_remove + entry_to_remove->rec_len;
        int size = 100;
        memcpy(entry_to_remove, next, size);
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

