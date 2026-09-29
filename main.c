#include <stdio.h>
#include "fs_indexed.h"

int main()
{
    // initialize file system
    initFS();

    // create files
    createFile("alpha.txt", 3072);
    createFile("beta.txt", 5120);

    // print the directory
    printf("\n");
    listFiles();

    // print free blocks
    printFreeBlocks();

    // delete a file
    deleteFile("alpha.txt");

    // print the directory
    printf("\n");
    listFiles();

    // print free blocks
    printFreeBlocks();

    // create new files
    createFile("gamma.txt", 4096);
    createFile("delta.txt", 8192);

    printf("\n");

    // print free blocks
    printFreeBlocks();
    deleteFile("beta.txt");
    deleteFile("gamma.txt");
    deleteFile("delta.txt");

    printf("\n");

    // list the root directory
    listFiles();

    printf("\n");

    // list free blocks
    printFreeBlocks();
    return 0;
}