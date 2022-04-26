#ifndef GLOBALS_H
#define GLOBALS_H

#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
OFT oft[NOFT]; //opened file instance
PROC   proc[NPROC], *running;
MOUNT mountTable[8];  // set all dev = 0 in init()

char gpath[128]; // global for tokenized components
char *name[64];  // assume at most 64 components in pathname
int   n;         // number of component strings

int fd, dev;
int nblocks, ninodes, bmap, imap, iblk;
char line[128], cmd[32], pathname[128], pathname2[128];

#endif