/****************************************************************************
File name:		td.c
Author:				Kevin Jo
Date:					3/4/2015
Class:				CS480
Assignment:		Top Down tdParser
Purpose:			This is the implementation file for each of the top down tdParser
							functions associated with the top down tdParser module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/td.h"

#define EMPTYSTRING " "
#define ST_SIZE 991
#define MAX_PARAMS 256

static void nextToken (tdParser *);
static bool tdMatch (tdParser *, tokenClassStates, int);
static ErrorCode program (tdParser *);
static ErrorCode mainprogram (tdParser *);
static ErrorCode externaldefs (tdParser *);
static ErrorCode externaldef (tdParser *, Entry *);
static ErrorCode typepart (tdParser *, Entry *);
static ErrorCode vardecl (tdParser *, Entry *, int *);
static ErrorCode optparamlist (tdParser *, int *, Entry *);
static ErrorCode moreparams (tdParser *, int *, Entry *);
static ErrorCode functionbody (tdParser *, Entry *, int *);
static ErrorCode typedecllist (tdParser *, Entry *, int *);
static ErrorCode idorptr (tdParser *, Entry *);
static ErrorCode optarraydecl (tdParser *, Entry *);
static ErrorCode moredecls (tdParser *, Entry *, int *);
static ErrorCode functionstmt (tdParser *, int *);
static ErrorCode optdecllist (tdParser *, Entry *, int *);
static ErrorCode optinit (tdParser *, Entry *);
static ErrorCode initializer (tdParser *, Entry *, int);
static ErrorCode initlist (tdParser *, Entry *);
static ErrorCode moreinit (tdParser *, Entry *, int);
static ErrorCode moreinitdecls (tdParser *, Entry *, int *);
static ErrorCode stmtlist (tdParser *);
static ErrorCode statement (tdParser *);
static ErrorCode optexp (tdParser *, bool *);
static ErrorCode optelse (tdParser *);
static ErrorCode arglist (tdParser *, int *);
static ErrorCode morearglist (tdParser *, int *);
static ErrorCode EXPRESSION (tdParser *);
static bool startexp (tdParser *);
static bool startStatement (tdParser *);
static ErrorCode panicmode (tdParser *);
static void tdAddEntry (tdParser *, Entry);
static void tdUpdateCurrent (tdParser *);

/****************************************************************************
Function:			tdCreateParser

Description:	Creates a tdParser.

Parameters:		psTDParser - the tdParser
							pFile		 	 - the input file
							psLex		 	 - the Lexer to be initialized

Returned:			None
*****************************************************************************/
void tdCreateParser (tdParser *psTDParser, FILE *pFile, Lexer *psLex,
										 SymbolTablePtr psSymbolTable, QuadFile *psQuadFile,
										 buParser *psBUParser)
{
	psTDParser->pFile = pFile;
	psTDParser->bDebugBegin = false;
	psTDParser->bDebugEnd = false;
	psTDParser->bSymbolTable = false;
	erCreateErrorHandler (&(psTDParser->sErrorHandler));
	stCreateSymbolTable (psSymbolTable, ST_SIZE, RSRVWRD_LEVEL);
	quadFileCreate (psQuadFile);
	psTDParser->psQuadFile = psQuadFile;
	psTDParser->psSymbolTable = psSymbolTable;
	psTDParser->psLexer = psLex;
	psTDParser->psBUParser = psBUParser;
	psTDParser->psBUParser->psQuadFile = psQuadFile;
	psTDParser->bForLoop = false;
	psTDParser->tempBufferIndex = 0;
}

/****************************************************************************
Function:			parse

Description:	Starts the parsing process

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR if no error.
*****************************************************************************/
ErrorCode tdParse (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;

	tdError = program (psTDParser);

	return tdError;
}

/****************************************************************************
Function:			nextToken

Description:	Retreves the next token

Parameters:		psTDParser - the tdParser

Returned:			None
*****************************************************************************/
void nextToken (tdParser *psTDParser)
{
	ErrorCode lexError;

	psTDParser->sPrevToken = psTDParser->sCurrentToken;
	psTDParser->tempBufferIndex = psTDParser->psLexer->buffIndex;

	if ('\n' == psTDParser->psLexer->buffer [psTDParser->tempBufferIndex] ||
			(psTDParser->psLexer->currentToken.class == SEMICOLON &&
			 psTDParser->bForLoop == false))
	{
		psTDParser->tempBufferIndex = 0;

	}

	psTDParser->sCurrentToken = lexGetToken (psTDParser->psLexer, &lexError,
														&(psTDParser->sErrorHandler));



	erAddError (&(psTDParser->sErrorHandler), lexError, EMPTYSTRING);
}

