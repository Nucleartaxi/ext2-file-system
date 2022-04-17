#ifndef OPEN_CLOSE_H
#define OPEN_CLOSE_H

int my_open();
int my_close(int fd);
int my_close_pathname(); //uses global pathname as file to close
int my_lseek();


#endif