#ifndef ALLOC_H
#define ALLOC_H

#include "type.h"

//tests a bit for 1 or 0
int tst_bit(char *buf, int bit);
//sets bit to 1
int set_bit(char *buf, int bit);
//clears bit to 0
int clr_bit(char *buf, int bit);

//allocates an indoe number from bitmap
int ialloc(int dev);
//allocates an block number from bitmap
int balloc(int dev);

//allocates a free minode for use
MINODE* mialloc();

#endif
