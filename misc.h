#ifndef MISC_H
#define MISC_H

#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

#include "type.h"
#include "globals.h"
#include "util.h"

struct stat my_stat();
int my_chmod();
int utime();

#endif