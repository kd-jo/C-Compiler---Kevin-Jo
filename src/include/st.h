/****************************************************************************
File name:	st.h
Author:			Kevin Jo
Date:				2/11/15
Class:			CS480
Assignment:	Symbol Table
Purpose:		Defines header file for symbol table module.
****************************************************************************/
#ifndef ST_H_
#define ST_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/hashtable.h"
#include "../include/pcc13defines.h"

#define ADD 'A'
#define PRINT 'P'
#define INC_LEVEL 'L'
#define SEARCH 'S'
#define DEC_LEVEL 'D'
#define NO_ADDRESS -1
#define OTHER -1
#define UNINITIALIZED -1
#define ST_SIZE 991

typedef struct SymbolTable* SymbolTablePtr;
typedef struct SymbolTable
{
	HashTable sTheSymbolTable;
	int size;
	int currentLevel;
} SymbolTable;

ErrorCode stCreateSymbolTable (SymbolTablePtr, int, int);
ErrorCode stDisposeSymbolTable (SymbolTablePtr, int);
ErrorCode stInsertName (SymbolTablePtr, char *, SymbolTableType, int, int,
												int);
ErrorCode stDeleteLevel (SymbolTablePtr);
ErrorCode stIncrementLevel (SymbolTablePtr);
ErrorCode stDecrementLevel (SymbolTablePtr);
ErrorCode stSearchEntry (SymbolTablePtr, char *, HT_DATATYPE **);
ErrorCode stAddResWords (SymbolTablePtr);
ErrorCode stPrintAll (SymbolTablePtr);
ErrorCode stPrintEntry (HT_DATATYPE *);
ErrorCode stPrintHeader ();

#endif /* ST_H_ */
