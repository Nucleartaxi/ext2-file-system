#include "write_mv.h"


int my_write(int fd, char* buf, int nbytes) {
    printf("enter my_write\n");
    printf("text to write:\n");
    printf("%s\n\n", buf);

    int count = 0; 
    OFT* oftp = proc[0].fd[fd]; //oft pointer
    while (nbytes) {
        int lbk = oftp->offset / BLKSIZE; 
        int blk = 0;
        int start = oftp->offset % BLKSIZE;

        //convert logical block number (lbk) to physical block number (blk)
        if(lbk < 12){ //direct block
            blk = proc[0].fd[fd]->minodePtr->INODE.i_block[lbk];
        }
        else if(lbk >= 12 && lbk < 256 + 12){ //indirect block
            int ibuf[256];
            get_block(dev, proc[0].fd[fd]->minodePtr->INODE.i_block[12], (char*)ibuf);
            blk = ibuf[lbk - 12];
        }
        else{ //double indirect block
            int ibuf[256];
            get_block(dev, proc[0].fd[fd]->minodePtr->INODE.i_block[13], (char*)ibuf);
            int lbkSet = (lbk - 268) / 256;
            int lbkOffset = (lbk - 268) % 256;
            get_block(fd, ibuf[lbkSet], (char*)ibuf);
            blk = ibuf[lbkOffset];
        }
    }
}

int write_file() {
    int fd = pathname_to_fd(pathname);
    if (fd < 0) {
        printf("Write error: no fd for pathname=%s\n", pathname);
        return -1;
    }
    //fd exists 
    if (proc[0].fd[fd]->mode != 0) { //opened for W, RW, or APPEND
        printf("fd=%d is valid mode\n", fd);
        char buf[BLKSIZE]; 
        strcpy(buf, pathname2);
        return my_write(fd, buf, strlen(buf));
    } else {
        printf("Write error: fd=%d already opened for read so we cannot write\n", fd);
        return -1;
    }
}

//called cp dest src
int cp(){
    char cpbuf[BLKSIZE];
    char pathnameHolder[128];
    strcpy(pathnameHolder, pathname2); //saves cp dest for use

    //opens src for read
    pathname2[0] = 0;
    fd = my_open();

    //open dest for write or create if it doesn't exist
    strcpy(pathname, pathnameHolder);
    pathname2[0] = 1; pathname2[1] = '\0';
    int gd = my_open();

    while(n = my_read(fd, cpbuf, BLKSIZE)){
        write(gd, cpbuf, n);
    }
}