/****************************************************************************
Function:			tdMatch

Description:	tdMatches a given tokenClass and value with current token

Parameters:		psTDParser - the tdParser
							tokenClass - the token class
							value			- the token class value

Returned:			true if the current token tdMatches, false otherwise
*****************************************************************************/
bool tdMatch (tdParser *psTDParser, tokenClassStates tokenClass, int value)
{
	bool btdMatch = false;


	if (psTDParser->sCurrentToken.class == tokenClass)
	{
			if (RES_WORD == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.RWType == value)
				{
					btdMatch = true;
				}
			}

			else if (ADDRESSOP == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.addressofop == value)
				{
					btdMatch = true;
				}
			}

			else if (IDENT == tokenClass)
			{
					btdMatch = true;
			}

			else if (CONSTANT == tokenClass)
			{
				btdMatch = true;
			}

			else if (ASSIGNOP == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.assignop == value)
				{
					btdMatch = true;
				}
			}

			else if (COMMA == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.comma == value)
				{
					btdMatch = true;
				}
			}

			else if (LBRACKET == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.BRACKETType == value)
				{
					btdMatch = true;
				}
			}

			else if (RBRACKET == tokenClass)
			{
				if (psTDParser->sCurrentToken.value.BRACKETType == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == MULOP)
			{
				if (psTDParser->sCurrentToken.value.MULOPType == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == BRACE)
			{
				if (psTDParser->sCurrentToken.value.BRACEType == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == SEMICOLON)
			{
				if (psTDParser->sCurrentToken.value.semicolon == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == LPAREN)
			{
				if (psTDParser->sCurrentToken.value.PARENTHESISType == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == RPAREN)
			{
				if (psTDParser->sCurrentToken.value.PARENTHESISType == value)
				{
					btdMatch = true;
				}
			}

			else if (tokenClass == DEREF)
			{
				btdMatch = true;
			}
	}

	if (true == btdMatch)
	{
		psTDParser->tempBufferIndex = psTDParser->psLexer->buffIndex;
		nextToken (psTDParser);
	}

	return btdMatch;
}

/****************************************************************************
Function:			program

Description:	The starting function for the tdParser.

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode program (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	Quad sTempQuad;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("program\n");
	}

	quadInit (&sTempQuad);

	sTempQuad.level = GLOBAL_LEVEL;
	sTempQuad.opCode = OP_PROGBEGIN;

	quadInsert (psTDParser->psQuadFile, sTempQuad);

	nextToken (psTDParser);

	tdError = externaldefs (psTDParser);

	if (NO_ERROR == tdError)
	{
		tdError = mainprogram (psTDParser);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving program\n");
	}

	quadInit (&sTempQuad);
	sTempQuad.level = GLOBAL_LEVEL;
	sTempQuad.opCode = OP_PROGEND;
	quadInsert (psTDParser->psQuadFile, sTempQuad);

	return tdError;
}

/****************************************************************************
Function:			mainprogram

Description:	The main program portion of the C-Lite grammar.

Parameters:		psTDParser - the tdParser

Returned:			MISSING_RPAREN if a right parenthesis is missing.
							MISSING_LPAREN if a left parenthesis is missing.
							MAIN_EXPECTED if main is not existent
*****************************************************************************/
ErrorCode mainprogram (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bMain = false, bLParen = false, bRParen = false;
	Quad sTempQuad;
	HT_DATATYPE *psSTData;
	int sizeLocalData = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("mainprogram\n");
	}

	bMain = tdMatch (psTDParser, RES_WORD, RSRVWRD_MAIN);


	if (true == bMain)
	{
		stSearchEntry (psTDParser->psSymbolTable, "main", &psSTData);
		psSTData->stData.idAddressField = psTDParser->psQuadFile->quadLevel;
		quadInit (&sTempQuad);
		sTempQuad.level = LOCAL_LEVEL;

		bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);

		if (true == bLParen)
		{
			sTempQuad.opCode = OP_FUNCBEGIN;
			quadInsert (psTDParser->psQuadFile, sTempQuad);
			bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

			if (true == bRParen)
			{
				stIncrementLevel (psTDParser->psSymbolTable);
				tdError = functionstmt (psTDParser, &sizeLocalData);


				psTDParser->psQuadFile->localVarAddress = sTempQuad.localVarsIndex;

				//Function return for main ()
				quadInit (&sTempQuad);
				sTempQuad.level = LOCAL_LEVEL;
				sTempQuad.opCode = OP_FUNCRETURN;
				quadInsert (psTDParser->psQuadFile, sTempQuad);

				if (psTDParser->bSymbolTable == true)
				{
					printf ("Deleting a level - ST before leaving function %s:\n\n",
									"main");
					stPrintHeader ();
					stPrintAll (psTDParser->psSymbolTable);
				}

				stDeleteLevel (psTDParser->psSymbolTable);
				stDecrementLevel (psTDParser->psSymbolTable);

				//Call Main ()
				quadInit (&sTempQuad);
				sTempQuad.level = GLOBAL_LEVEL;
				sTempQuad.opCode = OP_FUNCCALL;
				sTempQuad.operands [2] = psSTData->stData.idAddressField;
				quadInsert (psTDParser->psQuadFile, sTempQuad);
			}

			else
			{
				tdError = MISSING_RPAREN;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			}
		}

		else
		{
			tdError = MISSING_LPAREN;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	else
	{
		tdError = MAIN_EXPECTED;
		erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
	}



	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving mainprogram\n");
	}

	return tdError;
}

/****************************************************************************
Function:			externaldefs

Description:	Checks external definitions., Entry *

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode externaldefs (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bInt = false;
	Entry sStEntry;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("externaldefs\n");
	}

	bInt = tdMatch (psTDParser, RES_WORD, RSRVWRD_INT);

	if (true == bInt)
	{
		tdError = externaldef (psTDParser, &sStEntry);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving externaldefs\n");
	}

	return tdError;
}

/****************************************************************************
Function:			externaldef

Description:	Checks identifiers for external definitions.

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode externaldef (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bId = false, bDeref = false, bSemiColon = false;
	int numLocalVars = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("externaldef\n");
	}

	bId = tdMatch (psTDParser, IDENT, 0);
	bDeref = tdMatch (psTDParser, DEREF, 0);

	if (true == bId)
	{
		memcpy (psEntry->identifier, psTDParser->sPrevToken.lexeme,
						sizeof (char) * MAX_IDENTIFIER);
		psEntry->sType = INTEGER;
		psEntry->level = GLOBAL_LEVEL;
		tdError = typepart (psTDParser, psEntry);

		if (psTDParser->psSymbolTable->currentLevel == LOCAL_LEVEL)
		{
			stDecrementLevel (psTDParser->psSymbolTable);
		}

		tdError = externaldefs (psTDParser);
	}

	else if (true == bDeref)
	{
		bId = false;
		bId = tdMatch (psTDParser, IDENT, 0);

		if (true == bId)
		{
			memcpy (psEntry->identifier, psTDParser->sPrevToken.lexeme,
							sizeof (char) * MAX_IDENTIFIER);
			psEntry->sType = POINTER;
			psEntry->level = GLOBAL_LEVEL;
			tdError = vardecl (psTDParser, psEntry, &numLocalVars);
			bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

			if (true == bSemiColon)
			{
				tdError = externaldefs (psTDParser);
			}

			else
			{
				tdError = MISSING_SEMICOLON;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			}
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving externaldef\n");
	}

	return tdError;
}

/****************************************************************************
Function:			typepart

Description:	Checks to see if a id is apart of a declaration

Parameters:		psTDParser - the tdParser

Returned:			MISSING _RPAREN if a right parenthesis is missing.
							MISSING_SEMICOLON if a semicolon is missing
							else, NO_ERROR
*****************************************************************************/
ErrorCode typepart (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bLParen = false, bRParen = false, bSemiColon = false;
	Quad sTempQuad;
	Entry paramEntry [MAX_PARAMS];
	int numParams = 0, dummyLocalVars = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("typepart\n");
	}

	bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);

	if (true == bLParen)
	{
		quadInit (&sTempQuad);
		sTempQuad.level = LOCAL_LEVEL;
		sTempQuad.opCode = OP_FUNCBEGIN;
		psEntry->addressField = psTDParser->psQuadFile->quadLevel;
		quadInsert (psTDParser->psQuadFile, sTempQuad);
		psEntry->sType = FUNCNAME;

		optparamlist (psTDParser, &numParams, paramEntry);
		psEntry->other = numParams;
		tdAddEntry (psTDParser, (*psEntry));
		stIncrementLevel (psTDParser->psSymbolTable);

		bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

		if (true == bRParen)
		{

			functionbody (psTDParser, paramEntry, &numParams);

			//Function return, Resetting the local address -Quad add
			quadInit (&sTempQuad);
			sTempQuad.level = LOCAL_LEVEL;
			sTempQuad.opCode = OP_FUNCRETURN;
			quadInsert (psTDParser->psQuadFile, sTempQuad);
			psTDParser->psQuadFile->localVarAddress = 0;
			psTDParser->psQuadFile->argIndex = 0;

			if (psTDParser->bSymbolTable == true)
			{
				printf ("Deleting a level - ST before leaving function %s:\n\n",
							 (psEntry->identifier));
				stPrintHeader ();
				stPrintAll (psTDParser->psSymbolTable);
			}

			stDeleteLevel (psTDParser->psSymbolTable);
		}

		else
		{
			tdError = MISSING_RPAREN;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	else
	{
		vardecl (psTDParser, psEntry, &dummyLocalVars);
		bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

		if (true == bSemiColon)
		{
			tdError = NO_ERROR;
		}

		else
		{
			tdError = MISSING_SEMICOLON;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving typepart\n");
	}

	return tdError;
}

/****************************************************************************
Function:			vardecl

Description:	Declares variables

Parameters:		psTDParser - the tdParser

Returned:
*****************************************************************************/
ErrorCode vardecl (tdParser *psTDParser, Entry *psEntry, int *pNumLocalVars)
{
	ErrorCode tdError = NO_ERROR;
	bool bLBracket = false, bConstant = false, bRBracket = false;
	Quad sTempQuad;
	int rtsAddress, constant = 0, i, arraySpace = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("vardecl\n");
	}

	bLBracket = tdMatch (psTDParser, LBRACKET, BEG_BRACKET);

	if (true == bLBracket)
	{
		if (psEntry->sType == INTEGER)
		{
			psEntry->sType = INTEGER_ARRAY;
		}

		else if (psEntry->sType == POINTER)
		{
			psEntry->sType = POINTER_ARRAY;
		}

		bConstant = tdMatch (psTDParser, CONSTANT, 0);

		if (true == bConstant)
		{
			psEntry->other = atoi (psTDParser->sPrevToken.lexeme);
			constant = psEntry->other;
			(*pNumLocalVars) += constant;

			bRBracket = tdMatch (psTDParser, RBRACKET, END_BRACKET);

			if (true == bRBracket)
			{
				if (psEntry->level == GLOBAL_LEVEL)
				{
					rtsInsert (psTDParser->psLexer->psRTStack, 0, &rtsAddress);
					(*psEntry).addressField = rtsAddress;

					arraySpace = psTDParser->psLexer->psRTStack->index + constant;

					for (i = psTDParser->psLexer->psRTStack->index;
							i < arraySpace; i++)
					{
						rtsInsert (psTDParser->psLexer->psRTStack, 0, &rtsAddress);
					}
				}

				else if (psEntry->level == LOCAL_LEVEL)
				{
					psEntry->addressField = psTDParser->psQuadFile->localVarAddress + 1;
					psTDParser->psQuadFile->localVarAddress += constant + 1;
				}

				tdError = optinit (psTDParser, psEntry);
				quadInit (&sTempQuad);

				//Assigning the first spot on array to the array name - Quad Add
				sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
				sTempQuad.opCode = OP_ASSIGN;

				if (sTempQuad.level == GLOBAL_LEVEL)
				{
					sTempQuad.addressMode [0] = GLOBAL_LVALUE;
					sTempQuad.addressMode [2] = GLOBAL_LVALUE;
				}

				else if (sTempQuad.level == LOCAL_LEVEL)
				{
					sTempQuad.addressMode [0] = LOCAL_LVALUE;
					sTempQuad.addressMode [2] = LOCAL_LVALUE;
				}

				sTempQuad.operands [0] = psEntry->addressField + 1;
				sTempQuad.operands [2] = psEntry->addressField;
				quadInsert (psTDParser->psQuadFile, sTempQuad);

				psEntry->other = constant;
				tdAddEntry (psTDParser, (*psEntry));

				if (NO_ERROR == tdError)
				{
					tdError = moreinitdecls (psTDParser, psEntry, pNumLocalVars);
				}
			}

			else
			{
				tdError = MISSING_RBRACKET;
			}
		}
	}

	else
	{
		if (psEntry->level == GLOBAL_LEVEL)
		{
			rtsInsert (psTDParser->psLexer->psRTStack, 0, &rtsAddress);
			(*psEntry).addressField = rtsAddress;
		}

		else if (psEntry->level == LOCAL_LEVEL)
		{
			psTDParser->psQuadFile->localVarAddress++;
			psEntry->addressField = psTDParser->psQuadFile->localVarAddress;
		}


		tdError = optinit (psTDParser, psEntry);
		psEntry->other = -1;
		tdAddEntry (psTDParser, (*psEntry));

		if (NO_ERROR == tdError)
		{
			tdError = moreinitdecls (psTDParser, psEntry, pNumLocalVars);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving vardecl\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optparamlist

Description:	Optional parameter list

Parameters:		psTDParser - the tdParser

Returned:			NO_ERRPR
*****************************************************************************/
ErrorCode optparamlist (tdParser *psTDParser, int *pNumParams, Entry *sEntryArray)
{
	ErrorCode tdError = NO_ERROR;
	bool bId = false;
	Quad sTempQuad;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optparamlist\n");
	}

	bId = tdMatch (psTDParser, IDENT, 0);

	if (true == bId)
	{
		quadGetCurrent (psTDParser->psQuadFile, &sTempQuad);
		memcpy (sEntryArray [*pNumParams].identifier,
						psTDParser->sPrevToken.lexeme, sizeof (char) * MAX_IDENTIFIER);
		stIncrementLevel (psTDParser->psSymbolTable);
		sEntryArray [*pNumParams].addressField = sTempQuad.argIndex - 1;
		sTempQuad.argIndex = sTempQuad.argIndex - 1;
		(*pNumParams)++;
		quadUpdateCurrent (psTDParser->psQuadFile, sTempQuad);
		tdError = moreparams (psTDParser, pNumParams, sEntryArray);
		stDecrementLevel (psTDParser->psSymbolTable);
	}

	else
	{
		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optparamlist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			moreparams

Description:	More parameters

Parameters:		psTDParser - the tdParser

Returned:			IDENTIFIER_EXPECTED if there is no id
							else NO_ERROR;
*****************************************************************************/
ErrorCode moreparams (tdParser *psTDParser, int *pNumParams, Entry *sEntryArray)
{
	ErrorCode tdError = NO_ERROR;
	bool bComma = false, bId = false;
	Quad sTempQuad;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("moreparams\n");
	}

	bComma = tdMatch (psTDParser, COMMA, 0);

	if (true == bComma)
	{
		bId = tdMatch (psTDParser, IDENT, 0);

		if (true == bId)
		{
			quadGetCurrent (psTDParser->psQuadFile, &sTempQuad);

			memcpy (sEntryArray [*pNumParams].identifier,
							psTDParser->sPrevToken.lexeme, sizeof (char) * MAX_IDENTIFIER);
			sEntryArray [*pNumParams].addressField = sTempQuad.argIndex - 1;
			sTempQuad.argIndex = sTempQuad.argIndex - 1;
			quadUpdateCurrent (psTDParser->psQuadFile, sTempQuad);
			(*pNumParams)++;
			tdError = moreparams (psTDParser, pNumParams, sEntryArray);
		}

		else
		{
			tdError = IDENTIFIER_EXPECTED;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving moreparams\n");
	}

	return tdError;
}

/****************************************************************************
Function:			functionbody

Description:	The body of a function

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode functionbody (tdParser *psTDParser, Entry *sEntryArray, int *pNumParams)
{
	ErrorCode tdError = NO_ERROR;
	int numTempParams = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("functionbody\n");
	}

	tdError = typedecllist (psTDParser, sEntryArray, &numTempParams);

	if (numTempParams != (*pNumParams))
	{
		tdError = UNDECLARED_IDENTIFIER;
		erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
	}

	tdError = functionstmt (psTDParser, pNumParams);

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving functionbody\n");
	}

	return tdError;
}

/****************************************************************************
Function:			typedecllist

Description:	Declares int types

Parameters:		psTDParser - the tdParser

Returned:			MISSING_SEMICOLON if there is no semicolon
							otherwise, NO_ERROR
*****************************************************************************/
ErrorCode typedecllist (tdParser *psTDParser, Entry *sEntryArray, int *pNumParams)
{
	ErrorCode tdError = NO_ERROR;
	bool bInt = false, bSemiColon = false;
	Entry sTempEntry;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("typedecllist\n");
	}

	bInt = tdMatch (psTDParser, RES_WORD, RSRVWRD_INT);

	if (true == bInt)
	{
		sTempEntry = sEntryArray [*pNumParams];
		tdError = idorptr (psTDParser, &sTempEntry);
		tdError = optarraydecl (psTDParser, &sTempEntry);
		sEntryArray [*pNumParams].sType = sTempEntry.sType;
		sEntryArray [*pNumParams].other = NONE;
		tdAddEntry (psTDParser, sEntryArray [*pNumParams]);
		(*pNumParams)++;

		if (NO_ERROR == tdError)
		{
			tdError = moredecls (psTDParser, sEntryArray, pNumParams);
			bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

			if (true == bSemiColon)
			{
				tdError = typedecllist (psTDParser, sEntryArray, pNumParams);
			}

			else
			{
				tdError = MISSING_SEMICOLON;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			}
		}

	}

	else
	{
		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving typedecllist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			functionstmt

Description:	the actual statement of the function

Parameters:		psTDParser - the tdParser

Returned:			MISSING_RBRACE - if there is no right brace
							MISSING_LBRACE - if there is no left brace
*****************************************************************************/
ErrorCode functionstmt (tdParser *psTDParser, int *pNumLocalVars)
{
	ErrorCode tdError = NO_ERROR;
	bool LBrace = false, RBrace = false;
	Entry sTempEntry;
	Quad sTempQuad;
	int numLocalVars = 0;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("functionstmt\n");
	}

	LBrace = tdMatch (psTDParser, BRACE, BEG_BRACE);

	if (true == LBrace)
	{

		tdError = optdecllist (psTDParser, &sTempEntry, &numLocalVars);
		(*pNumLocalVars) = numLocalVars;
		//BackPatching Number of local variables.
		quadPatchFuncBegin (psTDParser->psQuadFile, OP_FUNCBEGIN, numLocalVars);

		if (NO_ERROR == tdError)
		{
			tdError = stmtlist (psTDParser);

			if (NO_ERROR == tdError)
			{
				RBrace = tdMatch (psTDParser, BRACE, END_BRACE);

				if (true == RBrace)
				{
					//Return Value Set
					quadInit (&sTempQuad);
					sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
					sTempQuad.opCode = OP_ASSIGN;
					sTempQuad.addressMode [2] = GLOBAL_LVALUE;
					quadInsert (psTDParser->psQuadFile, sTempQuad);
					tdError = NO_ERROR;
				}

				else
				{
					tdError = MISSING_RBRACE;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				}
			}
		}
	}

	else
	{
		tdError = MISSING_LBRACE;
		erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving functionstmt\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optdecllist

Description:	Optional declaration list.

Parameters:		psTDParser - the tdParser

Returned:			MISSING_SEMICOLON if there is no semicolon
							otherwise, NO_ERROR
*****************************************************************************/
ErrorCode optdecllist (tdParser *psTDParser, Entry *psEntry, int *pNumLocalVars)
{
	ErrorCode tdError = NO_ERROR;
	bool bInt = false, bSemiColon = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optdecllist\n");
	}

	bInt = tdMatch (psTDParser, RES_WORD, RSRVWRD_INT);
	psEntry->level = LOCAL_LEVEL;

 	if (true == bInt)
	{
		tdError = idorptr (psTDParser, psEntry);

		if (NO_ERROR == tdError)
		{
			(*pNumLocalVars)++;

			tdError = vardecl (psTDParser, psEntry, pNumLocalVars);

			if (NO_ERROR == tdError)
			{
				bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

				if (true == bSemiColon)
				{
					tdError = optdecllist (psTDParser, psEntry, pNumLocalVars);
				}

				else
				{
					tdError = MISSING_SEMICOLON;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				}
			}
		}
	}

	else
	{
		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optdecllist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optinit

Description:	Optional initializer.

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode optinit (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bAssign = false;
	int tempBufferIndex;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optinit\n");
	}

	//Save the buffer index of the beginning of the EXP
	tempBufferIndex = psTDParser->psLexer->buffIndex;
	bAssign = tdMatch (psTDParser, ASSIGNOP, 0);

	if (true == bAssign)
	{
		tdError = initializer (psTDParser, psEntry, tempBufferIndex);
	}

	else
	{
		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optinit\n");
	}

	return tdError;
}
/****************************************************************************
Function:			initializer

Description:	The initalizer function that initializes variables.

Parameters:		psTDParser - the tdParser

Returned:			INVALID_DECLARATION if an expression is missing.
							otherwise, false
*****************************************************************************/
ErrorCode initializer (tdParser *psTDParser, Entry *psEntry,
											 int tempBuffIndex)
{
	ErrorCode tdError = NO_ERROR;
	bool bExp = false, bLBrace = false, bRBrace = false;
	Token sTempToken;
	Quad sTempQuad;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("initializer\n");
	}

	bExp = startexp (psTDParser);
	bLBrace = tdMatch (psTDParser, BRACE, BEG_BRACE);

	if (true == bLBrace)
	{
		tdError = initlist (psTDParser, psEntry);
		bRBrace = tdMatch (psTDParser, BRACE, END_BRACE);

		if (false == bRBrace)
		{
			tdError = MISSING_RBRACE;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	else if (true == bExp)
	{
		buSetInitExp (psTDParser->psBUParser);
		lexSetBuffIndex (psTDParser->psLexer, tempBuffIndex + 1);
		EXPRESSION (psTDParser);
		quadInit (&sTempQuad);

		if (psEntry->level == GLOBAL_LEVEL)
		{
			sTempQuad.addressMode [2] = GLOBAL_LVALUE;
		}

		else if (psEntry->level == LOCAL_LEVEL)
		{
			sTempQuad.addressMode [2] = LOCAL_LVALUE;
		}

		sTempQuad.operands [2] = psEntry->addressField;

		sTempToken = psTDParser->psBUParser->sTopStack;

		if (sTempToken.level == GLOBAL_LEVEL)
		{
			sTempQuad.addressMode [0] = GLOBAL_RVALUE;
		}

		else if (sTempToken.level == LOCAL_LEVEL)
		{
			sTempQuad.addressMode [0] = LOCAL_RVALUE;
		}

		sTempQuad.operands [0] = sTempToken.field;
		sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
		sTempQuad.opCode = OP_ASSIGN;
		quadInsert (psTDParser->psQuadFile, sTempQuad);

		buSetInitExp (psTDParser->psBUParser);
	}

	else
	{
		tdError = INVALID_DECLARATION;
		erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		panicmode (psTDParser);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving initializer\n");
	}

	return tdError;
}

/***************************************************************************
Function:			initlist

Description:	Checks for a list for initialization

Parameters:		psTDParser - the tdParser

Returned:			bBeugBeing - the debug output
***************************************************************************/
ErrorCode initlist (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bExp = false;
	Token sTempToken;
	Quad sTempQuad;
	int arrayIndex = 1;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("initlist\n");
	}

	bExp = startexp (psTDParser);

	if (true == bExp)
	{
		tdError = EXPRESSION (psTDParser);

		quadInit (&sTempQuad);

		if (psEntry->level == GLOBAL_LEVEL)
		{
			sTempQuad.addressMode [2] = GLOBAL_LVALUE;
		}

		else if (psEntry->level == LOCAL_LEVEL)
		{
			sTempQuad.addressMode [2] = LOCAL_LVALUE;
		}

		sTempQuad.operands [2] = psEntry->addressField + 1;

		sTempToken = psTDParser->psBUParser->sTopStack;

		if (sTempToken.level == GLOBAL_LEVEL)
		{
			sTempQuad.addressMode [0] = GLOBAL_RVALUE;
		}

		else if (sTempToken.level == LOCAL_LEVEL)
		{
			sTempQuad.addressMode [0] = LOCAL_RVALUE;
		}

		sTempQuad.operands [0] = sTempToken.field;
		sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
		sTempQuad.opCode = OP_ASSIGN;
		quadInsert (psTDParser->psQuadFile, sTempQuad);

		tdError = moreinit (psTDParser, psEntry, arrayIndex + 1);
	}

	else
	{
		tdError = INVALID_DECLARATION;
		erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		panicmode (psTDParser);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving initlist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			moreinit

Description:	MOre initial declarations

Parameters:		psTDParser - the tdParser

Returned:			INVALID_DECLARATIONS - if no expression is found.
*****************************************************************************/
ErrorCode moreinit (tdParser *psTDParser, Entry *psEntry, int arrayIndex)
{
	ErrorCode tdError = NO_ERROR;
	bool bComma = false, bExp = false;
	Token sTempToken;
	Quad sTempQuad;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("moreinit\n");
	}

	bComma = tdMatch (psTDParser, COMMA, 0);

	if (true == bComma)
	{
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);
		bExp = startexp (psTDParser);

		if (true == bExp)
		{
			tdError = EXPRESSION (psTDParser);

			quadInit (&sTempQuad);

			if (psEntry->level == GLOBAL_LEVEL)
			{
				sTempQuad.addressMode [2] = GLOBAL_LVALUE;
			}

			else if (psEntry->level == LOCAL_LEVEL)
			{
				sTempQuad.addressMode [2] = LOCAL_LVALUE;
			}

			sTempQuad.operands [2] = psEntry->addressField + arrayIndex;

			sTempToken = psTDParser->psBUParser->sTopStack;

			if (sTempToken.level == GLOBAL_LEVEL)
			{
				sTempQuad.addressMode [0] = GLOBAL_RVALUE;
			}

			else if (sTempToken.level == LOCAL_LEVEL)
			{
				sTempQuad.addressMode [0] = LOCAL_RVALUE;
			}

			sTempQuad.operands [0] = sTempToken.field;
			sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
			sTempQuad.opCode = OP_ASSIGN;
			quadInsert (psTDParser->psQuadFile, sTempQuad);

			tdError = moreinit (psTDParser, psEntry, arrayIndex + 1);
		}

		else
		{
			tdError = INVALID_DECLARATION;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			panicmode (psTDParser);
		}
	}

	else
	{
		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving moreinit\n");
	}

	return tdError;
}

/****************************************************************************
Function:			moreinitdecls

Description:	More initial declarations

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode moreinitdecls (tdParser *psTDParser, Entry *psEntry, int *pNumLocalVars)
{
	ErrorCode tdError = NO_ERROR;
	bool bComma = false;
	Entry sTempEntry;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("moreinitdecls\n");
	}

	bComma = tdMatch (psTDParser, COMMA, 0);

	if (true == bComma)
	{
		tdError = idorptr (psTDParser, &sTempEntry);
		sTempEntry.level = psEntry->level;


		if (NO_ERROR == tdError)
		{
			(*pNumLocalVars)++;
			tdError = vardecl (psTDParser, &sTempEntry, pNumLocalVars);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving moreinitdecls\n");
	}

	return tdError;
}

/****************************************************************************
Function:			stmtlist

Description:	Starts the statement function

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode stmtlist (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bStartStatement = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("stmtlist\n");
	}

	 bStartStatement = startStatement (psTDParser);

	if (true == bStartStatement)
	{
		tdError = statement (psTDParser);
		tdError = stmtlist (psTDParser);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving stmtlist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			statement

Description:	Checks to see if the string is a statement

Parameters:		psTDParser - the tdParser

Returned:			MISSING_RPAREN - missing a right parenthesis
							INVALID_DECLARATION - missing a expression
							MISSING_LPAREN - missing a left parenthesis
							MISSING_SEMICOLON - missing a semicolon
*****************************************************************************/
ErrorCode statement (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bIf = false, bFor = false, bReturn = false, bSemiColon = false,
			 bLBrace = false, bInput = false, bOutput = false, bExp = false,
			 bLParen = false, bRParen = false, bRBrace = false, bIsReturn = false;
	Quad sTempQuad;
	Token sTempToken;
	int numArgs = 0, quadLevelBackPatch, quadConditional, quadBnot, quadJump;

	quadInit (&sTempQuad);

	if (true == psTDParser->bDebugBegin)
	{
		printf ("statement\n");
	}

	if ((psTDParser->sCurrentToken.class == RES_WORD &&
			psTDParser->sCurrentToken.value.RWType == RSRVWRD_IF))
	{
		bIf = tdMatch (psTDParser, RES_WORD, RSRVWRD_IF);
	}

	else if ((psTDParser->sCurrentToken.class == RES_WORD &&
			psTDParser->sCurrentToken.value.RWType == RSRVWRD_FOR))
	{
		bFor = tdMatch (psTDParser, RES_WORD, RSRVWRD_FOR);
	}

	else if ((psTDParser->sCurrentToken.class == RES_WORD &&
			psTDParser->sCurrentToken.value.RWType == RSRVWRD_RETURN))
	{
		bReturn = tdMatch (psTDParser, RES_WORD, RSRVWRD_RETURN);
	}

	else if ((psTDParser->sCurrentToken.class == BRACE &&
			psTDParser->sCurrentToken.value.BRACEType == BEG_BRACE))
	{
		bLBrace = tdMatch (psTDParser, BRACE, BEG_BRACE);
	}

	else if ((psTDParser->sCurrentToken.class == RES_WORD &&
			psTDParser->sCurrentToken.value.RWType == RSRVWRD_INPUT))
	{
		bInput = tdMatch (psTDParser, RES_WORD, RSRVWRD_INPUT);
	}

	else if ((psTDParser->sCurrentToken.class == RES_WORD &&
			psTDParser->sCurrentToken.value.RWType == RSRVWRD_OUTPUT))
	{
		bOutput = tdMatch (psTDParser, RES_WORD, RSRVWRD_OUTPUT);
	}

	else if (psTDParser->sCurrentToken.class == SEMICOLON)
	{
		bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);
	}

	else
	{
		bExp = startexp (psTDParser);
	}

	if (true == bIf)
	{
		bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);

		if (true == bLParen)
		{
			bExp = startexp (psTDParser);

			if (true == bExp)
			{
				lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);
				tdError = EXPRESSION (psTDParser);
				bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

				if (true == bRParen)
				{
					sTempToken = psTDParser->psBUParser->sTopStack;
					quadInit (&sTempQuad);
					sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
					sTempQuad.opCode = OP_BNOT;
					sTempQuad.operands [0] = sTempToken.field;
					quadLevelBackPatch = psTDParser->psQuadFile->quadLevel;

					if (sTempToken.level == GLOBAL_LEVEL)
					{
						sTempQuad.addressMode [0] = GLOBAL_RVALUE;
					}

					else
					{
						sTempQuad.addressMode [0] = LOCAL_RVALUE;
					}

					quadInsert (psTDParser->psQuadFile, sTempQuad);

					tdError = statement (psTDParser);

					quadInit (&sTempQuad);
					sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
					sTempQuad.opCode = OP_BRA;
					sTempQuad.operands [2] = psTDParser->psQuadFile->quadLevel + 1;
					quadInsert (psTDParser->psQuadFile, sTempQuad);

					quadPatchJumps (psTDParser->psQuadFile, quadLevelBackPatch,
													psTDParser->psQuadFile->quadLevel);

					quadLevelBackPatch = psTDParser->psQuadFile->quadLevel - 1;

					tdError = optelse (psTDParser);

					quadPatchJumps (psTDParser->psQuadFile, quadLevelBackPatch,
													psTDParser->psQuadFile->quadLevel);

				}

				else
				{
					tdError = MISSING_RPAREN;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
					panicmode (psTDParser);
				}
			}

			else
			{
				tdError = INVALID_DECLARATION;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				panicmode (psTDParser);
			}
		}

		else
		{
			tdError = MISSING_LPAREN;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			panicmode (psTDParser);
		}
	}

	else if (true == bFor)
	{
		psTDParser->bForLoop = true;
		bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);

		if (true == bLParen)
		{
			bExp = startexp (psTDParser);

			if (true == bExp)
			{
				buSetExp (psTDParser->psBUParser);
				tdError = EXPRESSION (psTDParser);
				bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);
				lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);

				if (true == bSemiColon)
				{
					bExp = startexp (psTDParser);

					if (true == bExp)
					{
						//Save the quad level of conditional exp.
						quadConditional = psTDParser->psQuadFile->quadLevel;

						tdError = EXPRESSION (psTDParser);
						bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);
						buSetExp (psTDParser->psBUParser);

						quadBnot = psTDParser->psQuadFile->quadLevel;
						sTempToken = psTDParser->psBUParser->sTopStack;
						quadInit (&sTempQuad);
						sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
						sTempQuad.opCode = OP_BNOT;
						sTempQuad.operands [0] = sTempToken.field;


						if (sTempToken.level == GLOBAL_LEVEL)
						{
							sTempQuad.addressMode [0] = GLOBAL_RVALUE;
						}

						else
						{
							sTempQuad.addressMode [0] = LOCAL_RVALUE;
						}

						quadInsert (psTDParser->psQuadFile, sTempQuad);


						if (true == bSemiColon)
						{
							bExp = startexp (psTDParser);

							if (true == bExp)
							{
								quadJump = psTDParser->psQuadFile->quadLevel;

								quadInit (&sTempQuad);
								sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
								sTempQuad.opCode = OP_BRA;
								quadInsert (psTDParser->psQuadFile, sTempQuad);

								//Third exp
								tdError = EXPRESSION (psTDParser);
								quadInit (&sTempQuad);
								sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
								sTempQuad.opCode = OP_BRA;
								sTempQuad.operands [2] = quadConditional;
								quadInsert (psTDParser->psQuadFile, sTempQuad);

								quadPatchJumps (psTDParser->psQuadFile, quadJump,
																psTDParser->psQuadFile->quadLevel);

								bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

								if (true == bRParen)
								{
									tdError = statement (psTDParser);

									quadInit (&sTempQuad);
									sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
									sTempQuad.opCode = OP_BRA;
									sTempQuad.operands [2] = quadJump + 1;
									quadInsert (psTDParser->psQuadFile, sTempQuad);

									//Backpatch bnot
									quadPatchJumps (psTDParser->psQuadFile, quadBnot,
																	psTDParser->psQuadFile->quadLevel);

									psTDParser->bForLoop = false;

								}

								else
								{
									tdError = MISSING_RPAREN;
									erAddError (&(psTDParser->sErrorHandler), tdError,
															EMPTYSTRING);
									panicmode (psTDParser);
								}
							}

							else
							{
								tdError = INVALID_DECLARATION;
								erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
								panicmode (psTDParser);
							}
						}

						else
						{
							tdError = MISSING_SEMICOLON;
							erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
							panicmode (psTDParser);
						}
					}

					else
					{
						tdError = INVALID_DECLARATION;
						erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
						panicmode (psTDParser);
					}
				}

				else
				{
					tdError = MISSING_SEMICOLON;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
					panicmode (psTDParser);
				}
			}

			else
			{
				tdError = INVALID_DECLARATION;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				panicmode (psTDParser);
			}
		}

		else
		{
			tdError = MISSING_LPAREN;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			panicmode (psTDParser);
		}
	}

	else if (true == bExp)
	{
		buSetExp (psTDParser->psBUParser);
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex - 1);
		tdError = EXPRESSION (psTDParser);

		if (NO_ERROR == tdError)
		{
			bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

			if (true == bSemiColon)
			{
				tdError = NO_ERROR;
			}

			else
			{
				tdError = MISSING_SEMICOLON;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				panicmode (psTDParser);
			}
		}

		buSetExp (psTDParser->psBUParser);
	}

	else if (true == bLBrace)
	{
		tdError = stmtlist (psTDParser);
		bRBrace = tdMatch (psTDParser, BRACE, END_BRACE);

		if (true == bRBrace)
		{
			tdError = NO_ERROR;
		}

		else
		{
			tdError = MISSING_RBRACE;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			panicmode (psTDParser);
		}
	}

	else if (true == bSemiColon)
	{
		tdError = NO_ERROR;
	}

	else if (true == bInput)
	{
		bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);

		if (true == bLParen)
		{
			tdError = arglist (psTDParser, &numArgs);
			bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

			if (true == bRParen)
			{
				quadInit (&sTempQuad);
				sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
				sTempQuad.opCode = OP_INPUT;
				sTempQuad.operands [0] = numArgs;
				quadInsert (psTDParser->psQuadFile, sTempQuad);
				bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

				if (true == bSemiColon)
				{
					tdError = NO_ERROR;
				}

				else
				{
					tdError = MISSING_SEMICOLON;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
					panicmode (psTDParser);
				}
			}

			else
			{
				tdError = MISSING_RPAREN;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				panicmode (psTDParser);
			}
		}
	}

	else if (true == bOutput)
	{
		bLParen = tdMatch (psTDParser, LPAREN, BEG_PARENTHESIS);
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);
		psTDParser->psBUParser->bOutput = true;

		if (true == bLParen)
		{
			tdError = arglist (psTDParser, &numArgs);
			bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);

			if (true == bRParen)
			{
				quadInit (&sTempQuad);
				sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
				sTempQuad.opCode = OP_OUTPUT;
				sTempQuad.operands [0] = numArgs;
				quadInsert (psTDParser->psQuadFile, sTempQuad);

				bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);
				psTDParser->psBUParser->bOutput = false;

				if (true == bSemiColon)
				{
					tdError = NO_ERROR;
				}

				else
				{
					tdError = MISSING_SEMICOLON;
					erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
					panicmode (psTDParser);
				}
			}

			else
			{
				tdError = MISSING_RPAREN;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
				panicmode (psTDParser);
			}
		}
	}

	else if (true == bReturn)
	{
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);
		buSetExp (psTDParser->psBUParser);
		tdError = optexp (psTDParser, &bIsReturn);

		if (bIsReturn == true)
		{
			sTempToken = psTDParser->psBUParser->sTopStack;
			quadInit (&sTempQuad);
			sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
			sTempQuad.opCode = OP_ASSIGN;
			sTempQuad.operands [0] = sTempToken.field;

			if (sTempToken.level == GLOBAL_LEVEL)
			{
				sTempQuad.addressMode [0] = GLOBAL_RVALUE;
			}

			else
			{
				sTempQuad.addressMode [0] = LOCAL_RVALUE;
			}

			sTempQuad.addressMode [2] = GLOBAL_LVALUE;
			sTempQuad.operands [2]	= 0;

			quadInsert (psTDParser->psQuadFile, sTempQuad);

			quadInit (&sTempQuad);

			sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
			sTempQuad.opCode = OP_FUNCRETURN;
			quadInsert (psTDParser->psQuadFile, sTempQuad);
		}

		bSemiColon = tdMatch (psTDParser, SEMICOLON, 0);

		if (true == bSemiColon)
		{
			tdError = NO_ERROR;
		}

		buSetExp (psTDParser->psBUParser);
	}


	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving statement\n");
	}

	return tdError;
}

