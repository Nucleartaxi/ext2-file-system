#include "misc.h"

//stat command
int mystat(){
    struct stat myst;

    //get ino of filename into memory
    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);
    //copy fields into stat
    myst.st_dev = dev;
    myst.st_ino = ino;
    myst.st_mode = mip->INODE.i_mode;
    myst.st_nlink = mip->INODE.i_links_count;
    myst.st_uid = mip->INODE.i_uid;
    myst.st_gid = mip->INODE.i_gid;
    myst.st_size = mip->INODE.i_size;
    myst.st_blocks = mip->INODE.i_blocks;
    myst.st_atim = mip->INODE.i_atime;
    myst.st_mtim = mip->INODE.i_mtime;
    myst.st_ctim = mip->INODE.i_ctime;

    iput(mip);

}