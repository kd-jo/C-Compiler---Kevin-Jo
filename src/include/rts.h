/****************************************************************************
File name:	rts.h
Author:			Kevin Jo
Date:				2/23/15
Class:			CS480
Assignment:	Lexical Analyzer
Purpose:		Defines header file for the runtime stack module.
****************************************************************************/
#ifndef RTS_H_
#define RTS_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/pcc13defines.h"

typedef struct RuntimeStack
{
	int runtimeStack [MAX_RUNTIME + 1];
	int index, end;
} RuntimeStack;

ErrorCode rtsCreate (RuntimeStack *);
ErrorCode rtsDispose (RuntimeStack *);
ErrorCode rtsInsert (RuntimeStack *, int, int *);
ErrorCode rtsSearch (RuntimeStack *, int, int *);
ErrorCode rtsPrint (RuntimeStack *);

#endif
