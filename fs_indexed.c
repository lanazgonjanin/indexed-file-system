#include "fs_indexed.h"
#include <stdlib.h>
#include <string.h>

FileSystem fs;   // global file system instance

// Removes a free block from the head of the free block list and returns it
Block *allocateFreeBlock(void) {
    
    FreeBlockNode *headBlockNode = fs.vcb.freeBlockList.head;  // pointer to the head block of the free block list

    if (headBlockNode == NULL){
        return NULL;  // free block list is empty
    }

    fs.vcb.freeBlockList.head = headBlockNode->next;  // set head to be whatever the next block node is

    fs.vcb.freeBlockList.count --;  // decrement the number of nodes in the free block list

    Block *block = headBlockNode->block;  // get the block struct from the previous head block node

    free(headBlockNode);  // free the previous head block node

    return block;  // return the Block of the previous headBlockNode

}

// Adds a free block back to the tail of the free block list
void returnFreeBlock(Block *block) {
    // create a free block
    FreeBlockNode* freeBlock = (FreeBlockNode*)malloc(sizeof(FreeBlockNode));

    if (freeBlock == NULL) {
        printf("Error: Cannot create new block.\n");
    }

    freeBlock->block = block;  // assign block of FreeBlockNode to be what the user inputs
    freeBlock->next = NULL;  // since it's the tail, "next" is set to NULL

    // Adding the new block to the VCB
    if (fs.vcb.freeBlockList.head == NULL) {  // if the free block list is empty
        fs.vcb.freeBlockList.head = freeBlock;  // the new block is the head of the list
    } else {  // linked list traversal
        FreeBlockNode* node = fs.vcb.freeBlockList.head;
        while (node->next != NULL) node = node->next;
        node->next = freeBlock;  // add the tail once we find the last node in the list
        
    }
    fs.vcb.freeBlockList.count++;  // increment the counter
}

// Displays all free block numbers and the total count of free block (as shown in the output.txt file)
void printFreeBlocks(void) {

    int numFreeBlocks = fs.vcb.freeBlockList.count;  // get free block count

    printf("Free Blocks (%d):", numFreeBlocks);
    
    FreeBlockNode *node = fs.vcb.freeBlockList.head;  // get the head

    // Linked list traversal to print all the blocks
    while(node != NULL){
        printf(" [%d] ->", node->block->blockNumber);  
        node = node->next; 
    }
    printf("NULL\n");
}


void initFS(void) {
    // Initialize counters and arrays in volume control block
    fs.vcb.fileCount = 0;

    for (int i = 0; i < MAX_FILES; i++) {
        fs.vcb.fibIDStatus[i] = 0;
        fs.vcb.files[i] = NULL;
    }

    // Initialize free block list
    fs.vcb.freeBlockList.head = NULL;
    fs.vcb.freeBlockList.count = 0;

    // Intialize disk blocks
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        fs.vcb.diskBlocks[i].blockNumber = i;  // give each block a number

        // Push each block into free block list
        returnFreeBlock(&fs.vcb.diskBlocks[i]);
    }

    // Check that total number of blocks created matches the number of blocks we wanted to create
    if (fs.vcb.freeBlockList.count == TOTAL_BLOCKS) {
        printf("File system initialized: %d blocks, block size %d bytes.\n", TOTAL_BLOCKS, BLOCK_SIZE);
    } else {
        printf("Error: file system initialization failed.\n");
    }
}

// Go through status of all FIB slots to find the first free one
int getFileInformationBlockID() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.vcb.fibIDStatus[i] == 0) {  // if we found a free FIB slot
            fs.vcb.fibIDStatus[i] = 1;  // mark this FIB slot as used 
            return i;  // return its FIB
        }
    }
    return -1;  // no free FIB slots available
}

