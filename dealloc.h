#ifndef DEALLOC_H
#define DEALLOC_H

#include "type.h"

//releases a used minode
int midalloc(MINODE *mip);

//deallocates an inode
int idalloc(int dev, int ino);
//deallocates a block
int bdalloc(int dev, int bno);

#endif