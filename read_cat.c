#include "read_cat.h"

int my_read(int fd, char* buf, int nbytes){
    int count  = 0, blk = 0, numbytes = nbytes;
    char* cq = buf;

    //compute availible bytes in file
    int avil = proc[0].fd[fd]->minodePtr->INODE.i_size - proc[0].fd[fd]->offset;

    while(numbytes && avil){
        int lbk = proc[0].fd[fd]->offset / BLKSIZE; //compute logical block
        int start = proc[0].fd[fd]->offset % BLKSIZE;// compute start byte

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
            int lbkSet, lbkOffset;
            lbkSet = (lbk - 268) / 256;
            lbkOffset = (lbk - 268) % 256;
            get_block(fd, ibuf[lbkSet], (char*)ibuf);
            blk = ibuf[lbkOffset];
        }

        //prep to read
        char kbuf[BLKSIZE];
        get_block(dev, blk, kbuf);
        char* cp = kbuf + start;
        int remain = BLKSIZE - start;

        //read into buf
        while(remain){
            *cq++ = *cp++;
            proc[0].fd[fd]->offset++; count++;
            remain--; avil--; nbytes--;
            if(nbytes <= 0 || remain <= 0){
                break;
            }
        }
    }
    return count;
}

//called read fd_name number_bytes
int read_file(){
    char readbuf[BLKSIZE];
    //get fd and nbytes from user input
    fd = pathname_to_fd(pathname);
    int nbytes = 0;
    if(atoi(pathname2)){
        nbytes = atoi(pathname2);
        printf("nbytes = %d\n", nbytes);
    }
    else{
        nbytes = proc[0].fd[fd]->minodePtr->INODE.i_size;
        printf("nbytes = %d\n", nbytes);
    }

    //verify file is opened in correct mode
    if(proc[0].fd[fd]->mode != 0 && proc[0].fd[fd]->mode != 2){
        printf("ERROR: file not opened in correct mode\n");
        return 0;
    }

    int ret = 0;
    if(nbytes > 1024){
        while (nbytes > 1024){
            ret  = ret + my_read(fd, readbuf, 1024);
            printf("%s", readbuf);
            nbytes - 1024;
        }
        ret  = ret + my_read(fd, readbuf, nbytes);
        printf("%s\n", readbuf);
    }
    else{
        ret = my_read(fd, readbuf, nbytes);
        printf("%s\n", readbuf);
    }
    return(ret);
}