/****************************************************************************
Function:			idorptr

Description:	Checks for a id or pointer

Parameters:		psTDParser - the tdParser

Returned:			IDENTIFIER_EXPECTED if a identifier is missing, else
							NO_ERROR
*****************************************************************************/
ErrorCode idorptr (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bId = false, bStar = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("idorptr\n");
	}

	bId = tdMatch (psTDParser, IDENT, 0);

	if (true == bId)
	{
			psEntry->sType = INTEGER;
			memcpy (psEntry->identifier, psTDParser->sPrevToken.lexeme,
							sizeof (char) * MAX_IDENTIFIER);
			tdError = NO_ERROR;
	}

	else if (false == bId)
	{
		bStar = tdMatch (psTDParser, DEREF, 0);

		if (true == bStar)
		{
			bId = tdMatch (psTDParser, IDENT, 0);

			if (true == bId)
			{
				psEntry->sType = POINTER;
				memcpy (psEntry->identifier, psTDParser->sPrevToken.lexeme,
								sizeof (char) * MAX_IDENTIFIER);
				tdError = NO_ERROR;
			}

			else
			{
				tdError = IDENTIFIER_EXPECTED;
				erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
			}
		}

		else
		{
			tdError = IDENTIFIER_EXPECTED;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving idorptr\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optexp

Description:	Optional expressions if needed.

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode optexp (tdParser *psTDParser, bool *pbReturn)
{
	ErrorCode tdError = NO_ERROR;
	bool bExp = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optexp\n");
	}

	bExp = startexp (psTDParser);

	if (true == bExp)
	{
		(*pbReturn) = true;
		tdError = EXPRESSION (psTDParser);
	}

	else
	{
		(*pbReturn) = false;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optexp\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optelse

Description:	Checks for a else statement

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode optelse (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bElse = false, bStatement = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optelse\n");
	}

	bElse = tdMatch (psTDParser, RES_WORD, RSRVWRD_ELSE);

	if (true == bElse)
	{
		bStatement = startStatement (psTDParser);

		if (true == bStatement)
		{
			tdError = statement (psTDParser);
		}
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optelse\n");
	}

	return tdError;
}

/****************************************************************************
Function:			arglist

Description:	Checks for an argument list

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode arglist (tdParser *psTDParser, int *pNumArgs)
{
	ErrorCode tdError = NO_ERROR;
	Quad sTempQuad;
	Token sTempToken;
	bool bExp = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("arglist\n");
	}

	bExp = startexp (psTDParser);

	if (true == bExp)
	{
		psTDParser->psBUParser->bArguments = true;
		(*pNumArgs)++;
		tdError = EXPRESSION (psTDParser);

		sTempToken = psTDParser->psBUParser->sTopStack;

		tdError = morearglist (psTDParser, pNumArgs);

		quadInit (&sTempQuad);
		sTempQuad.level = psTDParser->psSymbolTable->currentLevel;
		sTempQuad.opCode = OP_PUSHPARAM;
		sTempQuad.operands [0] = sTempToken.field;

		if (GLOBAL_LEVEL == sTempToken.level)
		{
			if (sTempToken.brValue == true)
			{
				sTempQuad.addressMode [0] = GLOBAL_RVALUE;
			}

			else
			{
				sTempQuad.addressMode [0] = GLOBAL_LVALUE;
			}
		}

		else if (LOCAL_LEVEL == sTempToken.level)
		{
			if (sTempToken.brValue == true)
			{
				sTempQuad.addressMode [0] = LOCAL_RVALUE;
			}

			else
			{
				sTempQuad.addressMode [0] = LOCAL_LVALUE;
			}
		}

		quadInsert (psTDParser->psQuadFile, sTempQuad);

		if (psTDParser->psBUParser->sTempQuad.quadLevel > 0 &&
				sTempToken.bAutoop == true)
		{
			quadInit (&sTempQuad);
			quadGetCurrent (&psTDParser->psBUParser->sTempQuad, &sTempQuad);
			quadInsert (psTDParser->psBUParser->psQuadFile, sTempQuad);

			psTDParser->psBUParser->sTempQuad.quadLevel--;
		}

		psTDParser->psBUParser->bArguments = false;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving arglist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			morearglist

Description:	Checks for more arguments in the list

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode morearglist (tdParser *psTDParser, int *pNumArgs)
{
	ErrorCode tdError = NO_ERROR;
	bool bComma = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("morearglist\n");
	}

	bComma = tdMatch (psTDParser, COMMA, 0);

	if (true == bComma)
	{
		lexSetBuffIndex (psTDParser->psLexer, psTDParser->tempBufferIndex + 1);
		tdError = arglist (psTDParser, pNumArgs);
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving morearglist\n");
	}

	return tdError;
}

/****************************************************************************
Function:			optarraydecl

Description:	Checks for an optional array declaration

Parameters:		psTDParser - the tdParser

Returned:			MISSING_RBRACKET if a right bracket is missing
*****************************************************************************/
ErrorCode optarraydecl (tdParser *psTDParser, Entry *psEntry)
{
	ErrorCode tdError = NO_ERROR;
	bool bLBracket = false, bRBracket = false;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("optarraydecl\n");
	}

	bLBracket = tdMatch (psTDParser, LBRACKET, BEG_BRACKET);

	if (true == bLBracket)
	{
		bRBracket = tdMatch (psTDParser, RBRACKET, END_BRACKET);

		if (true == bRBracket)
		{
			if (psEntry->sType == INTEGER)
			{
				psEntry->sType = INTEGER_ARRAY;
			}

			else if (psEntry->sType == POINTER)
			{
				psEntry->sType = POINTER_ARRAY;
			}

			tdError = NO_ERROR;
		}

		else
		{
			tdError = MISSING_RBRACKET;
			erAddError (&(psTDParser->sErrorHandler), tdError, EMPTYSTRING);
		}
	}

	else
	{

		tdError = NO_ERROR;
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving optarraydecl\n");
	}

	return tdError;
}

/****************************************************************************
Function:			moredecls

Description:	Checks for more declarations

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode moredecls (tdParser *psTDParser, Entry *sEntryArray, int *pNumParam)
{
	ErrorCode tdError = NO_ERROR;
	bool bComma = false;
	Entry sTempEntry;

	if (true == psTDParser->bDebugBegin)
	{
		printf ("moredecls\n");
	}

	bComma = tdMatch (psTDParser, COMMA, 0);

	if (true == bComma)
	{
		sTempEntry = sEntryArray [*pNumParam];
		tdError = idorptr (psTDParser, &sTempEntry);
		tdError = optarraydecl (psTDParser, &sTempEntry);
		sEntryArray [*pNumParam].sType = sTempEntry.sType;
		sEntryArray [*pNumParam].other = NONE;
		tdAddEntry (psTDParser, sEntryArray [*pNumParam]);
		(*pNumParam)++;
		tdError = moredecls (psTDParser, sEntryArray, pNumParam);

		/*
		if (NO_ERROR == tdError)
		{
			tdError = optarraydecl (psTDParser, sEntryArray);
		}
		*/
	}

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving moredecls\n");
	}

	return tdError;
}

