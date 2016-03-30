/****************************************************************************
File name:	quad.h
Author:			Kevin Jo
Date:				4/1/15
Class:			CS480
Assignment:	Semantic Actions 1
Purpose:		Defines header file for the quad module.
****************************************************************************/
#ifndef QUAD_H_
#define QUAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/pcc13defines.h"
#include "../include/rts.h"

#define MAX_OPERANDS 3
#define MAX_ADDRESS_MODE 3
#define MAX_IDENTIFIER 32

typedef struct Quad
{
	int level;
	int opCode;
	int operands [MAX_OPERANDS];
	int addressMode [MAX_ADDRESS_MODE];
	int numFunctionArgs;
	int numLocalVars;
	int argIndex;
	int localVarsIndex;
} Quad;

typedef struct QuadFile
{
	Quad sQuadFile [MAX_QUADS];
	int quadLevel;
	int localVarAddress;
	int argIndex;
	bool bQuadDump;
} QuadFile;

void quadFileCreate (QuadFile *);
void quadInit (Quad *);
void quadInsert (QuadFile *, Quad);
void quadPrint (QuadFile *, RuntimeStack, FILE *);
void quadGetCurrent (QuadFile *, Quad *);
void quadUpdateCurrent (QuadFile *, Quad);
void quadPatchFuncBegin (QuadFile *, int, int);
void quadPatchJumps (QuadFile *, int, int);
void quadQuadDump (QuadFile *, bool);

#endif /* QUAD_H_ */
