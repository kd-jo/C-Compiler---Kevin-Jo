/****************************************************************************
 File name:		lex_driver.c
 Author:			Kevin Jo
 Date:				2/23/15
 Class:				CS480
 Assignment: 	Lexical Analysis
 Purpose:			Adds reserved words, reads from file to produce tokens, stores
 	 	 	 	 	 	  constants in the constant table and the address of that
 	 	 	 	 	 	  constant is kept in the runtime stack.
 Hours:				30
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/st.h"
#include "../include/er.h"
#include "../include/pcc13defines.h"
#include "../include/lex.h"
#include "../include/ct.h"
#include "../include/rts.h"

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
	Lexer sLexer;
	ConstantTable sConstantTable;
	RuntimeStack sRuntimeStack;
	Token theToken;
	SymbolTable sSymbolTable;
	ErrorHandler sErrorHandler;
	ErrorCode stError;

	LOAD_ERRORS

	FILE *pFile = fopen (argv [1], "r");

	stError = stCreateSymbolTable (&sSymbolTable, ST_SIZE, RSRVWRD_LEVEL);
	/*
	ctCreate (&sConstantTable);
	rtsCreate (&sRuntimeStack);
	*/
	erCreateErrorHandler (&sErrorHandler);
	erIncrementLine (&sErrorHandler);
	erIncrementLine (&sErrorHandler);

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

		lexCreate (&sLexer, pFile, &sConstantTable, &sRuntimeStack, &sSymbolTable);
		theToken.lexeme [0] = ' ';
		theToken.class = 0;
		//lexFillBuffer (&sLexer);

		if (true == sLexer.bDebug)
		{
			lexDebug (&sLexer);
		}

		while (!feof (pFile) || '\0' != theToken.lexeme [0])
		{
			//theToken = lexGetToken (&sLexer, &stError, &sErrorHandler);
			erAddError (&sErrorHandler, stError, theToken.lexeme);

			/*
			if (CONSTANT == theToken.type)
			{
				ctSearch (&sConstantTable, theToken.lexeme, &bFound);

				if (false == bFound)
				{
					lexError = rtsInsert (&sRuntimeStack, atoi (theToken.lexeme), &stackAddress);

					if (OUT_OF_MEMORY != lexError)
					{
						ctInsert (&sConstantTable, theToken.lexeme, stackAddress);
						theToken.value.constant = stackAddress;
					}

					else
					{
						erAddError (&sErrorHandler, lexError, theToken.lexeme);
						theToken.lexeme [0] = ' ';
					}
				}

				else
				{
					rtsSearch (&sRuntimeStack, atoi (theToken.lexeme), &stackAddress);
					theToken.value.constant = stackAddress;
				}
			}
			*/

			if (('\n' == theToken.lexeme [0]) && (true == sLexer.bDebug))
			{
				lexDebug (&sLexer);
			}

			else if ('\0' != theToken.lexeme [0] && (true == sLexer.bDebug))
			{
				lexPrintToken (theToken);
			}
		}

		ctPrint (&sConstantTable);
		rtsPrint (&sRuntimeStack);
	}

	printf ("\n");
	erPrintAll (&sErrorHandler);
	erClear (&sErrorHandler);

	ctDispose (&sConstantTable);
	stDisposeSymbolTable (&sSymbolTable, ST_SIZE);
	erDisposeErrorHandler (&sErrorHandler);
	rtsDispose (&sRuntimeStack);

	if (NULL != pFile)
	{
		fclose (pFile);
	}

	return stError;
}
