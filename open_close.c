#include "globals.h"
#include "util.h"
#include "mkdir_creat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

enum MODE {RD=0, WR=1, RW=2, APPEND=3};

int my_truncate(MINODE* mip) {
    for (int i = 0; i < 12; ++i) { //iterate through 12 direct blocks
        if (mip->INODE.i_block[i]) { //if block exists
            printf("released block=%d\n", mip->INODE.i_block[i]);
            bdalloc(dev, mip->INODE.i_block[i]);
        } else { //optimization, break loop early if block doesn't exist
            break;
        }
    }
    if (mip->INODE.i_block[12]) { //indirect blocks
        int buf[BLKSIZE/4];
        get_block(dev, mip->INODE.i_block[12], (char*) buf); //get the block which holds 256 pointers to other blocks
        for (int i = 0; i < BLKSIZE/4; ++i) {
            if (buf[i]) {
                printf("released block=%d\n", buf[i]);
                bdalloc(dev, buf[i]);
            } else { //break if we finished looking through blocks
                break;
            }
        }
    }
    if (mip->INODE.i_block[13]) { //double indirect blocks
        int double_indirect_buf[BLKSIZE/4];
        get_block(dev, mip->INODE.i_block[13], (char*) double_indirect_buf); //get the double indirect block
        for (int j = 0; j < BLKSIZE/4; j++) { //double indirect blocks
            //indirect blocks
            if (double_indirect_buf[j]) { //if block exists, clear all indirect blocks
                int buf[BLKSIZE/4];
                get_block(dev, double_indirect_buf[j], (char*) buf); //get the block which holds 256 pointers to other blocks
                for (int i = 0; i < BLKSIZE/4; ++i) {
                    if (buf[i]) {
                        printf("released block=%d\n", buf[i]);
                        bdalloc(dev, buf[i]);
                    } else { //break if we finished looking through blocks
                        break;
                    }
                }
            } else {
                break;
            }
        }
    }

    //TODO: update time field? 
    mip->INODE.i_atime = time(0L);
    mip->INODE.i_mtime = time(0L);
    mip->INODE.i_size = 0; //update size
    mip->dirty = 1; //mark mip dirty
    return 0;
}

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
            switch(mode){
                case 0 : oft[i].offset = 0;     // R: offset = 0
                    oft[i].minodePtr->INODE.i_atime = time(0L);
                    break;
                case 1 : my_truncate(mip);        // W: truncate file to 0 size
                    oft[i].offset = 0;
                    oft[i].minodePtr->INODE.i_atime = time(0L);
                    oft[i].minodePtr->INODE.i_mtime = time(0L);
                    break;
                case 2 : oft[i].offset = 0;     // RW: do NOT truncate file
                    oft[i].minodePtr->INODE.i_atime = time(0L);
                    oft[i].minodePtr->INODE.i_mtime = time(0L);
                    break;
                case 3 : oft[i].offset = mip->INODE.i_size;  // APPEND mode
                    oft[i].minodePtr->INODE.i_atime = time(0L);
                    oft[i].minodePtr->INODE.i_mtime = time(0L);
                    break;
                default: printf("invalid mode\n");
                    return(-1);
            }
            mip->dirty = 1; //because we modified time 
            for (int j = 0; j < NFD; ++j) { //search for first free fd[index] with the lowest entry in PROC
                if (proc[0].fd[j]==0) { //found a free entry
                    proc[0].fd[j] = &oft[i];
                    printf("fd=%d\n", j);
                    return j; //return index as file descriptor
                }
            }
            break;
        }
    }
}
int my_close(int fd) {
    if (proc[0].fd[fd] != 0) {
        proc[0].fd[fd]->refCount--; //dec OFT's refCount by 1 
        if (proc[0].fd[fd]->refCount == 0) { //if last process using this OFT
            printf("last process using fd=%d, releasing minode\n", fd);
            iput(proc[0].fd[fd]->minodePtr); //release minode
        }
    }
    proc[0].fd[fd] = 0; //clear PROC's fd[fd] to 0 
}
int my_close_pathname() {
    int fd = pathname_to_fd(pathname); //get the fd of the pathname 
    if (fd == -1) {
        printf("Error: could not close file=%s\n", pathname);
        return -1;
    }
    my_close(fd);
    printf("Successfully closed %s, fd=%d\n", pathname, fd);
}
int my_lseek() {

}