/****************************************************************************
Function:			EXPRESSION

Description:	Checks for an expression.

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode EXPRESSION (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;


	if (true == psTDParser->bDebugBegin)
	{
		printf ("EXPRESSION\n");
	}

	lexDecrementBuffIndex (psTDParser->psLexer);
	buParse (psTDParser->psBUParser);
	tdUpdateCurrent (psTDParser);

	if (true == psTDParser->bDebugEnd)
	{
		printf ("leaving EXPRESSION\n");
	}

	return tdError;
}

/****************************************************************************
Function:			startexp

Description:	Checks to see if a start of an expression is at the current
							token

Parameters:		psTDParser - the tdParser

Returned:			true if the token is the start of an expression, else, false.
*****************************************************************************/
bool startexp (tdParser *psTDParser)
{
	bool bStartExp = false;

	if (psTDParser->sCurrentToken.class == IDENT ||
			psTDParser->sCurrentToken.class == CONSTANT ||
			psTDParser->sCurrentToken.class == DEREF ||
			(psTDParser->sCurrentToken.class == ADDOP &&
			psTDParser->sCurrentToken.value.ADDOPType == SUBTRACT_OP) ||
			psTDParser->sCurrentToken.class == ADDRESSOP ||
			psTDParser->sCurrentToken.class == LPAREN ||
			(psTDParser->sCurrentToken.class == NEGOP) ||
			(psTDParser->sCurrentToken.class == DEREF) ||
			(psTDParser->sCurrentToken.class == ADDRESSOP))
	{
		bStartExp = true;
	}
	return bStartExp;
}

