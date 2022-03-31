#ifndef UTIL_H
#define UTIL_H

#include "type.h"

int get_block(int dev, int blk, char *buf);

int put_block(int dev, int blk, char *buf);

int tokenize(char *pathname);

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino);

//releases a used MINODE
void iput(MINODE *mip);

int search(MINODE *mip, char *name);

int getino(char *pathname);

// These 2 functions are needed for pwd()
int findmyname(MINODE *parent, u32 myino, char myname[ ]);

int findino(MINODE *mip, u32 *myino); // myino = i# of . return i# of ..    //this is the get_myino(MINODE *mip, int * parent_ino) function from the textbook

#endif