/****************************************************************************
 File name:		pcc.c
 Author:			Kevin Jo
 Date:				4/5/15
 Class:				CS480
 Assignment: 	Semantic Actions 3
 Purpose:			This is the driver for the pcc which will produce a quad
 	 	 	 	 	 	  file to run a program
 Hours:				150
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
#include "../include/bu.h"

#define MAX_FILE_SIZE 256

extern char gszErrors [ERROR_TBL_SIZE][MAX_ERROR_LINE];

const char * DRAGON_FILE = "dragon.txt";

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
	buParser sBUParser;
	SymbolTable sSymbolTable;
	//ErrorHandler sErrorHandler;
	ErrorCode tdError;
	Lexer sLexer;
	ConstantTable sConstantTable;
	RuntimeStack sRuntimeStack;
	QuadFile sQuadFile;
	int character, option_index = 0;
	char *fileName;
	char defaultFile [MAX_FILE_SIZE] = "quad.q";
	bool bLexDebug = false, bTdDebugLeave = false, bTdDebug = false,
			 bBuDebug = false, bSA1Dump = false, bSA2Dump = false, bSA3Dump = false,
			 bSA3TypeCheck = false;

	FILE *pDragon = fopen (DRAGON_FILE, "r");

	fileName = argv [1];

	FILE *pFile = fopen (fileName, "r");

	static struct option long_options [] =
	{
			{"tddebugleave", no_argument, 0, 't'},
			{"tddebugenter", no_argument, 0, 'e'},
			{"lexdebug", no_argument, 0, 'l'},
			{"budumphandle", no_argument, 0, 'b'},
			{"sa1symdump", no_argument, 0, '4'},
			{"sa2tabdump", no_argument, 0, '2'},
			{"sa3quaddump", no_argument, 0, '3'},
			{"sa3typecheck", no_argument, 0, 'c'},
			{"-o", optional_argument, 0, 'o'}
	};

	while (1)
	{
		character = getopt_long (argc, argv, "-telb423co",
								long_options, &option_index);

		if (character == -1)
		{
			break;
		}

		switch (character)
		{
			case 'c':
				bSA3TypeCheck = true;
				break;

			case '3':
				bSA3Dump = true;
				break;

			case '2':
				bSA2Dump = true;
				break;

			case '4':
				bSA1Dump = true;
				break;

			case 'b':
				bBuDebug = true;
				break;

			case 'l':
				bLexDebug = true;
				break;

			case 't':
				bTdDebugLeave = true;
				break;

			case 'e':
				bTdDebug = true;
				break;

			case '\1':
				break;

			case 'o':
				if ((getopt_long (argc, argv, "-telb423co", long_options, &option_index))
						== '\1')
				{
					strcpy (defaultFile, optarg);
				}
		}
	}

	FILE *pWriteFile = fopen (defaultFile, "w");

	buCreate (&sBUParser, &sLexer, pFile, &sConstantTable, &sRuntimeStack,
						&(sParser.sErrorHandler), &sSymbolTable);
	tdCreateParser (&sParser, pFile, &sLexer, &sSymbolTable, &sQuadFile,
									&sBUParser);

	if (true == bLexDebug)
	{
		lexDebug (sParser.psLexer);
	}

	tdLeaveDebug (&sParser, bTdDebugLeave);
	tdDebug (&sParser, bTdDebug);
	buDebug (&sBUParser, bBuDebug);
	tdTypeCheck (&sParser, bSA3TypeCheck);
	tdSymbolTable (&sParser, bSA1Dump);
	quadQuadDump (sParser.psQuadFile, bSA3Dump);

	if (NULL == pFile)
	{
		tdError = CANNOT_OPEN_FILE;
		erAddError (&sParser.sErrorHandler, tdError, argv [1]);
		erPrintEntry (&sParser.sErrorHandler, tdError, argv [1]);
	}

	stAddResWords (sParser.psSymbolTable);

	tdParse (&sParser);

	if (true == bSA2Dump)
	{
		printf ("\n");
		ctPrint (sParser.psLexer->psConstantTable);
		printf ("\n");
		rtsPrint (sParser.psLexer->psRTStack);
	}

	printf ("\n");

	if (sParser.sErrorHandler.psErrorList->numElements == 0)
	{
		printf ("0 Errors.\n\n");
		quadPrint (sParser.psQuadFile, (*sParser.psLexer->psRTStack), pWriteFile);
	}

	else
	{
		tdPrintDragon (pDragon);
		erPrintAll (&sParser.sErrorHandler);
	}

	ctDispose (&sConstantTable);
	erDisposeErrorHandler (&(sParser.sErrorHandler));
	rtsDispose (&sRuntimeStack);
	stDisposeSymbolTable (&sSymbolTable, ST_SIZE);
	buDispose (sParser.psBUParser);

	fclose (pWriteFile);
	fclose (pFile);

	return EXIT_SUCCESS;
}
