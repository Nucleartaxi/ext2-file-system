#include "stdio.h"
#include "time.h"

#include "type.h"
#include "globals.h"
#include "util.h"

//tests a bit for 1 or 0
int tst_bit(char *buf, int bit){
    return buf[bit/8] & (1 << (bit % 8));
}

//sets bit to 1
int set_bit(char *buf, int bit){
    buf[bit/8] |= (1 << (bit % 8));
}

// decreases free inodes count in SUPER and GD
int decFreeInodes(int dev){
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

// decreases free block count in SUPER and GD
int decFreeBlocks(int dev){
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

//allocates an inode
int ialloc(int dev){
    int  i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(dev, imap, buf);

    for (i=0; i < ninodes; i++){ // use ninodes from SUPER block
        if (tst_bit(buf, i)==0){
            set_bit(buf, i);
	        put_block(dev, imap, buf);

	        decFreeInodes(dev);

	        printf("allocated ino = %d\n", i+1); // bits count from 0; ino from 1
            return i+1;
        }
    }
  return 0;
}

int balloc(int dev){ 
    int  i;
    char buf[BLKSIZE];

    // read block_bitmap block
    get_block(dev, bmap, buf);

    for (i=0; i < nblocks; i++){ // use nblocks from SUPER block
        if (tst_bit(buf, i)==0){
            set_bit(buf, i);
	        put_block(dev, bmap, buf);

	        decFreeBlocks(dev);

	        printf("allocated block = %d\n", i+1); // bits count from 0; block from 1
            return i+1;
        }
    }
  return 0;
}

//add mialloc and midalloc pg 338

//allocates a free minode for use
MINODE* mialloc(){
    int i;
    for(i = 0; i < NMINODE; i++){
        MINODE *mp = &minode[i];
        if(mp->refCount == 0){
            mp->refCount = 1;
            return mp;
        }
    }
    printf("FS panic: out of minodes\n");
    return 0;
}