int createFile(const char *fileName, int size) {
    
    // 1. Check that the total number of files does not exceed the maximum allowed
    if (fs.vcb.fileCount >= MAX_FILES) {
        printf("Error: maximum number of files (%d) reached.\n", MAX_FILES);
        return -1;
    }

    // 2. Check that the number of blocks required for the new file is ≤ free blocks available 
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;  // ceil(fileSize/BLOCK_SIZE)
    int totalBlocksNeeded = blocksNeeded + 1;  // add 1 for index block

    if (fs.vcb.freeBlockList.count < totalBlocksNeeded) {
        printf("Error: not enough free blocks (need %d, have %d).\n", totalBlocksNeeded, fs.vcb.freeBlockList.count);
        return -1;
    }

    // If yes, allocate data blocks using the indexed allocation scheme and the allocateFreeBlock() function
    Block *dataBlocks[blocksNeeded];  // create array of pointers to Block to hold the addresses of each data block we allocate
    for (int i = 0; i < blocksNeeded; i++) {
        dataBlocks[i] = allocateFreeBlock();  // remove one block from free block list anad store it in dataBlocks[i]
        if (dataBlocks[i] == NULL) {  // if there is no block 
            // Return the blocks that we already allocated
            for (int j = 0; j < i; j++) {
                returnFreeBlock(dataBlocks[j]);
            }
            printf("Error: failed to allocate data blocks for file '%s'.\n", fileName);  // didn't get enough blocks needed for the file
            return -1;
        }
    }

    // If yes, also need to allocate an index block to store pointers to data block
    Block *indexBlock = allocateFreeBlock();  // get the free block
    if (indexBlock == NULL) {  // if there is no block
        // Return the blocks we already allocated
        for (int i = 0; i < blocksNeeded; i++) {
            returnFreeBlock(dataBlocks[i]);
        }
        printf("Error: failed to allocate index block for file '%s'.\n", fileName);  // couldn't get index block therefore can't get enough blocks for file
        return -1;
    }

    // Store data block numbers in the index block
    int *indexData = (int *) indexBlock->data;  // cast index block as array of integers
    for (int i = 0; i < blocksNeeded; i++) {
        indexData[i] = dataBlocks[i]->blockNumber;  // store the block number of each data block in the index block
    }

    // 4. Allocate a file information block ID to the new file by creating the function getFileInformationBlockID() to achieve this task
    int fibID = getFileInformationBlockID();
    if (fibID == -1) {
        printf("Error: there are no free file informtion block IDs.\n");
        return -1;
    }

    // 5. Update file system’s File Information Block available in the file system (creating a file block for the file)
    FIB *fib = (FIB *) malloc(sizeof(FIB));
    fib->fibID = fibID;
    strncpy(fib->fileName, fileName, MAX_FILENAME_SIZE);
    fib->fileSize = size;
    fib->blockCount = blocksNeeded;
    fib->indexBlock = indexBlock;

    fs.vcb.files[fibID] = fib;  // assign file list to be the file block that we just created
    fs.vcb.fileCount++;  // increment number of files

    // Success message
    printf("File created: '%s', %d bytes, %d data blocks, FIBID=%d\n", fib->fileName, fib->fileSize, fib->blockCount, fib->fibID);

    return 0;

}


void deleteFile(const char *fileName) {
    int fibIndex = -1;  // variable to indicate index of the file we're searching for
    FIB *fib = NULL;  // points to the FIB of the file we're searching for

    // 1. Find the FIB of the file we're searching for
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.vcb.files[i] != NULL && strcmp(fs.vcb.files[i] -> fileName, fileName) == 0) {   // checks if the file name stored in the FIB matches the name of the file we're searching for
            fib = fs.vcb.files[i];  // pointer to the file's FIB
            fibIndex = i;  // update the index to that of the file
            break;
        }
    }

    // Error case to handle the file not being found
    if (fib == NULL) {
        printf("Error: file '%s' not found.\n", fileName);
        return;
    }

    // 2. Return all the blocks used by the file, including the index block, to the free block list using the returnFreeBlock()
    int *indexData = (int *) fib->indexBlock->data;

    for (int i = 0; i < fib->blockCount; i++) {  // iterate over how many data blocks the file uses
        int blockNum = indexData[i];

        if (blockNum >= 0 && blockNum < TOTAL_BLOCKS) {  // check that it is a valid block number
            returnFreeBlock(&fs.vcb.diskBlocks[blockNum]);  // return the block to the free block list
        }
    }

    returnFreeBlock(fib->indexBlock);  // return the index block to the free block list

    // 3. Return the FIB ID to the file system, and update the FIB IDs available in the file system
    fs.vcb.fibIDStatus[fib->fibID] = 0;
    fs.vcb.files[fibIndex] = NULL;
    fs.vcb.fileCount--;

    // Success message
    printf("File '%s' deleted successfully (FIBID=%d).\n", fib->fileName, fib->fibID);

    free(fib);  // free the FIB slot
}

void listFiles(void) {
    printf("Root Directory Listing (%d files):\n", fs.vcb.fileCount);

    // Case when there are no files
    if (fs.vcb.fileCount == 0) {
        printf("No files\n");
        return;
    }

    // Go through all the files
    for (int i = 0; i < MAX_FILES; i++) {

        if (fs.vcb.fibIDStatus[i] == 1 && fs.vcb.files[i] != NULL) {  // if fib is being used and has a valid fib pointer
            FIB *fib = fs.vcb.files[i];  // get the FIB struct data from the fib
            printf("%s | %d bytes | %d data blocks | FIBID=%d\n", fib->fileName, fib->fileSize, fib->blockCount, fib->fibID);
        }
    }
}