/****************************************************************************
Function:			startStatement

Description:	Checks to see if a start of a statement is at the current
							token

Parameters:		psTDParser - the tdParser

Returned:			true if the token is the start of a statement, else, false.
*****************************************************************************/
bool startStatement (tdParser *psTDParser)
{
	bool bExp = false, bStartStatement = false;

	bExp = startexp (psTDParser);

	if ((psTDParser->sCurrentToken.class == RES_WORD &&
			 psTDParser->sCurrentToken.value.RWType == RSRVWRD_IF) ||
			(psTDParser->sCurrentToken.class == RES_WORD &&
			 psTDParser->sCurrentToken.value.RWType == RSRVWRD_FOR) ||
			(psTDParser->sCurrentToken.class == RES_WORD &&
			 psTDParser->sCurrentToken.value.RWType == RSRVWRD_RETURN) ||
			(psTDParser->sCurrentToken.class == RES_WORD &&
			 psTDParser->sCurrentToken.value.RWType == RSRVWRD_INPUT) ||
			(psTDParser->sCurrentToken.class == RES_WORD &&
			 psTDParser->sCurrentToken.value.RWType == RSRVWRD_OUTPUT) ||
			(psTDParser->sCurrentToken.class == BRACE &&
			 psTDParser->sCurrentToken.value.BRACEType == BEG_BRACE) ||
			(psTDParser->sCurrentToken.class == SEMICOLON) ||
			true == bExp)
	{
		bStartStatement = true;
	}

	return bStartStatement;
}

