/****************************************************************************
File name:	td.h
Author:			Kevin Jo
Date:				3/4/2015
Class:			CS480
Assignment:	Top Down Parser
Purpose:		Defines header file for top down parser module.
****************************************************************************/
#ifndef TD_H_
#define TD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/pcc13defines.h"
#include "../include/lex.h"
#include "../include/quad.h"
#include "../include/bu.h"

#define MAX_IDENTIFIER 32

//ST Entry
typedef struct Entry
{
	char identifier [MAX_IDENTIFIER];
	SymbolTableType sType;
	int addressField;
	int argIndex;
	int other; //Array size or num param
	int level;
} Entry;

typedef struct tdParser
{
	Lexer *psLexer;
	ErrorHandler sErrorHandler;
	Token sCurrentToken, sPrevToken;
	FILE *pFile;
	SymbolTablePtr psSymbolTable;
	buParser *psBUParser;
	QuadFile *psQuadFile;
	bool bDebugBegin, bDebugEnd, bSymbolTable, bForLoop, bTypeCheck;
	int tempBufferIndex;
} tdParser;

void tdCreateParser (tdParser *, FILE *, Lexer *, SymbolTablePtr, QuadFile *,
									 	 buParser *);
ErrorCode tdParse (tdParser *);
void tdDebug (tdParser *, bool);
void tdLeaveDebug (tdParser *, bool);
void tdPrintDragon (FILE *);
void tdTypeCheck (tdParser *, bool);
void tdSymbolTable (tdParser *, bool);

#endif
