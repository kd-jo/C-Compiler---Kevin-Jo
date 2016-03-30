/****************************************************************************
File name:		er.h
Author:				Kevin Jo
Date:					2/11/15
Class:				CS480
Assignment:		Symbol Table
Purpose:			Defines header file for error handler module.
****************************************************************************/
#ifndef ER_H_
#define ER_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"

typedef struct ErrorHandler* ErrorHandlerPtr;
typedef struct ErrorHandler
{
	int numErrors;
	int lineNumber;
	ListPtr psErrorList;
} ErrorHandler;

void erCreateErrorHandler (ErrorHandlerPtr);
void erDisposeErrorHandler (ErrorHandlerPtr);
void erAddError (ErrorHandlerPtr, ErrorCode, char *);
void erClear (ErrorHandlerPtr);
void erIncrementLine (ErrorHandlerPtr);
void erGetCurrentEntry (ErrorHandlerPtr, ER_DATATYPE *);
void erPrintEntry (ErrorHandlerPtr, ErrorCode, char *);
void erPrintAll (ErrorHandlerPtr);

#endif
