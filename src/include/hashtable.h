/***********************************************************
 File name:	 hashtable.h
 Author:     Kevin Jo
 Date:       2/11/15
 Class:      CS480
 Assignment: Symbol Table
 Purpose:    This file defines the constants, data structures, and function
             prototypes for implementing a hash table data structure. In
             essence, the hash table API is defined for other modules.
 ***********************************************************/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"

typedef short int HT_ERRORCODE;

#define HT_DUPLICATE 16

#define HT_NO_ERROR 0

#define HT_ERROR_NO_HT_CREATE -1

#define HT_ERROR_EMPTY_HT -2

#define HT_ERROR_FULL_HT -3

#define HT_ERROR_INVALID_HT -9

#define HT_ERROR_NULL_PTR -10

#define HT_ERROR_DATA_NOT_FOUND -12

typedef HT_ERRORCODE (*hashFunction) (char *, int *, int);

typedef int (*compareFunction) (const void *, const void *);

typedef struct HT_STATISTICS
{
	int longestChain;
	int numElements;
	int emptyBuckets;
	double averageProbes;
} HT_STATISTICS;

typedef struct HashTable* HashTablePtr;

typedef struct HashTable
{
	int size;
	hashFunction htFunction;
	compareFunction htCompare;
	HT_STATISTICS htStats;
	ListPtr psListArray;
} HashTable;

/***********************************************************
* 											Allocation and Deallocation
***********************************************************/
HT_ERRORCODE htCreate (HashTablePtr, hashFunction, compareFunction, int);
// results: Size of list array is set. Hash function and compare function is
//						set. Values in HT_STATISTICS initialized to 0.

HT_ERRORCODE htDispose (HashTablePtr, int);
// results:	Disposes a hashtable.

/***********************************************************
* 												Insertion, Deletion, Updating
***********************************************************/
HT_ERRORCODE htInsertKey (HashTablePtr, HT_DATATYPE);
// results:	Inserts a key into the hash table using the hash function in
//					struct.

HT_ERRORCODE htDeleteKey (HashTablePtr, char *, HT_DATATYPE *, int);
// results:	Deletes the key passed in from the hash table and returns the
//					data.

HT_ERRORCODE htUpdateKey (HashTablePtr, char *);
// results:	Updates a key's data.

/**********************************************************
* 												Checking for elements
***********************************************************/
HT_ERRORCODE htFindKey (HashTablePtr, char *, HT_DATATYPE *);
// results:Searches for a key passed in through arguments and returns the data
//	through the arguments.

HT_ERRORCODE htGetStats (HashTablePtr, HT_STATISTICS *);
// results:	Returns the data in the HT_STATISTICS struct.

HT_ERRORCODE htDecrementNumElements (HashTablePtr);
// results:	Decrements the number of elements in the hashtable.

HT_ERRORCODE htUpdateLongest (HashTablePtr, int);
// results: Updates the longest chain stat.

HT_ERRORCODE htSort (HashTablePtr, HT_DATATYPE **, int);
// results:	Sorts the hash table using the compare function in the hash table
//					struct.

HT_ERRORCODE htPrintHead (HashTablePtr);
// results: Prints the header, words and count in the hash table, and hash
//					table stats.

HT_ERRORCODE htMidSquare (char [], int *);
// results:	Computes the key value by summing then squaring the ASCII value
//					of the key. Then returns the bucket address by using bits 8 to
//					17 of the key value.

HT_ERRORCODE htCRCFunction (char [], int *, int);
// results: Computes a key value by using the CRC hash function.

int htCompareAlpha (const void *, const void *);
// results:	Compares the alphabets of two keys.

int htCompareInt (const void *, const void *);
// results: Compares the count of two keys.

#endif /* HASHTABLE_H_ */
