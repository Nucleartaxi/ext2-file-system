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

int decFreeInodes(int dev){ //I don't know what dec means and we don't have buf declared
    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

int ialloc(int dev){

}

int balloc(dev){

}