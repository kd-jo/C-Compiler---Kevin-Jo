/****************************************************************************
 File name:		td_driver.c
 Author:			Kevin Jo
 Date:				3/4/15
 Class:				CS480
 Assignment: 	Top Down Parser
 Purpose:			This program will parse a input file and output the function
 	 	 	 	 	 	 	names as they are entered.
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
#include "../include/td.h"

extern char gszErrors [ERROR_TBL_SIZE][MAX_ERROR_LINE];

/****************************************************************************
 Function:    main

 Description:	A driver to test the top down parser module.

 Parameters:	argc - number of command line arguments
 	 	 	 	 	 		argv - the command line argument

 Returned:    EXIT STATUS
****************************************************************************/
int main (int argc, char *argv[])
{
	LOAD_ERRORS

	tdParser sParser;
	SymbolTable sSymbolTable;
	ErrorHandler sErrorHandler;
	ErrorCode tdError;
	Lexer sLexer;
	ConstantTable sConstantTable;
	RuntimeStack sRuntimeStack;
	QuadFile sQuadFile;
	int character, option_index = 0;
	bool bLexDebug = false, bTdDebugLeave = false;

	while (1)
	{
		static struct option long_options [] =
		{
				{"lexdebug", no_argument, 0, 'l'},
				{"tddebugleave", no_argument, 0, 't'},
				{0						 ,					 0, 0,  0}
		};

		character = getopt_long (argc, argv, "lt:", long_options, &option_index);

		if (character == -1)
		{
			break;
		}

		switch (character)
		{
			case 'l':
				bLexDebug = true;
				break;

			case 't':
				bTdDebugLeave = true;
				break;
		}
	}

	argc -= optind;
	argv += optind;

	FILE *pFile = fopen (argv [0], "r");

	lexCreate (&sLexer, pFile, &sConstantTable, &sRuntimeStack, &sSymbolTable);
	tdCreateParser (&sParser, pFile, &sLexer, &sSymbolTable, &sQuadFile,
									NULL);
	tdDebug (&sParser, bLexDebug);
	tdLeaveDebug (&sParser, bTdDebugLeave);

	if (NULL == pFile)
	{
		tdError = CANNOT_OPEN_FILE;
		erAddError (&sErrorHandler, tdError, argv [1]);
		erPrintEntry (&sErrorHandler, tdError, argv [1]);
	}

	tdParse (&sParser);

	ctPrint (&sConstantTable);
	rtsPrint (&sRuntimeStack);
	erPrintAll (&(sParser.sErrorHandler));

	ctDispose (&sConstantTable);
	erDisposeErrorHandler (&(sParser.sErrorHandler));
	rtsDispose (&sRuntimeStack);

	return EXIT_SUCCESS;
}
