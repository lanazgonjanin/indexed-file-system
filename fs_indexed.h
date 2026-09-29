#ifndef FS_OS_H
#define FS_OS_H

#include <stdio.h>

#define BLOCK_SIZE 1024      // block size = 1 KB = 2^10 bytes
#define TOTAL_BLOCKS 64      // total number of blocks in disk
#define MAX_FILES 10         // maximum number of files
#define MAX_FILENAME_SIZE 32 // maximum length of file name

// define a block (one chunk of disk space)
typedef struct Block{
    unsigned char data[BLOCK_SIZE];  // array to hold the contents of the block
    int blockNumber;   // ID of the block
} Block; 

// file information block (FIB) for each file created
typedef struct FIB {
    int fibID; 
    char fileName[MAX_FILENAME_SIZE];  // stores file name
    int fileSize; 
    int blockCount;  // how many data blocks this file uses
    Block *indexBlock;  // pointer to the index block 
} FIB; 

typedef struct FreeBlockNode {
   Block *block;
   struct FreeBlockNode *next; 
} FreeBlockNode;

typedef struct FreeBlockList {
    FreeBlockNode *head;
    int count;
} FreeBlockList;

typedef struct VolumeControlBlock {
    FreeBlockList freeBlockList;  // free block singly linked list
    Block diskBlocks[TOTAL_BLOCKS];  // all blocks in memory; disk blocks avialable in the volume simulated as an array of blocls

    int fibIDStatus[MAX_FILES];  // 0 = free, 1 = used
    FIB *files[MAX_FILES];  // index of these files are the FIBids unique ids for the files i the simulation

    int fileCount;  // number of files created
} VolumeControlBlock;

// file system structure containing the volume control block
typedef struct FileSystem {
    VolumeControlBlock vcb;
} FileSystem;

void initFS(void);  // initializes the file system
int createFile(const char *fileName, int size);  // creates a file in file system
void deleteFile(const char *fileName);  // deletes a file with the given file name
void listFiles(void);  // lists all the files in the flat file system

Block *allocateFreeBlock(void);  // removes a free block from the head of the free block list and returns it
void returnFreeBlock(Block *block);  // adds a free block back to the tail of the free block list
void printFreeBlocks(void);  // displays all free block numbers and the total count of free block

#endif // FS_OS_H
