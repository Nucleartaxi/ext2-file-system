#include "symlink.h"

int my_symlink(){
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

int my_readlink() { //unfinished but just leaving it here in case we need it in the future
    MINODE* mip = iget(dev, getino(pathname));
    if ((mip->INODE.i_mode & 0xF000)== 0xA000) { //check if link type
        printf("link type OK\n");
    } else {
        printf("Error: file is not link type\n");
        return -1;
    }
}