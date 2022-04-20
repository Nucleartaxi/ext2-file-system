#include "write_mv.h"


int my_write(int fd, char* buf, int nbytes) {
    printf("enter my_write\n");
    printf("text to write:\n");
    printf("%s\n\n", buf);

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