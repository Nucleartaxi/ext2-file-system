#ifndef CD_LS_PWD_H
#define CD_LS_PWD_H

#include "type.h"
#include "globals.h"

//basic change directory
int cd();

//basic ls
int ls();

//basic print working directory
char* pwd(MINODE *wd);

#endif
