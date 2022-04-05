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