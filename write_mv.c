#include "write_mv.h"


int my_write(int fd, char* buf, int nbytes) {
    printf("enter my_write\n");
    printf("text to write:\n");
    printf("%s\n\n", buf);

    int count = 0; 
    OFT* oftp = proc[0].fd[fd]; //oft pointer
    while (nbytes > 0) {
        int lbk = oftp->offset / BLKSIZE; 
        int blk = 0;
        int start = oftp->offset % BLKSIZE;
        char kbuf[BLKSIZE];

        MINODE* mip = proc[0].fd[fd]->minodePtr;
        //convert logical block number (lbk) to physical block number (blk)
        if(lbk < 12){ //direct block
            if (mip->INODE.i_block[lbk] == 0) { //no data block yet
                mip->INODE.i_block[lbk] = balloc(mip->dev); //allocate a block
            }
            blk = mip->INODE.i_block[lbk];
        }
        else if(lbk >= 12 && lbk < 256 + 12){ //indirect block
            int ibuf[BLKSIZE/4];
            if (mip->INODE.i_block[12] == 0) { //if the indirect block doesn't exist
                mip->INODE.i_block[12] = balloc(mip->dev);
                get_block(dev, mip->INODE.i_block[12], (char*)ibuf); //get the allocated block
                bzero(ibuf, BLKSIZE); //zero out the allocated block
                put_block(dev, mip->INODE.i_block[12], (char*)ibuf); //get the allocated block
            }
            get_block(dev, mip->INODE.i_block[12], (char*)ibuf);
            blk = ibuf[lbk - 12];
            if (blk == 0) { //if block doesn't exist, allocate one
                ibuf[lbk - 12] = balloc(mip->dev);
            }
        }
        else{ //double indirect block
            int ibuf[BLKSIZE/4]; //double indirect block buf
            if (mip->INODE.i_block[13] == 0) { //if the double indirect block doesn't exist
                mip->INODE.i_block[13] = balloc(mip->dev);  //allocate the block
                get_block(dev, mip->INODE.i_block[13], (char*)ibuf); 
                bzero(ibuf, BLKSIZE);
                put_block(dev, mip->INODE.i_block[13], (char*)ibuf);
            }
            //mip->INODE.i_block[13] exists 
            get_block(dev, mip->INODE.i_block[13], (char*)ibuf); //get the double indirect block into ibuf
            int lbkSet = (lbk - 268) / 256;
            int lbkOffset = (lbk - 268) % 256;
            int ibuf2[BLKSIZE/4]; //indirect block buf   
            if (ibuf[lbkSet] == 0) { //indirect doesn't exist, allocate an indirect block for it 
                ibuf[lbkSet] = balloc(mip->dev); //allocate an indirect block and store it in the double indirect block
                get_block(dev, ibuf[lbkSet], (char*)ibuf2); //get the indirect block
                bzero(ibuf2, BLKSIZE); //zero out indirect block 
                put_block(dev, ibuf[lbkSet], (char*)ibuf2); //put indirect block back to disk
            }
            //indirect block exists 
            get_block(dev, ibuf[lbkSet], (char*)ibuf2);
            if (ibuf2[lbkOffset] == 0) { //direct block doesn't exist
                ibuf2[lbkOffset] = balloc(mip->dev); //allocate a block in the indirect block
            } 
            blk = ibuf2[lbkOffset]; //blk = double indirect[lbkSet] -> indirect[lbkOffset] -> blk
            put_block(dev, ibuf[lbkSet], (char*)ibuf2); //put indirect block back
            put_block(dev, mip->INODE.i_block[13], (char*)ibuf); //put the double indirect block back to disk
        }
        get_block(dev, blk, kbuf); //read block into kbuf[BLKSIZE]
        char* cp = kbuf + start; 
        int remain = BLKSIZE - start; 
        while (remain > 0) {
            *cp++ = *buf++; 
            oftp->offset++; count++; 
            remain--; nbytes--; 
            if (oftp->offset > mip->INODE.i_size) {
                mip->INODE.i_size++;
            }
            if (nbytes < 0) {
                break;
            }
        }
        put_block(dev, blk, kbuf);
    }
    oftp->minodePtr->dirty = 1; 
    return count;
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
        char buf[BLKSIZE + 1]; 
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
    close(fd);
    close(gd);
    return 0;
}