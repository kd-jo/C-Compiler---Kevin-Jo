/****************************************************************************
File name:		rts.c
Author:				Kevin Jo
Date:					2/23/15
Class:				CS480
Assignment:		Lexical Analysis
Purpose:			This is the implementation file for each of the runtime stack
							functions associated with the runtime stack module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/rts.h"

/****************************************************************************
Function:			rtsCreate

Description:	Creates a runtime stack.

Parameters:		psRTStack - the runtime stack

Returned:			NO_ERROR if the runtime stack is created.
*****************************************************************************/
ErrorCode rtsCreate (RuntimeStack *psRTStack)
{
	ErrorCode rtsError;

	psRTStack->runtimeStack [0] = -1;

	psRTStack->index = 1;
	psRTStack->end = 1;

	psRTStack->runtimeStack [psRTStack->end] = -1;

	rtsError = NO_ERROR;

	return rtsError;
}

/****************************************************************************
Function:			rtsDisose

Description:	Disposes the runtime stack.

Parameters:		psRTStack - the runtime stack

Returned:			NO_ERROR if the runtime stack is disposed
*****************************************************************************/
ErrorCode rtsDispose (RuntimeStack *psRTStack)
{
	ErrorCode rtsError;
	int index;

	for (index = 0; index < psRTStack->end; index++)
	{
		psRTStack->runtimeStack [index]	= '\0';
	}

	rtsError = NO_ERROR;

	return rtsError;
}

/****************************************************************************
Function:			rtsInsert

Description:	Inserts a constant to the runtime stack and returns the address

Parameters:		psRTStack - the runtime stack
							value			- the constant to be inserted to the runtime stack
							address		- the address which will be returned.

Returned:			NO_ERROR if the runtime stack is created.
*****************************************************************************/
ErrorCode rtsInsert (RuntimeStack *psRTStack, int value, int *address)
{
	ErrorCode rtsError;

	if (5000 > psRTStack->end)
	{
		psRTStack->end++;
		psRTStack->runtimeStack [psRTStack->end] = psRTStack->runtimeStack [0];
		psRTStack->runtimeStack [psRTStack->index] = value;
		(*address) = psRTStack->index;
		psRTStack->index++;
		rtsError = NO_ERROR;
	}

	else
	{
		rtsError = OUT_OF_MEMORY;
	}

	return rtsError;
}

/****************************************************************************
Function:			rtsSearch

Description:	Searches the runtime stack for a constant and returns the
							address if found.

Parameters:		psRTStack - the runtime stack
							value			- the value to be searched for in the RTS
							address		- the address that will be returned if found

Returned:			NO_ERROR if the value is found.
*****************************************************************************/
ErrorCode rtsSearch (RuntimeStack *psRTStack, int value, int *address)
{
	int index = 1;
	ErrorCode rtsError;

	while (value != psRTStack->runtimeStack [index] && index < psRTStack->end)
	{
		index++;
	}

	if (value == psRTStack->runtimeStack [index])
	{
		(*address) = index;
		rtsError = NO_ERROR;
	}

	else
	{
		rtsError = UNKNOWN_ERROR;
	}

	return rtsError;
}

/****************************************************************************
Function:			rtsPrint

Description:	Prints the entire runtime stack and the addresses of constants.

Parameters:		psRTStack - the runtime stack

Returned:			NO_ERROR if the runtime stack is created.
*****************************************************************************/
ErrorCode rtsPrint (RuntimeStack *psRTStack)
{
	ErrorCode rtsError;
	int index;

	printf ("%11s", " ");
	printf ("RUNTIME STACK\n\n");
	printf ("%14s     ", "ADDRESS");
	printf ("VALUE\n");
	printf ("       -------     -----\n");

	for (index = 0; index <= psRTStack->end; index++)
	{
		printf ("%14d     ", index);
		printf ("%5d\n", psRTStack->runtimeStack [index]);
	}

	rtsError = NO_ERROR;

	return rtsError;
}
