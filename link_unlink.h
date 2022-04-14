#ifndef LINKS_H
#define LINKS_H

#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc_dalloc.h"
#include "mkdir_creat.h"
#include "rmdir.h"

//link command
int my_link();

//unlink command
int my_unlink();

#endif