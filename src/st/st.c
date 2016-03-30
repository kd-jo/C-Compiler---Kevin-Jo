/****************************************************************************
File name:		st.c
Author:				Kevin Jo
Date:					2/11/15
Class:				CS480
Assignment:		Symbol Table
Purpose:			This is the implementation file for each of the symbol table
							functions associated with the symbol table module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/st.h"
#include "../include/pcc13defines.h"

/****************************************************************************
Function:			stCreateSymbolTable

Description:	Creates a symbol table given the size and initial level.

Parameters:		psSymbolTable 	- the symbol table
							htSize					- the size of the symbol table
							initLevel				- the initial level

Returned:			UNKNOWN_ERROR if the symbol table cannot be created, otherwise
							NO_ERROR
*****************************************************************************/
ErrorCode stCreateSymbolTable (SymbolTablePtr psSymbolTable, int htSize,
															 int initLevel)
{
	ErrorCode stError;

	if (NO_ERROR != htCreate (&(psSymbolTable->sTheSymbolTable),
			htCRCFunction, (compareFunction) htCompareAlpha, htSize))
	{
		htDispose (&(psSymbolTable->sTheSymbolTable), htSize);
		stError = UNKNOWN_ERROR;
	}

	psSymbolTable->currentLevel = initLevel;
	psSymbolTable->size = htSize;

	stError = NO_ERROR;

	return stError;
}

/****************************************************************************
Function:			stDisposeSymbolTable

Description:	Disposes a symbol table given the size.

Parameters:		psSymbolTable 	- the symbol table
							htSize					- the size of the symbol table

Returned:			UNKNOWN_ERROR if the symbol table cannot be disposed, otherwise
							NO_ERROR
*****************************************************************************/
ErrorCode stDisposeSymbolTable (SymbolTablePtr psSymbolTable, int htSize)
{
	ErrorCode stError;

	if (NULL != psSymbolTable)
	{
		htDispose (&(psSymbolTable->sTheSymbolTable), htSize);
		stError = NO_ERROR;
	}

	else
	{
		stError = UNKNOWN_ERROR;
	}

	return stError;
}

/****************************************************************************
Function:			stInsertName

Description:	Inserts an identifier to the symbol table.

Parameters:		psSymbolTable 	- the symbol table
							identifier			- the identifer to be added
							type						- the type of identifier
							address					- the address of the identifer
							paramSize				- the parameter size if needed

Returned:			UNKNOWN_ERROR if the identifer cannot be added.
							DUPLICATE_ERROR when trying to add a identifer that already
							exists in the same level.
							NO_ERROR otherwise
*****************************************************************************/
ErrorCode stInsertName (SymbolTablePtr psSymbolTable, char* identifier,
												SymbolTableType type, int address, int arraySize,
												int paramSize)
{
	HT_DATATYPE insertData;
	HT_ERRORCODE htError;
	ErrorCode stError;

	insertData.key = identifier;
	insertData.stData.level = psSymbolTable->currentLevel;
	insertData.stData.idType = type;
	insertData.stData.idAddressField = address;
	insertData.stData.arrayDim = arraySize;
	insertData.stData.paramCount = paramSize;

	htError = htInsertKey (&(psSymbolTable->sTheSymbolTable), insertData);

	if (HT_ERROR_INVALID_HT == htError)
	{
		stError = UNKNOWN_ERROR;
	}

	else if (HT_DUPLICATE == htError)
	{
		stError = DUPLICATE_ID;
	}

	else
	{
		stError = NO_ERROR;
	}

	return stError;
}

