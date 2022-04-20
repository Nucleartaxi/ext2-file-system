#include "write_mv.h"


int my_write(int fd, char* buf, int nbytes) {

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
        
    } else {
        printf("Write error: fd=%d already opened for read so we cannot write\n", fd);
    }
}