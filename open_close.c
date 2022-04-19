#include "open_close.h"

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

// called lseek fd_pathname position 
int my_lseek() {
    int fd = pathname_to_fd(pathname);
    int pos = atoi(pathname2);
    int saveOFT = proc[0].fd[fd]->offset;
    int* OFT = &proc[0].fd[fd]->offset;
    MINODE* mip = proc[0].fd[fd]->minodePtr;
    if(pos < 0 || pos > mip->INODE.i_size - 1){
        printf("ERROR: location outside of acceptable bounds\n");
        return -1;
    }
    else{
        *OFT = pos;
        return saveOFT;
    }
}