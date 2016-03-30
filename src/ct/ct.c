/****************************************************************************
File name:		ct.c
Author:				Kevin Jo
Date:					2/23/15
Class:				CS480
Assignment:		Lexical Analysis
Purpose:			This is the implementation file for each of the constant table
							functions associated with the constant table module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ct.h"

#define CT_SIZE 991

/****************************************************************************
Function:			ctCreate

Description:	Creates a constant table.

Parameters:		psCT - the constant table.

Returned:			None
*****************************************************************************/
ErrorCode ctCreate (ConstantTable *psCT)
{
	ErrorCode lexError;


	if (NO_ERROR != htCreate (&(psCT->sConstantTable), htCRCFunction,
			(compareFunction) htCompareInt, CT_SIZE))
	{
		htDispose (&(psCT->sConstantTable), CT_SIZE);
		lexError = UNKNOWN_ERROR;
	}

	else
	{
		psCT->size = 0;
		lexError = NO_ERROR;
	}

	return lexError;
}

/****************************************************************************
Function:			ctSearch

Description:	Searches the constant table for a given keyValue.

Parameters:		psCT 		 - the constant table.
							keyValue - the value to be searched
							bFound	 - the boolean that returns true if found, false if not

Returned:			DUPLICATE_ID if there the constant is already in the CT.
							otherwise, NO_ERROR
*****************************************************************************/
ErrorCode ctSearch (ConstantTable *psCt, char *keyValue, bool *bFound,
										HT_DATATYPE *pData)
{
	HT_DATATYPE htData;
	HT_ERRORCODE htError;
	ErrorCode ctError;
	int constant;
	char key [32];

	constant = atoi (keyValue);

	sprintf (key, "%d", constant);

	htError = htFindKey (&(psCt->sConstantTable), key, &htData);

	if (HT_ERROR_DATA_NOT_FOUND == htError)
	{
		(*bFound) = false;
		ctError = NO_ERROR;
	}

	else
	{
		(*bFound) = true;
		(*pData) = htData;
		ctError = DUPLICATE_ID;
	}

	return ctError;
}

/****************************************************************************
Function:			ctInsert

Description:	Inserts a constant to the constant table with a given address.

Parameters:		psCT 		 - the constant table.
							keyValue - the value to be searched
							address	 - the address of the constant that is inserted

Returned:			DUPLICATE_ID if there the constant is already in the CT.
							otherwise, NO_ERROR
*****************************************************************************/
ErrorCode ctInsert (ConstantTable *psCT, char *keyValue, int address)
{
	HT_DATATYPE insertData;
	HT_ERRORCODE htError;
	ErrorCode lexError;
	char key [32];

	insertData.ctData.value = atoi (keyValue);
	insertData.stData.level = 0;
	sprintf (key, "%d", insertData.ctData.value);
	insertData.key = key;
	insertData.ctData.stackAddress = address;
	insertData.ctData.keyValue = key;

	htError = htInsertKey (&(psCT->sConstantTable), insertData);

	if (HT_ERROR_INVALID_HT == htError)
	{
		lexError = UNKNOWN_ERROR;
	}

	else if (HT_DUPLICATE == htError)
	{
		lexError = DUPLICATE_ID;
	}

	else
	{
		lexError = NO_ERROR;
		psCT->size++;
	}

	return lexError;
}

/****************************************************************************
Function:			ctPrint

Description:	Prints the entire cosntant table and the addresses of the
							constants

Parameters:		psCT - the constant table.

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode ctPrint (ConstantTable *psCT)
{
	ErrorCode lexError;
	HT_DATATYPE *pCTEntry;
	HT_STATISTICS sStats;
	int index;

	htGetStats (&(psCT->sConstantTable), &sStats);

	pCTEntry = (HT_DATATYPE *) malloc (sizeof (HT_DATATYPE) *
							(sStats.numElements));

	htSort (&(psCT->sConstantTable), &pCTEntry, CT_SIZE);

	printf ("\n\n%11s", " ");
	printf ("CONSTANT TABLE\n\n");
	printf ("CONSTANT VALUE     RUNTIME STACK ADDRESS\n");
	printf ("--------------     ---------------------\n");

	for (index = 0; index < sStats.numElements; index++)
	{
		printf ("%14d     ", pCTEntry [index].ctData.value);
		printf ("%21d\n", pCTEntry [index].ctData.stackAddress);
	}

	printf ("\n\n");

	free (pCTEntry);

	lexError = NO_ERROR;

	return lexError;
}

/****************************************************************************
Function:			ctDispose

Description:	Disposes the constant table.

Parameters:		psCT 		 - the constant table.

Returned:			UNKNOWN_ERROR if the constant table cannot be disposed.
							NO_ERROR if the constant table is disposed.
*****************************************************************************/
ErrorCode ctDispose (ConstantTable *psCT)
{
	ErrorCode stError;

	if (NULL != psCT)
	{
		//htDispose (&(psCT->sConstantTable), psCT->size);
		htDispose (&(psCT->sConstantTable), psCT->sConstantTable.size);
		stError = NO_ERROR;
	}

	else
	{
		stError = UNKNOWN_ERROR;
	}

	return stError;
}
