#include "stdio.h"
#include "time.h"

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc.h"

//increments free inode count in super and GD
int incFreeInodes(int dev){
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

//deallocates an inode
int idalloc(int dev, int ino){
    int i;
    char buf[BLKSIZE];

    if(ino > ninodes){
        printf("inumber %d out of range \n", ino);
        return -1;
    }

    //cont here
}

//deallocates a block
int bdalloc(dev, bno){

}

//releases a used minode
int midalloc(MINODE *mip){
    mip->refCount = 0;
}