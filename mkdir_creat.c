#include <string.h>
#include <libgen.h>
#include <stdio.h>
#include <time.h>

#include "type.h"
#include "globals.h"
#include "util.h"
#include "alloc.h"

int enter_child(MINODE* pmip, int ino, char* bname) { //enters ino, basename as dir_entry to parent INODE
    char buf[BLKSIZE];
    for (int i = 0; i < 12; ++i) {
        if (pmip->INODE.i_block[i] == 0) {
            printf("i_block[%d]==0\n", i);
            break;
        }
        
        //step to last entry of data block
        get_block(dev, pmip->INODE.i_block[i], buf);
        DIR* dp = (DIR*) buf;
        char* cp = buf;

        while (cp + dp->rec_len < buf + BLKSIZE) {
            int test_ideal_length = 4*(  (8 + dp->name_len + 3) / 4  );
            printf("dp->rec_len=%d\n", dp->rec_len);
            printf("dp test_ideal_length=%d\n", test_ideal_length);
            cp += dp->rec_len; 
            dp = (DIR*) cp;
        }
        //dp points to last entry in block 
        int dp_ideal_length = 4*(  (8 + dp->name_len + 3) / 4  );
        printf("dp->rec_len=%d\n", dp->rec_len);
        printf("dp ideal_length=%d\n", dp_ideal_length);
        
        int need_length = 4*(  (8 + strlen(bname) + 3) / 4  );
        printf("len basename=%d\n", (int) strlen(bname));
        printf("need length=%d\n", need_length);

        int remain = dp->rec_len - dp_ideal_length; //last entry's rec_len - its ideal length
        printf("remain=%d\n", remain);

        if (remain >= need_length) {
            printf("remain >= need_length\n");
            
        }
    }
}

int kmkdir(MINODE* pmip, char* bname) { //creates the directory
    //allocate an inode and a disk block 
    int ino = ialloc(dev);
    int blk = balloc(dev);
    printf("ino=%d blk=%d\n", ino, blk);

    //load INODE into a minode
    MINODE* mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    ip->i_mode = 0x41ED; // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = blk; // new DIR has one data block
    for (int i = 1; i <= 14; ++i) { //ip->i_block[1] to ip->i_block[14] = 0;
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; // mark minode dirty
    iput(mip); // write INODE to disk

    //create data block for new DIR containing . and .. entries 
    char buf[BLKSIZE];
    bzero(buf, BLKSIZE); // optional: clear buf[ ] to 0
    DIR *dp = (DIR *)buf;
    // make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';
    // make .. entry: pino=parent DIR ino, blk=allocated block
    dp = (DIR*)((char *)dp + 12);
    dp->inode = pmip->ino;
    dp->rec_len = BLKSIZE-12; // rec_len spans block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(dev, blk, buf); // write to blk on diks

    //enter_child 
    enter_child(pmip, ino, bname);
}

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
        iput(pmip);
        return -1;
    }

    //basename must not exist in parent DIR:
    if (search(pmip, bname) == 0) {
        printf("basename %s does not exist in parent directory. This is good\n", bname);
        kmkdir(pmip, bname);
        iput(pmip);
        return 0;

    } else {
        printf("Error: basename %s already exists in parent directory.\n", bname);
        iput(pmip);
        return -1;
    }
}