#ifndef WRITE_MV_H
#define WRITE_MV_H

#include "globals.h"
#include "type.h"
#include "util.h"

#include <string.h>

int my_write(int fd, char* buf, int nbytes); 
int write_file();


#endif