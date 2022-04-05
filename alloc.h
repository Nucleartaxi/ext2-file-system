#ifndef ALLOC_H
#define ALLOC_H

#include "type.h"

//allocates an indoe number from bitmap
int ialloc(int dev);
//allocates an block number from bitmap
int balloc(int dev);

//allocates a free minode for use
MINODE* mialloc();

#endif
