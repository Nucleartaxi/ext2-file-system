#include "read_cat.h"

//called read fd_name number_bytes
int read_file(){
    //get fd from pathname
    if(pathname2){
        int nbytes = atoi(pathname2);
    }
}

int myread(int fd, char *buf, int nbytes){ //needs to be optimized
    int count = 0, blk = 0;
    MINODE *mip = proc[0].fd[fd]->minodePtr;
    int avil = mip->INODE.i_size - proc[0].fd[fd]->offset; //number of bytes still availible in file
    char readBuf[BLKSIZE];
    char* cq = buf;

    while(nbytes && avil){
        int lbk = proc[0].fd[fd]->offset / BLKSIZE; //logical block
        int startByte = proc[0].fd[fd]->offset % BLKSIZE; //start read byte

        if(lbk < 12){ //direct block
            blk = mip->INODE.i_block[lbk];
        }
        else if(lbk >= 12 && lbk < 256 + 12){ //indirect block
            char ibuf[256];
            get_block(fd, ip->i_block[12], ibuf);
            blk = ibuf[lbk - 12];
        }
        else{ //double indirect block
            char ibuf[256], ibuf2[256];
            get_block(fd, ip->i_block[13], ibuf);
            int lbkSet, lbkOffset;
            lbkSet = (lbk - 268) / 256;
            lbkOffset = (lbk - 268) % 256;
            get_block(fd, ibuf[lbkSet], ibuf2);
            blk = ibuf2[lbkOffset];
        }

        //get data block into readbuf
        get_block(mip->dev, blk, readBuf);

        //copy from startByte to buf, at most remain bytes in this block
        char* cp = readBuf + startByte;
        int remain = BLKSIZE - startByte; //bytes remaining

        while(remain > 0){
            *cq++ = *cp++;
            oft->offset++;
            count++;
            avil--;
            nbytes--;
            remain--;
            if(nbytes <= 0 || avil <= 0){
                break;
            }
        }
        //loops back through while loop if one block is not enough
    }
    printf("myread: read %d char from file descriptor %d\n", count, fd);  
    return count;   // count is the actual number of bytes read
}