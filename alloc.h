#ifndef ALLOC_H
#define ALLOC_H

#include "type.h"

int ialloc(int dev);
int balloc(int dev);

//allocates a free minode for use
MINODE* mialloc();
//releases a used minode
int midalloc(MINODE *mip);

#endif