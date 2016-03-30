/****************************************************************************
 File name:		bu_driver.c
 Author:			Kevin Jo
 Date:				3/13/15
 Class:				CS480
 Assignment: 	Buttom Up Parser
 Purpose:			Adds reserved words, reads from file to produce tokens, stores
 	 	 	 	 	 	  constants in the constant table and the address of that
 	 	 	 	 	 	  constant is kept in the runtime stack. Also parses expressions
 	 	 	 	 	 	  read from a file.
 Hours:				30
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "getopt.h"
#include "../include/st.h"
#include "../include/er.h"
#include "../include/pcc13defines.h"
#include "../include/lex.h"
#include "../include/ct.h"
#include "../include/rts.h"
#include "../include/bu.h"

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
	SymbolTable sSymbolTable;
	ErrorHandler sErrorHandler;
	ErrorCode buError;
	bool bEOF = false, bLexDebug = false;
	int character, option_index = 0;
	buParser sBuParser;
	LOAD_ERRORS

	while (1)
	{
		static struct option long_options [] =
		{
				{"lexdebug", no_argument, 0, 'l'},
				{0						 ,					 0, 0,  0}
		};

		character = getopt_long (argc, argv, "l:", long_options, &option_index);

		if (character == -1)
		{
			break;
		}

		switch (character)
		{
			case 'l':
				bLexDebug = true;
				break;
		}
	}

	argc -= optind;
	argv += optind;

	FILE *pFile = fopen (argv [0], "r");

	erCreateErrorHandler (&sErrorHandler);
	stCreateSymbolTable (&sSymbolTable, ST_SIZE, RSRVWRD_LEVEL);
	buCreate (&sBuParser, &sLexer, pFile, &sConstantTable, &sRuntimeStack,
						&sErrorHandler, &sSymbolTable);
	erIncrementLine (&sErrorHandler);

	if (true == bLexDebug)
	{
		lexDebug (sBuParser.psLexer);
		lexToggle (sBuParser.psLexer);

	}

	if (NULL == pFile)
	{
		buError = CANNOT_OPEN_FILE;
		erAddError (&sErrorHandler, buError, argv [1]);
		erPrintEntry (&sErrorHandler, buError, argv [1]);
	}

	stInsertName (&sSymbolTable, "main", RSRVWRD, RSRVWRD_MAIN,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "int", RSRVWRD, RSRVWRD_INT,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "if", RSRVWRD, RSRVWRD_IF,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "else", RSRVWRD, RSRVWRD_ELSE,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "return", RSRVWRD,
								RSRVWRD_RETURN, UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "for", RSRVWRD, RSRVWRD_FOR,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "input", RSRVWRD, RSRVWRD_INPUT,
								UNINITIALIZED, UNINITIALIZED);
	stInsertName (&sSymbolTable, "output", RSRVWRD,
								RSRVWRD_OUTPUT, UNINITIALIZED, UNINITIALIZED);

	//lexFillBuffer (sBuParser.psLexer);
	bEOF = lexCheckEOF (sBuParser.psLexer);

	while (false == bEOF)
	{
		buParse (&sBuParser);
		bEOF = lexCheckEOF (sBuParser.psLexer);
	}


	ctPrint (&sConstantTable);
	rtsPrint (&sRuntimeStack);
	erPrintAll (&sErrorHandler);
	erClear (&sErrorHandler);

	ctDispose (&sConstantTable);
	stDisposeSymbolTable (&sSymbolTable, ST_SIZE);
	erDisposeErrorHandler (&sErrorHandler);
	rtsDispose (&sRuntimeStack);
	buDispose (&sBuParser);

	if (NULL != pFile)
	{
		fclose (pFile);
	}

	return EXIT_SUCCESS;
}
