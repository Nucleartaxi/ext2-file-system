#ifndef LINKS_H
#define LINKS_H

#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "mkdir_creat.h"
#include "dealloc.h"
#include "rmdir.h"

//link command
int link();

//unlink command
int unlink();

//symlink command
int symlink();

//readlink command 
// int readlink(); 

#endif