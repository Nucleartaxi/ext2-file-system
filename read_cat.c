#include "read_cat.h"

int read_file(){
    //do later
}

int myread(int fd, char *buf, int nbytes){
    int count = 0;
    MINODE *mip = proc[0].fd[fd]->minodePtr;
    int avil = mip->INODE.i_size - proc[0].fd[fd]->offset; //number of bytes still availible in file
    char readBuf[BLKSIZE];

    while(nbytes && avil){
        int lbk = proc[0].fd[fd]->offset / BLKSIZE; //logical block
        int startByte = proc[0].fd[fd]->offset % BLKSIZE; //start read byte

        if(lbk < 12){ //direct block
            int blk = mip->INODE.i_block[lbk];
        }
        else if(lbk >= 12 && lbk < 256 + 12){ //indirect block
            //do later
        }
        else{ //double indirect block
            //do later
        }

        //get data block into readbuf
        get_block(mip->dev, blk, readbuf);

        //copy from startByte to buf, at most remain bytes in this block
        char* cp = readBuf + startByte;
        //cont from here
    }



    //int remain = BLKSIZE - start; //bytes remaining
}