/****************************************************************************
Function:			stDeleteLevel

Description:	Deletes all entries in the current level.

Parameters:		psSymbolTable 	- the symbol table

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode stDeleteLevel (SymbolTablePtr psSymbolTable)
{
	ErrorCode stError;
	DATATYPE sTempData;
	bool bIsEmpty;
	int index, index1, size;

	for (index = 0; index < psSymbolTable->size; index++)
	{
		lstIsEmpty (&(psSymbolTable->sTheSymbolTable.psListArray [index]),
										&bIsEmpty);

		if (false == bIsEmpty)
		{
			lstLast (&(psSymbolTable->sTheSymbolTable.psListArray [index]),
								 &sTempData);
			lstSize (&(psSymbolTable->sTheSymbolTable.psListArray [index]),
									&size);

			for (index1 = 0; index1 < size; index1++)
			{
				if (sTempData.htData.stData.level == psSymbolTable->currentLevel)
				{
					lstDeleteCurrent (&(psSymbolTable->sTheSymbolTable.psListArray
														[index]), &sTempData);
					htDecrementNumElements (&(psSymbolTable->sTheSymbolTable));
				}

				else
				{
					lstPrev(&(psSymbolTable->sTheSymbolTable.psListArray [index]),
											&sTempData);
				}

				lstPeek (&(psSymbolTable->sTheSymbolTable.psListArray [index]),
										&sTempData);
			}
		}
	}

	stError = NO_ERROR;

	return stError;
}

/****************************************************************************
Function:			stIncrementLevel

Description:	Increments the global level

Parameters:		psSymbolTable 	- the symbol table

Returned:			UNKNOWN_ERROR if the symbol table level cannot be incremented,
 	 	 	 	 	 	 	otherwise NO_ERROR
*****************************************************************************/
ErrorCode stIncrementLevel (SymbolTablePtr psSymbolTable)
{
	ErrorCode stError;

	if (psSymbolTable->currentLevel >= -1 && psSymbolTable->currentLevel < 2)
	{
		psSymbolTable->currentLevel++;
		stError = NO_ERROR;
	}

	else
	{
		stError = UNKNOWN_ERROR;
	}

	return stError;
}

/****************************************************************************
Function:			stDecrementLevel

Description:	Deletes the current level, then decrements the global level

Parameters:		psSymbolTable 	- the symbol table

Returned:			UNKNOWN_ERROR if the symbol table level cannot be decremented,
 	 	 	 	 	 	 	otherwise NO_ERROR
*****************************************************************************/
ErrorCode stDecrementLevel (SymbolTablePtr psSymbolTable)
{
	ErrorCode stError;

	if (psSymbolTable->currentLevel > -1)
	{
		//stDeleteLevel (psSymbolTable);
		psSymbolTable->currentLevel--;
		stError = NO_ERROR;
	}

	else
	{
		stError = UNKNOWN_ERROR;
	}

	return stError;
}

/****************************************************************************
Function:			stSearchEntry

Description:	Searches for an entry in the symbol table given the identifier.
							The pointer to the entry is returned through arguments.

Parameters:		psSymbolTable 	- the symbol table
							identifier			- the identifier to be searched
							hsSTData				- the pointer to the data that is returned

Returned:			UNDECLARED_IDENTIFIER if the identifier is not in the symbol
							table otherwise, NO_ERROR
*****************************************************************************/
ErrorCode stSearchEntry (SymbolTablePtr psSymbolTable, char *identifier,
												 HT_DATATYPE **hsSTData)
{
	ErrorCode stError;
	int bucketAddress;
	DATATYPE data;
	DATATYPE *psData;
	bool bHasPrev;

	data.htData.key = '\0';

	psSymbolTable->sTheSymbolTable.htFunction (identifier, &bucketAddress,
																							psSymbolTable->size);

	lstLast (&psSymbolTable->sTheSymbolTable.psListArray [bucketAddress],
						&data);
	lstPeekPointer (&psSymbolTable->sTheSymbolTable.psListArray [bucketAddress],
											&psData);
	lstPeek (&psSymbolTable->sTheSymbolTable.psListArray [bucketAddress],
							&data);
	lstHasPrev (&(psSymbolTable->sTheSymbolTable.psListArray [bucketAddress]),
									&bHasPrev);

	if (NULL != data.htData.key)
	{
		while ((0 != strcmp (data.htData.key, identifier)) && (true == bHasPrev))
		{
			lstPrev (&(psSymbolTable->sTheSymbolTable.psListArray [bucketAddress]),
								&data);
			lstPeekPointer (&psSymbolTable->sTheSymbolTable.psListArray
													[bucketAddress], &psData);
			lstPeek (&psSymbolTable->sTheSymbolTable.psListArray [bucketAddress],
									&data);
			lstHasPrev (&(psSymbolTable->sTheSymbolTable.psListArray
											[bucketAddress]), &bHasPrev);
		}

		if (0 == strcmp (identifier, data.htData.key))
		{
			(*hsSTData) = &(psData->htData);
			stError = NO_ERROR;
		}

		else
		{
			(*hsSTData) = NULL;
		}
	}

	else
	{
		stError = UNDECLARED_IDENTIFIER;
	}

	return stError;
}

