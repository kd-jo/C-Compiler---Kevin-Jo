/****************************************************************************
 File name:		st_driver.c
 Author:			Kevin Jo
 Date:				2/11/15
 Class:				CS480
 Assignment: 	Symbol Table
 Purpose:			Adds reserved words, and also reads from file to print, delete,
							add, search for identifier.
 Hours:				25
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/st.h"
#include "../include/er.h"
#include "../include/pcc13defines.h"

#define ST_SIZE 991
#define MAX_IDENTIFIER 1024
#define MAX_ID_SIZE 32

extern char gszErrors [ERROR_TBL_SIZE][MAX_ERROR_LINE];

/****************************************************************************
 Function:    main

 Description:	A driver to test the symbol table module.

 Parameters:	argc - number of command line arguments
 	 	 	 	 	 		argv - the command line argument

 Returned:    EXIT STATUS
****************************************************************************/
int main (int argc, char *argv[])
{
	SymbolTable sSymbolTable;
	ErrorHandler sErrorHandler;
	ErrorCode stError;
	HT_DATATYPE *hsEntry;
	int commandCode, idSize;
	char identifier [MAX_IDENTIFIER];

	LOAD_ERRORS

	FILE *pFile = fopen (argv [1], "r");

	stError = stCreateSymbolTable (&sSymbolTable, ST_SIZE, RSRVWRD_LEVEL);
	erCreateErrorHandler (&sErrorHandler);

	if (NULL == pFile)
	{
		stError = CANNOT_OPEN_FILE;
		erAddError (&sErrorHandler, stError, argv [1]);
		erPrintEntry (&sErrorHandler, stError, argv [1]);
	}

	else
	{
		stError = stInsertName (&sSymbolTable, "main", RSRVWRD, RSRVWRD_MAIN,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "int", RSRVWRD, RSRVWRD_INT,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "if", RSRVWRD, RSRVWRD_IF,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "else", RSRVWRD, RSRVWRD_ELSE,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "return", RSRVWRD,
														RSRVWRD_RETURN, UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "for", RSRVWRD, RSRVWRD_FOR,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "input", RSRVWRD, RSRVWRD_INPUT,
														UNINITIALIZED, UNINITIALIZED);
		stError = stInsertName (&sSymbolTable, "output", RSRVWRD,
														RSRVWRD_OUTPUT, UNINITIALIZED, UNINITIALIZED);
		stIncrementLevel (&sSymbolTable);
		erIncrementLine (&sErrorHandler);

		while (!feof (pFile))
		{
			commandCode = fgetc (pFile);
			erIncrementLine (&sErrorHandler);

			if (PRINT == commandCode)
			{
				commandCode = fgetc (pFile); //Used to take up '\n'
				stPrintHeader ();
				stPrintAll (&sSymbolTable);
			}

			else if (ADD == commandCode)
			{
				fscanf (pFile, "%s", identifier);
				commandCode = fgetc (pFile);
				idSize = strlen (identifier);

				if (idSize < MAX_ID_SIZE)
				{
					stError = stInsertName (&sSymbolTable, identifier, NONE, NO_ADDRESS,
																					OTHER, OTHER);
					erAddError (&sErrorHandler, stError, identifier);
					erPrintAll (&sErrorHandler);
					erClear (&sErrorHandler);
				}

				else
				{
					stError = INVALID_ID;
					erAddError (&sErrorHandler, stError, identifier);
					//erPrintEntry (&sErrorHandler, stError, identifier);
					erPrintAll (&sErrorHandler);
					erClear (&sErrorHandler);
				}
			}

			else if (DEC_LEVEL == commandCode)
			{
				commandCode = fgetc (pFile);
				stDecrementLevel (&sSymbolTable);
			}

			else if (INC_LEVEL == commandCode)
			{
				commandCode = fgetc (pFile);
				stIncrementLevel (&sSymbolTable);
			}

			else if (SEARCH == commandCode)
			{
				fscanf (pFile, "%s", identifier);
				commandCode = fgetc (pFile);
				idSize = strlen (identifier);

				if (idSize < MAX_ID_SIZE)
				{
					stError = stSearchEntry (&sSymbolTable, identifier, &hsEntry);
					erAddError (&sErrorHandler, stError, identifier);
					//erPrintEntry (&sErrorHandler, stError, identifier);
					erPrintAll (&sErrorHandler);
					erClear (&sErrorHandler);
				}

				else
				{
					stError = INVALID_ID;
					erAddError (&sErrorHandler, stError, identifier);
					//erPrintEntry (&sErrorHandler, stError, identifier);
					erPrintAll (&sErrorHandler);
					erClear (&sErrorHandler);
				}

				if (NO_ERROR == stError)
				{
					stPrintHeader ();
					stPrintEntry (hsEntry);
				}
			}
		}
	}

	stDisposeSymbolTable (&sSymbolTable, ST_SIZE);
	erDisposeErrorHandler (&sErrorHandler);

	if (NULL != pFile)
	{
		fclose (pFile);
	}

	return stError;
}
