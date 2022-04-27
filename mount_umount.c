#include "mount_umount.h"

//returns pointer to dev's mounttable entry
MOUNT *getmptr(int dev){
    for (int i = 0; i < 8; i++){
        if (mountTable[i].dev == dev){
            return &mountTable[i];
        }
    }
}

//use by mount filesys mount_point or just mount
int mount(){
    //if just mount display mounted filesystem
    if (pathname2 == '\0'){ 
        //display mounted filesystem
        return 0;
    }
    //set variables to fit 
    char* disk; strcpy(disk, pathname);
    int ino = getino(pathname2);
    MINODE *mountPoint = iget(dev, ino);
    //check if filesys is already mounted
    for (int i = 0; i < 8; i++){
        if(strcmp(disk, mountTable[i].name) == 0){
            printf("ERROR: filesystem %s already is mounted.\n", disk);
            return -1;
        }
    }
    int i = 0; MOUNT* mp = 0;
    while(mountTable[i].dev){
        i++;
    }
    mp = &mountTable[i];
    //open filesys for read/write
    pathname2[0] = 2; pathname2[1] = "\0"; char buf[BLKSIZE];
    dev = my_open();
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    if (sp->s_magic != 0xEF53){
      printf("ERROR: %s is not an ext2 filesystem\n", disk);
      return -1;
    }
    //verify mount point is a DIR and not busy
    if ((mountPoint->INODE.i_mode & 0xF000) != 0x4000){ // if (S_ISDIR())
      printf("ERROR: Mount Point is not a DIR\n");
      return -1;
    }
    if(mountPoint == running->cwd){
        printf("ERROR: mount point is busy\n");
        return -1;
    }
    //fill out info for new dev
    get_block(dev, 2, buf); 
    gp = (GD *)buf;
    mountTable[i].dev = root->dev;
    mountTable[i].ninodes = sp->s_inodes_count;
    mountTable[i].nblocks = sp->s_blocks_count;
    mountTable[i].bmap = gp->bg_block_bitmap;
    mountTable[i].imap = gp->bg_inode_bitmap;
    mountTable[i].iblk = gp->bg_inode_table;
    //mark as mounted and let point to mount table entry which points to mount point
    mountPoint->mounted = mountTable[i].mounted_inode = mountPoint;
    return 0;
}