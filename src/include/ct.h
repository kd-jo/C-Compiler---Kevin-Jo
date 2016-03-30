/****************************************************************************
File name:	ct.h
Author:			Kevin Jo
Date:				2/23/15
Class:			CS480
Assignment:	Lexical Analyzer
Purpose:		Defines header file for the constant table module.
****************************************************************************/
#ifndef CT_H_
#define CT_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/hashtable.h"
#include "../include/pcc13defines.h"

typedef struct ConstantTable
{
	HashTable sConstantTable;
	int size;

} ConstantTable;

ErrorCode ctCreate (ConstantTable *);
ErrorCode ctSearch (ConstantTable *, char *, bool *, HT_DATATYPE *);
ErrorCode ctInsert (ConstantTable *, char *, int);
ErrorCode ctPrint (ConstantTable *);
ErrorCode ctDispose (ConstantTable *);


#endif
