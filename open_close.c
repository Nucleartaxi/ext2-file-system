#include "globals.h"
#include "util.h"
#include "mkdir_creat.h"
#include <stdio.h>

int my_open() {
    int ino = getino(pathname); //gets ino of pathname
    printf("ino=%d\n", ino);
    if (ino==0) { //if ino doesn't exist, creat it
        printf("ino=%d does not exist. Creating %s\n", ino, pathname);
        my_creat(); //creates the file 
        ino = getino(pathname); //gets the ino of the file
    }
    MINODE* mip = iget(dev, ino);
    printf("ino=%d\n", mip->ino);
}
int my_close() {

}
int my_lseek() {

}