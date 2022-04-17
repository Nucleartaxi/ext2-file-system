#ifndef RMDIR_H
#define RMDIR_H

#include <stdio.h>
#include <string.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc_dalloc.h"

int my_rmdir();
int rm_child(MINODE *pmip, char* myname);

#endif