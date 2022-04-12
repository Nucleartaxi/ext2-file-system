#include "globals.h"
#include "util.h"
#include "mkdir_creat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum MODE {RD=0, WR=1, RW=2, APPEND=3};

int my_open() {
    int ino = getino(pathname); //gets ino of pathname
    printf("ino=%d\n", ino);
    if (ino==0) { //if ino doesn't exist, creat it
        printf("ino=%d does not exist. Creating %s\n", ino, pathname);
        my_creat(); //creates the file 
        ino = getino(pathname); //gets the ino of the file
    }
    MINODE* mip = iget(dev, ino);
    printf("ino=%d\n", mip->ino);

    //allocate an openTable entry OFT; initialize OFT entries 
    for (int i = 0; i < NOFT; ++i) { //iterate through the oft array
        if (oft[i].refCount==0) { //allocate a new oft
            printf("oft[%d].refCount==0\n", i);
            int mode = strtol(pathname2, NULL, 10);
            printf("mode=%d\n", mode);

            oft[i].mode = mode;
            oft[i].minodePtr = mip;
            oft[i].refCount = 1;
            if (mode == APPEND) { //for append mode, set offset to file size
                printf("mode=append\n");
                oft[i].offset = mip->INODE.i_size;
            } else {
                printf("mode=read, write, or read-write\n");
                oft[i].offset = 0;
            }
            break;
        }
    }
}
int my_close() {

}
int my_lseek() {

}