/****************************************************************************
Function:			panicmode

Description:	This function is used to recover from an error, and still
							continue parsing

Parameters:		psTDParser - the tdParser

Returned:			NO_ERROR
*****************************************************************************/
ErrorCode panicmode (tdParser *psTDParser)
{
	ErrorCode tdError = NO_ERROR;
	bool bSemicolon = false, bRParen = false, bRBrace = false;

	while (false == bSemicolon && false == bRParen && false == bRBrace)
	{
		nextToken (psTDParser);
		bSemicolon = tdMatch (psTDParser, SEMICOLON, 0);
		bRParen = tdMatch (psTDParser, RPAREN, END_PARENTHESIS);
		bRBrace = tdMatch (psTDParser, BRACE, END_BRACE);
	}

	return tdError;
}

/****************************************************************************
Function:			tdDebug

Description:	Turns on beginning debug options

Parameters:		psTDParser - the tdParser
							option	 - the option to turn debug options on or off

Returned:			None
*****************************************************************************/
void tdDebug (tdParser *psTDParser, bool option)
{
	if (true == option)
	{
		psTDParser->bDebugBegin = true;
	}

	else
	{
		psTDParser->bDebugBegin = false;
	}
}

/****************************************************************************
Function:			tdLeavingDebug

Description:	Turns on leaving debug options

Parameters:		psTDParser - the tdParser
							option	 - the option to turn debug options on or off

Returned:			None
*****************************************************************************/
void tdLeaveDebug (tdParser *psTDParser, bool option)
{
	if (true == option)
	{
		psTDParser->bDebugEnd = true;
	}
}

