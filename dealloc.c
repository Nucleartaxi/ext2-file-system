#include "dealloc.h"

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

//increments free block count in super and GD
int incFreeBlock(int dev){
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count++;
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

    // get inode bitmap block
    get_block(dev, imap, buf);
    clr_bit(buf, ino-1);

    // write buf back
    put_block(dev, imap, buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

//deallocates a block
int bdalloc(int dev, int bno){
    int i;
    char buf[BLKSIZE];

    if(bno > nblocks){
        printf("inumber %d out of range \n", bno);
        return -1;
    }

    // get inode bitmap block
    get_block(dev, bmap, buf);
    clr_bit(buf, bno-1);

    // write buf back
    put_block(dev, bmap, buf);

    // update free inode count in SUPER and GD
    incFreeBlock(dev);
}

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

//increments free block count in super and GD
int incFreeBlock(int dev){
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count++;
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

    // get inode bitmap block
    get_block(dev, imap, buf);
    clr_bit(buf, ino-1);

    // write buf back
    put_block(dev, imap, buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

//deallocates a block
int bdalloc(int dev, int bno){
    int i;
    char buf[BLKSIZE];

    if(bno > nblocks){
        printf("inumber %d out of range \n", bno);
        return -1;
    }

    // get inode bitmap block
    get_block(dev, bmap, buf);
    clr_bit(buf, bno-1);

    // write buf back
    put_block(dev, bmap, buf);

    // update free inode count in SUPER and GD
    incFreeBlock(dev);
}

//releases a used minode
int midalloc(MINODE *mip){
    mip->refCount = 0;
}