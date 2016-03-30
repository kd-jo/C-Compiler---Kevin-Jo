/****************************************************************************
File name:		lex.c
Author:				Kevin Jo
Date:					2/23/15
Class:				CS480
Assignment:		Lexical Analysis
Purpose:			This is the implementation file for each of the lexical analysis
							functions associated with the lexical analysis module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../include/lex.h"
#include "../include/pcc13defines.h"

static void lexFillBuffer (Lexer *);

/****************************************************************************
Function:			lexCreate

Description:	Creates a lexer.

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
void lexCreate (Lexer *psLexer, FILE *pFile, ConstantTable *psConstantTable,
								RuntimeStack *psRuntimeStack, SymbolTablePtr psSymbolTable)
{
	int index;

	psLexer->pFile = pFile;
	psLexer->bCommentOn = false;
	psLexer->bDebug = true;
	psLexer->lineNumber = 0;
	psLexer->buffIndex = 0;
	ctCreate (psConstantTable);
	rtsCreate (psRuntimeStack);
	psLexer->psConstantTable = psConstantTable;
	psLexer->psRTStack = psRuntimeStack;
	psLexer->psSymbolTable = psSymbolTable;

	for (index = 0; index < MAX_LINE; index++)
	{
		psLexer->buffer [index] = '\0';
	}

	lexFillBuffer (psLexer);
}

/****************************************************************************
Function:			lexToggle

Description:	Toggles the debug option for the lexer.

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
void lexToggle (Lexer *psLexer)
{
	if (true == psLexer->bDebug)
	{
		psLexer->bDebug = false;
	}

	else
	{
		psLexer->bDebug = true;
	}

}

/****************************************************************************
Function:			lexFillBuffer

Description:	Fills the buffer for the lexer from the file included in the
							argument.

Parameters:		psLexer - the lexer
							pFile		- the file to be read from

Returned:			None
*****************************************************************************/
static void lexFillBuffer (Lexer *pLexer)
{
	int index;

	pLexer->lineNumber++;

	for (index = 0; index < MAX_LINE; index++)
	{
		pLexer->buffer [index] = '\0';
	}

	fgets (pLexer->buffer, MAX_LINE, pLexer->pFile);

}

/****************************************************************************
Function:			lexDebug

Description:	Prints the debug option.

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
void lexDebug (Lexer *psLexer)
{
	printf ("Line      %d: %s\n", psLexer->lineNumber, psLexer->buffer);
	printf ("CLASS     LEXEME%34s\n", "VALUE");
	printf ("-----     ------%34s\n", "-----");
}

/****************************************************************************
Function:			lexPrintToken

Description:	Prints a token class, and class types.

Parameters:		sToken - the token to print.

Returned:			None
*****************************************************************************/
void lexPrintToken (Token sToken)
{

	if (' ' != sToken.lexeme [0])
	{
		printf ("%5d     %-6s", sToken.class, sToken.lexeme);

		if (RES_WORD == sToken.class)
		{
			printf ("%34d\n", sToken.value.RWType);
		}

		else if (IDENT == sToken.class)
		{
			printf ("%34lu\n", sToken.value.identifier);
		}

		else if (CONSTANT == sToken.class)
		{
			printf ("%34d\n", sToken.value.constant);
		}

		else if (RELOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.RELOPType);
		}

		else if (ADDOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.ADDOPType);
		}

		else if (MULOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.MULOPType);
		}

		else if (AUTOOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.AUTOOPType);
		}

		else if (ASSIGNOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.assignop);
		}

		else if (ADDRESSOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.addressofop);
		}

		else if (BOOLOP == sToken.class)
		{
			printf ("%34d\n", sToken.value.BOOLOPType);
		}

		else if (SEMICOLON == sToken.class)
		{
			printf ("%34d\n", sToken.value.semicolon);
		}

		else if (COMMA == sToken.class)
		{
			printf ("%34d\n", sToken.value.comma);
		}

		else if (PARENTHESIS == sToken.class)
		{
			printf ("%34d\n", sToken.value.PARENTHESISType);
		}

		else if (BRACKET == sToken.class)
		{
			printf ("%34d\n", sToken.value.BRACKETType);
		}

		else if (BRACE == sToken.class)
		{
			printf ("%34d\n", sToken.value.BRACEType);
		}
	}
}

