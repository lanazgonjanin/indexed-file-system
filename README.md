# Indexed File System

A C program that simulates a small indexed file system in memory, including file metadata, block allocation, free-space management, and directory operations.

Developed as part of COMPSCI 3SH3 (Operating Systems) at McMaster University under Dr. Neerja Mhaskar, in collaboration with Thaneesha Sivasithambaram.

## Overview

The simulator models a file system with 64 blocks of 1 KB each. It supports creating and deleting files, listing stored files, and tracking available disk blocks.

Files use indexed allocation, where an index block stores the block numbers containing the file's data. A linked list is used to manage and reuse free blocks.

## Operating Systems Concepts

* File systems and storage management
* Indexed file allocation
* Free-space management
* File metadata and File Information Blocks (FIBs)
* Directory management
* Dynamic memory allocation
* Linked lists
* Block allocation and deallocation

## Implementation

The file system maintains a simulated disk, a root directory, file metadata, and a linked list of free blocks.

When a file is created, the required number of data blocks and an index block are allocated. The index block stores the locations of the file's data blocks. When a file is deleted, its blocks are returned to the free-block list for reuse.

## Compilation and Execution

Compile using the included Makefile:

```bash
make
```

Run the program:

```bash
./fs_indexed
```

Or compile and run with:

```bash
make run
```

To remove the compiled executable:

```bash
make clean
```

## Technologies

* **Language:** C
* **Memory Management:** Dynamic allocation, linked lists
* **Data Structures:** Structs, arrays, linked lists
* **Concepts:** File systems, indexed allocation, free-space management
* **Build Tool:** Make
