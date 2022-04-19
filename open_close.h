#ifndef OPEN_CLOSE_H
#define OPEN_CLOSE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "util.h"
#include "mkdir_creat.h"

int my_open();
int my_close(int fd);
int my_close_pathname(); //uses global pathname as file to close
int my_lseek();


#endif