/****************************************************************************
Function:			lexGetToken

Description:	Retreves a token from the buffer.

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
Token lexGetToken (Lexer *psLexer, ErrorCode *pLexError,
									ErrorHandler *psErrorHandler)
{
	Token theToken;
	bool bFoundToken = false;
	char lookAhead = ' ';
	char lexeme [MAX_LINE];
	int state = 0, i, stackAddress;;
	bool bFound = false;
	ErrorCode lexError = NO_ERROR, stError = NO_ERROR;
	HT_DATATYPE *pEntry;
	HT_DATATYPE pConstant;

	theToken.brValue = false;
	theToken.bAutoop = false;
	psLexer->prevToken = psLexer->currentToken;

	for (i = 0; i < MAX_LINE; i++)
	{
		theToken.lexeme [i] = '\0';
		lexeme [i] = '\0';
	}

	theToken.class = -1;
	theToken.lexemeIndex = 0;

	while (false == bFoundToken)
	{
		switch (state)
		{
			//Start
			case 0:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if (true == psLexer->bCommentOn)
				{
					state = 13;
				}

				else if ('\n' == lookAhead)
				{
					lexFillBuffer (psLexer);
					psErrorHandler->lineNumber++;

					if (true == psLexer->bDebug)
					{
						printf ("\n\n");
						lexDebug (psLexer);
					}

					//bFoundToken = true;
					state = 0;

					lexeme [theToken.lexemeIndex] = ' ';
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));

					psLexer->buffIndex = 0;

				}

				else if (' ' == lookAhead)
				{
					psLexer->buffIndex++;
				}

				else if ('\0' == lookAhead)
				{
					bFoundToken = true;
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
				}

				else if ('<' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 1;
				}

				else if ('>' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 4;
				}

				else if ('=' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 6;
				}

				else if ('!' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 3;
				}

				//Identifiers
				else if (isalpha (lookAhead))
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 7;
				}

				//Addop
				else if ('+' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 8;
				}

				else if ('-' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 9;
				}

				//Mulop / Comments
				else if ('*' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));

					lookAhead = psLexer->buffer [psLexer->buffIndex + 1];

					if (CONSTANT == psLexer->prevToken.class ||
							IDENT == psLexer->prevToken.class)
					{
						theToken.class = MULOP;
						theToken.value.MULOPType = MULTIPLY;
						psLexer->buffIndex++;
					}

					else if (isalpha (lookAhead)|| (0 != isdigit (lookAhead)) ||
							lookAhead == '&' || lookAhead == '*' || lookAhead == '(')
					{
						theToken.class = DEREF;
						theToken.value.derefType = DEREFOP;
						lookAhead = psLexer->buffer [psLexer->buffIndex];
						psLexer->buffIndex++;
					}

					else if (MULOP == psLexer->prevToken.class ||
									 DEREF == psLexer->prevToken.class ||
									 RES_WORD == psLexer->prevToken.class)
					{
						theToken.class = DEREF;
						theToken.value.derefType = DEREFOP;
						lookAhead = psLexer->buffer [psLexer->buffIndex];
						psLexer->buffIndex++;
					}

					else
					{
						theToken.class = MULOP;
						theToken.value.MULOPType = MULTIPLY;
						psLexer->buffIndex++;
					}
					bFoundToken = true;
				}

				else if ('/' == lookAhead)
				{
					psLexer->buffIndex++;
					state = 12;
				}



				else if ('%' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = MULOP;
					theToken.value.MULOPType = MOD;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				//Addressof
				else if ('&' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 10;
				}

				//Boolop
				else if ('|' == lookAhead)
				{
					psLexer->buffIndex++;
					state = 11;
				}

				//SemiColon
				else if (';' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = SEMICOLON;
					theToken.value.semicolon = 0;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				//Comma
				else if (',' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = COMMA;
					theToken.value.comma = 0;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				//Parenthesis
				else if ('(' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = LPAREN;
					theToken.value.PARENTHESISType = BEG_PARENTHESIS;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				else if (')' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = RPAREN;
					theToken.value.PARENTHESISType = END_PARENTHESIS;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				//Bracket
				else if ('[' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = LBRACKET;
					theToken.value.BRACKETType = BEG_BRACKET;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				else if (']' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = RBRACKET;
					theToken.value.BRACKETType = END_BRACKET;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				//Brace
				else if ('{' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = BRACE;
					theToken.value.BRACEType = BEG_BRACE;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				else if ('}' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = BRACE;
					theToken.value.BRACEType = END_BRACE;
					psLexer->buffIndex++;

					bFoundToken = true;
				}

				else if (0 != isdigit (lookAhead))
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.class = CONSTANT;
					theToken.value.constant = 0;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 14;
				}

				//None matched
				else
				{
					(*pLexError) = NO_ERROR;
					theToken.class = 0;
					psLexer->buffIndex++;
				}

				break;

			//Less than
			case 1:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('=' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 2;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = RELOP;
					theToken.value.RELOPType = LESS_THAN;

					bFoundToken = true;
				}

				break;

			//Less than or equal
			case 2:
				theToken.class = RELOP;
				theToken.value.RELOPType = LT_EQUAL;

				bFoundToken = true;

				break;

			//Not equal
			case 3:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('=' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = EQUOP;
					theToken.value.equopType = NOT_EQUAL;
					psLexer->buffIndex++;
					bFoundToken = true;
				}

				else
				{
					(*pLexError) = BAD_CHAR;
					psLexer->buffIndex++;
					bFoundToken = true;
				}

				break;

			//Greater than
			case 4:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('=' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;
					state = 5;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = RELOP;
					theToken.value.RELOPType = GREATER_THAN;

					bFoundToken = true;
				}

				break;

			//Greater than or equal
			case 5:
				memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
				theToken.class = RELOP;
				theToken.value.RELOPType = GT_EQUAL;

				bFoundToken = true;

				break;

			//Equal relop / AssignOP
			case 6:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('=' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = EQUOP;
					theToken.value.equopType = EQUAL;
					psLexer->buffIndex++;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = ASSIGNOP;
					theToken.value.assignop = ASSIGN;
				}

				bFoundToken = true;

				break;

			//Identifiers
			case 7:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if (isalpha (lookAhead) || isdigit (lookAhead))
				{
					while (isalpha (lookAhead) || isdigit (lookAhead))
					{
						lexeme [theToken.lexemeIndex] = lookAhead;
						theToken.lexemeIndex++;
						psLexer->buffIndex++;

						lookAhead = psLexer->buffer [psLexer->buffIndex];
					}
				}

				memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));

				if (0 == strcmp ("main", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_MAIN;
				}

				else if (0 == strcmp ("int", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_INT;
				}

				else if (0 == strcmp ("if", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_IF;
				}

				else if (0 == strcmp ("else", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_ELSE;
				}

				else if (0 == strcmp ("return", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_RETURN;
				}

				else if (0 == strcmp ("for", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_FOR;
				}

				else if (0 == strcmp ("input", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_INPUT;
				}

				else if (0 == strcmp ("output", theToken.lexeme))
				{
					theToken.class = RES_WORD;
					theToken.value.RWType = RSRVWRD_OUTPUT;
				}

				else
				{
					theToken.class = IDENT;
					stError = stSearchEntry (psLexer->psSymbolTable, theToken.lexeme, &pEntry);

					if (NO_ERROR == stError)
					{
						theToken.value.identifier = (long int) pEntry;
					}

					else
					{
						theToken.value.identifier = (long int) NULL;
					}


					//psLexer->buffIndex++;
				}

				bFoundToken = true;

				break;

			//AutoOP
			case 8:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('+' == lookAhead &&
					 (IDENT == psLexer->prevToken.class ||
						RBRACKET == psLexer->prevToken.class ||
						RPAREN == psLexer->prevToken.class))
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = AUTOOP;
					theToken.value.AUTOOPType = INCREMENT;
					psLexer->buffIndex++;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = ADDOP;
					theToken.value.ADDOPType = ADD_OP;
				}

				bFoundToken = true;

				break;

			case 9:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('-' == lookAhead && (IDENT == psLexer->prevToken.class ||
						RPAREN == psLexer->prevToken.class ||
						RBRACKET == psLexer->prevToken.class))
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = AUTOOP;
					theToken.value.AUTOOPType = DECREMENT;
					psLexer->buffIndex++;
					lookAhead = psLexer->buffer [psLexer->buffIndex];
				}

				else if (AUTOOP == psLexer->prevToken.class ||
								 IDENT == psLexer->prevToken.class ||
								 CONSTANT == psLexer->prevToken.class ||
								 RPAREN == psLexer->prevToken.class ||
								 RBRACKET == psLexer->prevToken.class)
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = ADDOP;
					theToken.value.ADDOPType = SUBTRACT_OP;

				}

				else if (ADDOP == psLexer->prevToken.class && isalpha (lookAhead))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else if (ADDOP == psLexer->prevToken.class && ('-' == lookAhead))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else if (NEGOP == psLexer->prevToken.class && isalpha (lookAhead))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else if (NEGOP == psLexer->prevToken.class && (0 != isdigit (lookAhead)))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else if (isalpha (lookAhead))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else if (0 != isdigit (lookAhead))
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = NEGOP;
				}

				bFoundToken = true;

				break;

			//Boolop / Addressof
			case 10:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('&' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = ANDOP;
					theToken.value.BOOLOPType = AND;
					psLexer->buffIndex++;
				}

				else
				{
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = ADDRESSOP;
					theToken.value.addressofop = 0;
				}

				bFoundToken = true;

				break;

			case 11:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('|' == lookAhead)
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					psLexer->buffIndex++;
					theToken.class = OROP;
					theToken.value.BOOLOPType = OR;

					bFoundToken = true;
				}

				else
				{
					(*pLexError) = BAD_CHAR;
					psLexer->buffIndex++;
					bFoundToken = true;
				}

				break;

			//Comments / divide
			case 12:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				if ('*' == lookAhead)
				{
					psLexer->bCommentOn = true;
					psLexer->buffIndex++;
					state = 13;
				}

				else
				{
					lookAhead = psLexer->buffer [psLexer->buffIndex - 1];
					lexeme [theToken.lexemeIndex] = lookAhead;
					memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
					theToken.class = MULOP;
					theToken.value.MULOPType = DIVIDE;
					//psLexer->buffIndex++;

					bFoundToken = true;
				}

				break;

			case 13:
				if (true == psLexer->bCommentOn)
				{
					lookAhead = psLexer->buffer [psLexer->buffIndex];

					if ('*' == lookAhead)
					{
						psLexer->buffIndex++;
						lookAhead = psLexer->buffer [psLexer->buffIndex];

						if ('/' == lookAhead)
						{
							psLexer->bCommentOn = false;
							psLexer->buffIndex++;
							lexeme [0] = ' ';
							memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
							state = 0;
						}
					}

					else if ('\n' == lookAhead)
					{
						lexFillBuffer (psLexer);

						if (true == psLexer->bDebug)
						{
							lexDebug (psLexer);
						}

						state = 0;

						lexeme [theToken.lexemeIndex] = ' ';
						memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));

						psLexer->buffIndex = 0;
					}

					else
					{
						bFoundToken = false;
						psLexer->buffIndex++;
						state = 13;
					}
				}

				break;

			//Constants
			case 14:
				lookAhead = psLexer->buffer [psLexer->buffIndex];

				while (0 != isdigit (lookAhead))
				{
					lexeme [theToken.lexemeIndex] = lookAhead;
					theToken.lexemeIndex++;
					psLexer->buffIndex++;

					lookAhead = psLexer->buffer [psLexer->buffIndex];
				}

				bFoundToken = true;
				memcpy (theToken.lexeme, lexeme, sizeof (theToken.lexeme));
				ctSearch (psLexer->psConstantTable, theToken.lexeme, &bFound, &pConstant);

				if (false == bFound)
				{
					lexError = rtsInsert (psLexer->psRTStack, atoi (theToken.lexeme), &stackAddress);

					if (OUT_OF_MEMORY != lexError)
					{
						ctInsert (psLexer->psConstantTable, theToken.lexeme, stackAddress);
						theToken.value.constant = stackAddress;
					}

					else
					{
						theToken.lexeme [0] = ' ';
						(*pLexError) = lexError;
					}
				}

				else
				{
					theToken.value.constant = pConstant.ctData.stackAddress;
				}

				break;
		}
	}

	psLexer->currentToken = theToken;
	(*pLexError) = lexError;
	return theToken;
}

/****************************************************************************
Function:			lexDecrementBuffIndex

Description:	Decrements the buffer index in the lexer.

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
void lexDecrementBuffIndex (Lexer *psLexer)
{
	if (psLexer->buffIndex > 0)
	{
		psLexer->buffIndex--;
	}
}

/****************************************************************************
Function:			lexSetBuffIndex

Description:	Sets the buffer index in the lexer

Parameters:		psLexer - the lexer
							buffIndex - the value to set the buffer to

Returned:			None
*****************************************************************************/
void lexSetBuffIndex (Lexer *psLexer, int buffIndex)
{
	psLexer->buffIndex = buffIndex;
}

/****************************************************************************
Function:			lexCheckEOF

Description:	Checks the buffer for end of file

Parameters:		psLexer - the lexer

Returned:			None
*****************************************************************************/
bool lexCheckEOF (Lexer *psLexer)
{
	bool bEOF = false;

	if ('\n' == psLexer->buffer [psLexer->buffIndex])
	{
		bEOF = false;
	}

	else if ('\0' == psLexer->buffer [psLexer->buffIndex + 1])
	{
		bEOF = true;
	}


	return bEOF;
}
