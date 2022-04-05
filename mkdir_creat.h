#ifndef MKDIR_CREAT_H
#define MKDIR_CREAT_H

#include "type.h"
#include "globals.h"

int my_mkdir();
int my_creat();

//enters ino, basename as dir_entry to parent INODE
int enter_child(MINODE* pmip, int ino, char* bname);

#endif