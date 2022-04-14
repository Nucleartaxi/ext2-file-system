#ifndef SYMLINK_H
#define SYMLINK_H

#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc_dalloc.h"
#include "mkdir_creat.h"
#include "rmdir.h"

//symlink command
int my_symlink();

//readlink command 
// int my_readlink(); 

#endif