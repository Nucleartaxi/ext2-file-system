#include "type.h"
#include "globals.h"

//tests a bit for 1 or 0
int tst_bit(char *buf, int bit){
    return buf[bit/8] & (1 << (bit % 8));
}

//sets bit to 1
int set_bit(char *buf, int bit){
    buf[bit/8] |= (1 << (bit % 8));
}

// dec free inodes count in SUPER and GD (what does dec mean?)
int decFreeInodes(int dev, char buf){
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
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

	        decFreeInodes(dev, buf);

	        printf("allocated ino = %d\n", i+1); // bits count from 0; ino from 1
            return i+1;
        }
    }
  return 0;
}

int balloc(int dev, int ino){
    //creates an inode in an minode and writes to disk
    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    ip->i_mode = 0x41ED; //DIR type and permissions
    ip->i_uid = running->uid; //sets to owner uid
    ip->i_gid = running->gid; //sets group ID
    ip->i_size = BLKSIZE; //sets to uniform size in bytes
    ip->i_links_count = 2; //for . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; //blocks count in 512 byte chunks
    //ip->i_block[0] = bno; need to find what block number is *************
    for (int i = 1; 1 <= 14; i++){
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; //marks as dirty
    iput(mip); //writes INODE to disk

}