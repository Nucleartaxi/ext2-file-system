#ifndef MKDIR_CREAT_H
#define MKDIR_CREAT_H

#include <string.h>
#include <libgen.h>
#include <stdio.h>
#include <time.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc_dalloc.h"

int my_mkdir();
int my_creat();

//enters ino, basename as dir_entry to parent INODE
int enter_child(MINODE* pmip, int ino, char* bname);

#endif