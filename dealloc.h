#ifndef DEALLOC_H
#define DEALLOC_H

#include "stdio.h"
#include "time.h"

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc.h"

//releases a used minode
int midalloc(MINODE *mip);

//deallocates an inode
int idalloc(int dev, int ino);
//deallocates a block
int bdalloc(int dev, int bno);

#endif