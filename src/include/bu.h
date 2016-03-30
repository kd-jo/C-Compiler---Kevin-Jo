/****************************************************************************
File name:	bu.h
Author:			Kevin Jo
Date:				4/20/15
Class:			CS480
Assignment:	Buttom Up Parser
Purpose:		Defines header file for the Bottom Up Parser module.
****************************************************************************/
#ifndef BU_H_
#define BU_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/pcc13defines.h"
#include "../include/lex.h"
#include "../include/er.h"
#include "../include/ct.h"
#include "../include/rts.h"
#include "../include/quad.h"

#define MAX_OPERATORS 2048
#define EMPTYSTRING " "

typedef struct buParser
{
	Lexer *psLexer;
	Token *sBuStack;
	Token *sInputBuffer;
	Token sCurrentToken;
	Token sTopStack;
	ErrorHandler *psErrorHandler;
	QuadFile *psQuadFile;
	QuadFile sTempQuad;
	SymbolTablePtr psST;
	int inputSize;
	int stackIndex;
	int inputIndex;
	bool bDebug, bLexDebug, bFullExp, bArguments, bInitExp, bAddressOp, bOutput;
} buParser;

void buCreate (buParser *, Lexer *, FILE *, ConstantTable *, RuntimeStack *,
							 ErrorHandler *, SymbolTablePtr);
void buDebug (buParser *, bool);
void buFillInput (buParser *);
void buInitInput (buParser *);
void buParse (buParser *);
void buShift (buParser *);
void buReduce (buParser *, Token, Token, int);
void buPrintStack (buParser *);
void buInitStack (buParser *);
void buDispose (buParser *);
void buAddQuad (buParser *, Token *, Token *, Token *, Quad);
void buRelop (buParser *, Token, Token, Token *, int);
void buBoolop (buParser *, Token, Token, Token *, int);
void buEquop (buParser *, Token, Token, Token *, int);
void buSetExp (buParser *);
void buSetInitExp (buParser *);

#endif
