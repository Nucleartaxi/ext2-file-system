#include "write_mv.h"


int my_write(int fd, char* buf, int nbytes) {
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
                printf("allocated indirect block i_block[12]\n");
                mip->INODE.i_block[12] = balloc(mip->dev);
                get_block(dev, mip->INODE.i_block[12], (char*)ibuf); //get the allocated block
                bzero(ibuf, BLKSIZE); //zero out the allocated block
                put_block(dev, mip->INODE.i_block[12], (char*)ibuf); //get the allocated block
            }
            get_block(dev, mip->INODE.i_block[12], (char*)ibuf); //get the indirect block into ibuf
            blk = ibuf[lbk - 12];
            int ibuf2[BLKSIZE/4]; //buf for zeroing out the direct block within indirect block
            if (blk == 0) { //if block doesn't exist, allocate one
                printf("allocated new block in indirect block\n");
                ibuf[lbk - 12] = balloc(mip->dev);
                get_block(dev, ibuf[lbk - 12], (char*)ibuf2); //get the newly allocated block
                bzero(ibuf2, BLKSIZE);
                put_block(dev, ibuf[lbk - 12], (char*)ibuf2); //get the newly allocated block
                blk = ibuf[lbk - 12];
            }
            put_block(dev, mip->INODE.i_block[12], (char*)ibuf);

        }

        //DONT FORGET ABOUT THE OPTIMIZATION CODE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
                blk = ibuf[lbkSet];
            }
            //indirect block exists 
            get_block(dev, ibuf[lbkSet], (char*)ibuf2);
            int ibuf3[BLKSIZE/4];
            if (ibuf2[lbkOffset] == 0) { //direct block doesn't exist
                ibuf2[lbkOffset] = balloc(mip->dev); //allocate a block in the indirect block
                get_block(dev, ibuf2[lbkOffset], (char*)ibuf3); //get direct block
                bzero(ibuf3, BLKSIZE); //zero out direct block
                put_block(dev, ibuf2[lbkOffset], (char*)ibuf3); //put zeroed out direct block back 
                blk = ibuf2[lbkOffset];
            } 
            blk = ibuf2[lbkOffset]; //blk = double indirect[lbkSet] -> indirect[lbkOffset] -> blk
            put_block(dev, ibuf[lbkSet], (char*)ibuf2); //put indirect block back
            put_block(dev, mip->INODE.i_block[13], (char*)ibuf); //put the double indirect block back to disk
        }
        get_block(dev, blk, kbuf); //read block into kbuf[BLKSIZE]
        char* cp = kbuf + start; 
        int remain = BLKSIZE - start; 

        int to_write; //stores the number of bytes to write 
        if (remain < nbytes) { //min of remain and nbytes, so either write as many as remain allows if remain < nbytes, or write nbytes if nbytes < remain
            to_write = remain;
        } else {
            to_write = nbytes;
        }
        memcpy(cp, buf, to_write); 
        count += to_write; //update count of bytes written
        nbytes -= to_write; //update nbytes 
        remain -= to_write; //update remain
        oftp->offset += to_write; //update offset 
        if (oftp->offset > mip->INODE.i_size) { //update size if offset > size. (mostly used for RW and APPEND)
            mip->INODE.i_size += to_write;
        }
        // mip->INODE.i_size += remain;
        // count += remain;
        // while (remain > 0) {
        //     *cp++ = *buf++; 
        //     oftp->offset++; count++; 
        //     remain--; nbytes--; 
        //     if (oftp->offset > mip->INODE.i_size) {
        //         mip->INODE.i_size++;
        //     }
        //     if (nbytes <= 0) {
        //         break;
        //     }
        // }
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
    char cpbuf[BLKSIZE+1];
    char pathnameHolder[128];
    strcpy(pathnameHolder, pathname2); //saves cp dest for use
    int nbytes = 0, nread = 0;

    //opens src for read
    pathname2[0] = '0';
    fd = my_open();
    nbytes = proc[0].fd[fd]->minodePtr->INODE.i_size;

    //open dest for write or create if it doesn't exist
    strcpy(pathname, pathnameHolder);
    pathname2[0] = '3'; pathname2[1] = '\0';
    int gd = my_open();

    if(nbytes > 1024){
        while (nbytes > 1024){
            nread = my_read(fd, cpbuf, 1024);
            // cpbuf[1024] = '\0';
            my_write(gd, cpbuf, nread);
            nbytes -= 1024;
        }
        nread = my_read(fd, cpbuf, nbytes);
        cpbuf[nread] = '\0';
        my_write(gd, cpbuf, nread);
    }
    else{
        nread = my_read(fd, cpbuf, nbytes);
        cpbuf[nread] = '\0';
        my_write(gd, cpbuf, nread);
    }

    my_close(fd);
    my_close(gd);
    return 0;
}