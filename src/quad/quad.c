/****************************************************************************
File name:	quad.c
Author:			Kevin Jo
Date:				4/1/15
Class:			CS480
Assignment:	Semantic Actions 1
Purpose:		Implementation of the quad module.
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/quad.h"

/****************************************************************************
Function:			quadFileCreate

Description:	Creates a quadfile and initializes the values of the quads.

Parameters:		psQuadFile - the quad file

Returned:			None
*****************************************************************************/
void quadFileCreate (QuadFile *psQuadFile)
{
	Quad tempQuad;
	int i;

	quadInit (&tempQuad);

	for (i = 0; i < MAX_QUADS; i++)
	{
		psQuadFile->sQuadFile [i] = tempQuad;
	}

	psQuadFile->quadLevel = 0;
	psQuadFile->localVarAddress = 0;
	psQuadFile->argIndex = 0;
	psQuadFile->bQuadDump = false;
}

/****************************************************************************
Function:			quadInit

Description:	Initializes a quad.

Parameters:		psQuad - the quad

Returned:			None
*****************************************************************************/
void quadInit (Quad *psQuad)
{
	int i;

	psQuad->level = 0;
	psQuad->opCode = 0;

	for (i = 0; i < 3; i++)
	{
		psQuad->addressMode [i] = IMMEDIATE;
		psQuad->operands [i] = OP_NOP;
	}

	psQuad->argIndex = -2;
	psQuad->localVarsIndex = 0;
	psQuad->numFunctionArgs = 0;
	psQuad->numLocalVars = 0;
}

/****************************************************************************
Function:			quadInsert

Description:	Inserts a quad into the quadfile.

Parameters:		psQuadFile - the quad file
							sQuad			 - the quad to be inserted.

Returned:			None
*****************************************************************************/
void quadInsert (QuadFile *psQuadFile, Quad sQuad)
{
	psQuadFile->sQuadFile [psQuadFile->quadLevel] = sQuad;
	psQuadFile->quadLevel++;
}

/****************************************************************************
Function:			quadPrint

Description:	Prints the quadfile and RTS and outputs to a file as well.

Parameters:		psQuadFile - the quad file
							sRTS			 - the runtime stack
							pWriteFile - the output file

Returned:			None
*****************************************************************************/
void quadPrint (QuadFile *psQuadFile, RuntimeStack sRTS, FILE *pWriteFile)
{
	int i;

	if (psQuadFile->bQuadDump == true)
	{
		printf ("Quad File\n");
		printf ("---------\n");
	}

	for (i = 0; i < psQuadFile->quadLevel; i++)
	{
		if (psQuadFile->bQuadDump == true)
		{
			printf ("%d %3d %2d%6d %d%6d %d%6d\n",
							psQuadFile->sQuadFile [i].level,
							psQuadFile->sQuadFile [i].opCode,
							psQuadFile->sQuadFile [i].addressMode [0],
							psQuadFile->sQuadFile [i].operands [0],
							psQuadFile->sQuadFile [i].addressMode [1],
							psQuadFile->sQuadFile [i].operands [1],
							psQuadFile->sQuadFile [i].addressMode [2],
							psQuadFile->sQuadFile [i].operands [2]);
		}

		fprintf (pWriteFile, "%d %3d %2d%6d %d%6d %d%6d\n",
						psQuadFile->sQuadFile [i].level,
						psQuadFile->sQuadFile [i].opCode,
						psQuadFile->sQuadFile [i].addressMode [0],
						psQuadFile->sQuadFile [i].operands [0],
						psQuadFile->sQuadFile [i].addressMode [1],
						psQuadFile->sQuadFile [i].operands [1],
						psQuadFile->sQuadFile [i].addressMode [2],
						psQuadFile->sQuadFile [i].operands [2]);
	}

	if (psQuadFile->bQuadDump == true)
	{
		printf ("%d\n%d\n", sRTS.end, 0);
	}

	fprintf (pWriteFile, "%d\n%d\n", sRTS.end, 0);

	for (i = 1; i < sRTS.end; i++)
	{
		if (psQuadFile->bQuadDump == true)
		{
			printf ("%d\n", sRTS.runtimeStack [i]);
		}

		fprintf (pWriteFile, "%d\n", sRTS.runtimeStack [i]);
	}


}

/****************************************************************************
Function:			quadGetCurrent

Description:	Gets the current quad

Parameters:		psQuadFile 	 - the quad file
							psReturnQuad - the current quad file to be returned

Returned:			None
*****************************************************************************/
void quadGetCurrent (QuadFile *psQuadFile, Quad *psReturnQuad)
{
	(*psReturnQuad) = psQuadFile->sQuadFile [psQuadFile->quadLevel - 1];
}

/****************************************************************************
Function:			quadUpdateCurrent

Description:	Updates the current quad

Parameters:		psQuadFile 	 - the quad file
							sQuad 			 - the quad to update the current quad

Returned:			None
*****************************************************************************/
void quadUpdateCurrent (QuadFile *psQuadFile, Quad sQuad)
{
	psQuadFile->sQuadFile [psQuadFile->quadLevel - 1] = sQuad;
}

/****************************************************************************
Function:			quadPatchFuncBegin

Description:	Patches the function begin quad.

Parameters:		psQuadFile 	 - the quad file
							opCode			 - the opCode for function begin
							numLocals		 - the number of locals to backpatch

Returned:			None
*****************************************************************************/
void quadPatchFuncBegin (QuadFile *psQuadFile, int opCode, int numLocals)
{
	Quad sTempQuad;
	int tempIndex;

	tempIndex = psQuadFile->quadLevel;
	quadGetCurrent (psQuadFile, &sTempQuad);

	if (sTempQuad.opCode == opCode)
	{
		sTempQuad.operands [0] += numLocals;

		psQuadFile->sQuadFile [tempIndex - 1] = sTempQuad;
	}

	else
	{
		while (sTempQuad.opCode != opCode && sTempQuad.level == sTempQuad.level)
		{
			sTempQuad = psQuadFile->sQuadFile [tempIndex];
			tempIndex--;
		}

		if (sTempQuad.opCode)
		{
			sTempQuad.operands [0] += numLocals;

			psQuadFile->sQuadFile [tempIndex + 1] = sTempQuad;
		}
	}
}

/****************************************************************************
Function:			quadPatchJumps

Description:	Patches the branch quads

Parameters:		psQuadFile 	 - the quad file
							quadLevel		 - the level of the quad to patch
							operand			 - the operand to patch with

Returned:			None
*****************************************************************************/
void quadPatchJumps (QuadFile *psQuadFile, int quadLevel, int operand)
{
	Quad sTempQuad;

	sTempQuad = psQuadFile->sQuadFile [quadLevel];

	sTempQuad.operands [2] = operand;
	sTempQuad.addressMode [2] = RSRVWRD_LEVEL;

	psQuadFile->sQuadFile [quadLevel] = sTempQuad;
}

/****************************************************************************
Function:			quadQuadDump

Description:	Sets the option to print the quad.

Parameters:		psQuadFile 	 - the quad file
							option			 - the option to print or not.

Returned:			None
*****************************************************************************/
void quadQuadDump (QuadFile *psQuadFile, bool option)
{
	if (true == option)
	{
		psQuadFile->bQuadDump = true;
	}
}
