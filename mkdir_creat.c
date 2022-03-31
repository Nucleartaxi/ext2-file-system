#include <string.h>
#include <libgen.h>
#include <stdio.h>

#include "type.h"
#include "globals.h"
#include "util.h"


int my_mkdir() { 
    //divide pathname into dname and bname
    char dname_buf[128];
    char bname_buf[128];
    strcpy(dname_buf, pathname);
    char* dname = dirname(dname_buf);
    strcpy(bname_buf, pathname);
    char* bname = basename(bname_buf);
    printf("my_mkdir dname=%s bname=%s\n", dname, bname);

    //dirname must exist and is a DIR
    int pino = getino(dname);
    MINODE* pmip = iget(dev, pino);
    if ((pmip->INODE.i_mode & 0xF000) == 0x4000) { // if (S_ISDIR())
        printf("%s is a dir\n", dname);
    } else {
        printf("Error: %s is not a dir\n", dname);
        return -1;
    }

    //basename must not exist in parent DIR:
    if (search(pmip, bname) == 0) {
        printf("basename %s does not exist in parent directory. This is good\n", bname);
    } else {
        printf("Error: basename %s already exists in parent directory.\n", bname);
        return -1;
    }
}