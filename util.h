#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "type.h"
#include "globals.h"
#include "alloc_dalloc.h"

int get_block(int dev, int blk, char *buf);

int put_block(int dev, int blk, char *buf);

int tokenize(char *pathname);

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino);

//releases a used MINODE
void iput(MINODE *mip);

int search(MINODE *mip, char *name);

int getino(char *pathname);

int findmyname(MINODE *parent, u32 myino, char myname[ ]);

int findino(MINODE *mip, u32 *myino); // myino = i# of .. return i# of .

int pathname_to_fd(char* pathname); //gets the fd for a given pathname. Returns -1 if not found.

#endif