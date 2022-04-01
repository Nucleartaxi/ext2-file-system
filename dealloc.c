#include "stdio.h"
#include "time.h"

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc.h"

//releases a used minode
int midalloc(MINODE *mip){
    mip->refCount = 0;
}