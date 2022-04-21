#ifndef READ_CAT_H
#define READ_CAT_H

#include <stdlib.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "open_close.h"

int read_file();
int my_read(int fd, char* buf, int nbytes);
int my_cat();

#endif