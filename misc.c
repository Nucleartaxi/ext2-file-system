#include "misc.h"

//stat command
struct stat my_stat(){
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
    myst.st_atim.tv_sec = mip->INODE.i_atime;
    myst.st_mtim.tv_sec = mip->INODE.i_mtime;
    myst.st_ctim.tv_sec = mip->INODE.i_ctime;

    iput(mip);
    return myst;
}

//chmod command (filename then mode)
int my_chmod(){
    int ino = getino(pathname);
    MINODE *mip = iget(dev, ino);
    mip->INODE.i_mode |= atoi(pathname2);
    mip->dirty = 1;
    iput(mip);
}