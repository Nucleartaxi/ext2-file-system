#include "links.h"

int link(){
    //verify old file exists and is not a dir
    int oino = getino(pathname);
    if(!oino){
        printf("ERROR: old file does not exist\n");
        return -1;
    }
    MINODE *omip = iget(dev, oino);
    if ((omip->INODE.i_mode & 0xF000) == 0x4000){ // if dir
        printf("ERROR: old file is a directory\n");
        return -1;
    }

    //verify new file does not exist
    if(getino(pathname2)){
        printf("ERROR: new file already exists\n");
        return -1;
    }

    //creat new file with same inode number as old file
    char buf1[128], buf2[128];
    char *parent, *child;
    strcpy(buf1, pathname2);
    parent = dirname(buf1);
    strcpy(buf2, pathname2);
    child = basename(buf2); //sets child to new basename
    int pino = getino(parent);
    MINODE *pmip = iget(dev, pino);
    // creat entry in new parent DIR with same inode number of old_file
    enter_child(pmip, oino, child); //waiting for creat

    omip->INODE.i_links_count++;
    omip->dirty = 1;
    iput(omip);
    iput(pmip);
}

int unlink(){
    //get minode of filename
    int ino = getino(pathname);
    MINODE* mip = iget(dev, ino);
    if ((mip->INODE.i_mode & 0xF000) == 0x4000){
        printf("ERROR: file to unlink cannot be a directory\n");
        return -1;
    }

    //remove filename's entry from parent DIR's data block
    char buf1[128], buf2[128];
    char *parent, *child;
    strcpy(buf1, pathname);
    parent = dirname(buf1);
    strcpy(buf2, pathname);
    child = basename(buf2);
    int pino = getino(parent);
    MINODE *pmip = iget(dev, pino);
    printf("child=%s\n", child);
    rm_child(pmip, child); //waiting for rmdir to be done
    pmip->dirty = 1;
    iput(pmip);

    //decrement INODE's link count by one
    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count > 0){
        printf("links_count of %s >0\n", child);
        mip->dirty = 1; //so it writes back to disk
    }
    else{ //no links
        printf("links_count of %s ==0\n", child);
        for(int i = 0; i < mip->INODE.i_blocks; i++){
            bdalloc(mip->dev, mip->INODE.i_block[i]);
        }
        idalloc(dev, mip->ino);
    }
    iput(mip);
}

int symlink(){
    char buf[BLKSIZE];

    //verify old file exists
    int oino = getino(pathname);
    if(!oino){
        printf("ERROR: old file does not exist\n");
        return -1;
    }

    //verify new file does not exist
    if(getino(pathname2)){
        printf("ERROR: new file already exists\n");
        return -1;
    }

    //creat new file
    printf("pathname=%s pathname2=%s\n", pathname, pathname2);
    char oldfile[128];
    char oldfilepath[128];
    strcpy(oldfile, pathname);
    strcpy(oldfilepath, oldfile);
    char* old_file_bname = dirname(oldfilepath);
    strcpy(pathname, pathname2);
    my_creat();

    printf("oldfile=%s pathname=%s, pathname2=%s old_file_bname=%s\n", oldfile, pathname, pathname2, old_file_bname);

    //edit new ino values and store it
    int nino = getino(pathname2);
    MINODE *nmip = iget(dev, nino);
    nmip->INODE.i_mode = 0xA1FF;  //changes to link file
    
    //allocate a new block to store the newfile's pathname 
    int new_block = balloc(dev);
    nmip->INODE.i_block[0] = new_block;

    get_block(dev, nmip->INODE.i_block[0], buf); //get the buf
    
    strcpy(buf, oldfile); //store old_file name in newfile’s INODE.i_block[ ] area.
    printf("buf=%s\n", buf);
    put_block(dev, nmip->INODE.i_block[0], buf); //put the block back 

    nmip->INODE.i_size = strlen(oldfile); //set file size to length of old_file name
    nmip->dirty = 1;
    printf("isize=%d\n", nmip->INODE.i_size);
    iput(nmip);

    MINODE *cmip = iget(dev, getino(old_file_bname)); //current mip
    int pino;
    findino(cmip, &pino);
    iput(cmip);

    //mark and store it's parent ino
    MINODE *pmip = iget(dev, pino);
    pmip->dirty = 1;
    iput(pmip);
}
int readlink() { //unfinished but just leaving it here in case we need it in the future
    MINODE* mip = iget(dev, getino(pathname));
    if ((mip->INODE.i_mode & 0xF000)== 0xA000) { //check if link type
        printf("link type OK\n");
    } else {
        printf("Error: file is not link type\n");
        return -1;
    }
}