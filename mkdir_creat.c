#include "type.h"
#include "globals.h"
#include <string.h>
#include <libgen.h>
#include <stdio.h>

int my_mkdir() { 
    char* dname = dirname(pathname);
    char* bname = basename(pathname);
    printf("my_mkdir dname=%s bname=%s\n", dname, bname);
}