ErrorCode stAddResWords (SymbolTablePtr psSymbolTable)
{
	ErrorCode stError;

	stError = stInsertName (psSymbolTable, "main", RSRVWRD, RSRVWRD_MAIN,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "int", RSRVWRD, RSRVWRD_INT,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "if", RSRVWRD, RSRVWRD_IF,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "else", RSRVWRD, RSRVWRD_ELSE,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "return", RSRVWRD,
													RSRVWRD_RETURN, UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "for", RSRVWRD, RSRVWRD_FOR,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "input", RSRVWRD, RSRVWRD_INPUT,
													UNINITIALIZED, UNINITIALIZED);
	stError = stInsertName (psSymbolTable, "output", RSRVWRD,
													RSRVWRD_OUTPUT, UNINITIALIZED, UNINITIALIZED);

	stIncrementLevel (psSymbolTable);

	return stError;
}

/****************************************************************************
Function:			stPrintAll

Description:	Prints the all the entrys in the symbol table in alphabetically
							ascending order by ascii. If there are the same identifiers in
							different levels, then the most local identifier prints first.

Parameters:		psSymbolTable 	- the symbol table

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode stPrintAll (SymbolTablePtr psSymbolTable)
{
	ErrorCode stError;
	HT_DATATYPE *pSTEntry;
	HT_STATISTICS sStats;
	int index;

	htGetStats (&(psSymbolTable->sTheSymbolTable), &sStats);

	pSTEntry = (HT_DATATYPE *) malloc (sizeof (HT_DATATYPE) *
							(sStats.numElements));

	htSort (&(psSymbolTable->sTheSymbolTable), &pSTEntry,
						psSymbolTable->size);

	printf ("\n");

	for (index = 0; index < sStats.numElements; index++)
	{
		printf ("%33s", pSTEntry [index].key);
		printf ("%7d", pSTEntry [index].stData.level);
		printf ("%11d", pSTEntry [index].stData.idType);
		printf ("%12d", pSTEntry [index].stData.idAddressField);
		printf ("%16d\n", pSTEntry [index].stData.arrayDim);
	}

	printf ("\n");

	free (pSTEntry);

	stError = NO_ERROR;

	return stError;
}

/****************************************************************************
Function:			stPrintEntry

Description:	Prints a single entry given through the argument list.

Parameters:		pstEntry - the symbol table entry

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode stPrintEntry (HT_DATATYPE *pstEntry)
{
	ErrorCode stError = NO_ERROR;

	printf ("\n");
	printf ("%33s", pstEntry->key);
	printf ("%7d", pstEntry->stData.level);
	printf ("%11d", pstEntry->stData.idType);
	printf ("%12d", pstEntry->stData.idAddressField);
	printf ("%16d\n\n", pstEntry->stData.arrayDim);

	return stError;
}

/****************************************************************************
Function:			stPrintHeader

Description:	Prints the header to the symbol table.

Parameters:		None

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode stPrintHeader ()
{
	ErrorCode stError = NO_ERROR;
	int index;

	printf ("%39s%12s", "S Y M B O L", "T A B L E\n");
	printf ("%40s%10s%12s%16s\n", "Identifier  Level", "Type", "Address",
					"Other");

	for (index = 0; index < 79; index++)
	{
		printf ("-");
	}

	return stError;
}