/****************************************************************************
Function:			tdPrintDragon

Description:	Prints a dragon

Parameters:		psTDParser - the tdParser

Returned:			None
*****************************************************************************/
void tdPrintDragon (FILE *pDragon)
{
	int textLine;

	while (!feof (pDragon))
	{
		textLine = fgetc (pDragon);
		printf ("%c", textLine);
	}

	printf ("\n");

	fclose (pDragon);
}

/****************************************************************************
Function:			tdTypeCheck

Description:	Turns on type checking option

Parameters:		psTDParser - the tdParser
							option	 - the option to turn debug options on or off

Returned:			None
*****************************************************************************/
void tdTypeCheck (tdParser *psTDParser, bool option)
{
	if (true == option)
	{
		psTDParser->bTypeCheck = true;
	}
}

/****************************************************************************
Function:			tdSymbolTable

Description:	Turns on the option to dump the symbol table

Parameters:		psTDParser - the tdParser
							option	 - the option to turn debug options on or off

Returned:			None
*****************************************************************************/
void tdSymbolTable (tdParser *psTDParser, bool option)
{
	if (true == option)
	{
		psTDParser->bSymbolTable = true;
	}
}

/****************************************************************************
Function:			tdAddEntry

Description:	Adds a ST entry to the ST.

Parameters:		psTDParser  - the tdParser
							sTheEntry - the entry to be added

Returned:			None
*****************************************************************************/
static void tdAddEntry (tdParser *psTDParser, Entry sTheEntry)
{
	stInsertName (psTDParser->psSymbolTable, sTheEntry.identifier,
								sTheEntry.sType, sTheEntry.addressField,
								sTheEntry.other, sTheEntry.other);
}

/****************************************************************************
Function:			tdUpdateCurrent

Description:	Updates the TD current token

Parameters:		psTDParser  - the tdParser

Returned:			None
*****************************************************************************/
static void tdUpdateCurrent (tdParser *psTDParser)
{
	psTDParser->sCurrentToken = psTDParser->psLexer->currentToken;
}
