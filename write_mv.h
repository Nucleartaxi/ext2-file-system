#ifndef WRITE_MV_H
#define WRITE_MV_H

#include <string.h>

#include "globals.h"
#include "type.h"
#include "util.h"
#include "open_close.h"
#include "read_cat.h"

int cp();
int my_write(int fd, char* buf, int nbytes); 
int write_file();


#endif