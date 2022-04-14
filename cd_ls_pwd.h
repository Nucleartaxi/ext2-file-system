#ifndef CD_LS_PWD_H
#define CD_LS_PWD_H

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "type.h"
#include "globals.h"
#include "util.h"

//basic change directory
int cd();

//basic ls
int ls();

//basic print working directory
char* pwd(MINODE *wd);

#endif
