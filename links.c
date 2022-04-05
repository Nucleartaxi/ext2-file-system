#include "links.h"

int link(){
    //verify old file exists and is not a dir
    int oino = getino(pathname);
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
    
}