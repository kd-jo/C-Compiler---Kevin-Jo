/****************************************************************************
File name:		bu.c
Author:				Kevin Jo
Date:					4/20/2015
Class:				CS480
Assignment:		Bottom Up Parser
Purpose:			This is the implementation file for each of the bottom up parser
							functions associated with the bottom up parser module.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/bu.h"
#include "../include/er.h"
#include "../include/ct.h"
#include "../include/rts.h"

static int glOperatorPrecedenceTable[NUM_CLASSES][NUM_CLASSES] =
       {{1,1,1,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,1,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,2,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {2,2,2,2,2,2,2,0,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {2,2,2,2,2,2,2,0,0,0,1,2,0,2,0,1,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,3,0,0,1,1,1,1,0},
        {2,2,2,2,2,2,2,2,0,0,0,2,0,2,0,0,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,0,0,3,1,1,0,1,0},
        {2,2,2,2,2,2,2,2,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,0,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,2,0,0,3,2,3,2,0,0,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,2,0,0,1,1,1,1,0},
        /* The first int in the next row was previously a 3,
	   	  Removing
				exp -> * unexp assignop exp
	   	 	and adding
				exp -> derefexp assignop exp
				derefexp -> * unexp
	   	 	in the grammar fixes the problem of both
        * =. assignop
	   	 	and
        * >. assignop
  	   	being derived from the grammar.
	   	 	This allows expressions such as **id = 2 to be
	   	 	parsed correctly.
	   	 	ccw 11 march 2009
	*/
        {2,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {1,1,1,1,1,1,1,1,1,1,1,4,0,0,1,1,4,1,4}};

static Token buPopStack (buParser *);
static void buPushStack (buParser *, Token);
static void buCreateTemp (buParser *, Token *);

/****************************************************************************
Function:			buCreate

Description:	Creates a bottom up parser.

Parameters:		psbuParser - the BU parser
							psTheLexer - the lexer to be created for the BU parser
							pFile			 - the input file
							psCT			 - the constant table to be created in the lexer
							psRTS			 - the runtime stack to be craeted in the lexer

Returned:			None
*****************************************************************************/
void buCreate (buParser *psbuParser, Lexer *psTheLexer, FILE *pFile,
							ConstantTable *psCT, RuntimeStack *psRTS,
							ErrorHandler *psErrorhandler, SymbolTablePtr psSymbolTable)
{
	Token sTempToken;
	psbuParser->sBuStack = (Token *) malloc (MAX_OPERATORS * sizeof (Token));
	psbuParser->sInputBuffer = (Token *) malloc (MAX_OPERATORS *
															sizeof (Token));
	sTempToken.class = DOLLAR;
	lexCreate (psTheLexer, pFile, psCT, psRTS, psSymbolTable);
	psbuParser->psLexer = psTheLexer;
	lexToggle (psTheLexer);
	psbuParser->stackIndex = 0;
	psbuParser->inputSize = 0;
	psbuParser->inputIndex = 0;
	psbuParser->sBuStack [psbuParser->stackIndex] = sTempToken;
	buInitStack (psbuParser);
	psbuParser->bDebug = false;
	psbuParser->bLexDebug = false;
	psbuParser->psErrorHandler = psErrorhandler;
	psbuParser->psST =psSymbolTable;
	quadFileCreate (&psbuParser->sTempQuad);
	psbuParser->bFullExp = false;
	psbuParser->bArguments = false;
	psbuParser->bInitExp = false;
	psbuParser->bOutput = false;
}

/****************************************************************************
Function:			buDebug

Description:	This function is used for the BU debug options.

Parameters:		buParser - psbuParser
							option 	 - option

Returned:			None
*****************************************************************************/
void buDebug (buParser *psbuParser, bool option)
{
	if (true == option)
	{
		psbuParser->bDebug = true;
	}

	else
	{
		psbuParser->bDebug = false;
	}
}

/****************************************************************************
Function:			buFillInput

Description:	This function fills the input buffer.

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buFillInput (buParser *psbuParser)
{
	ErrorCode lexError = NO_ERROR;
	Token sTempToken, sEndToken;
	bool bEnd = false;
	int numLParen = 1, numRParen = 0;

	buInitInput (psbuParser);
	sTempToken.lexeme [0] = '\0';
	sEndToken.class = DOLLAR;
	psbuParser->inputIndex = 0;
	psbuParser->stackIndex = 0;
	psbuParser->inputSize = 0;

	sTempToken = lexGetToken (psbuParser->psLexer, &lexError,
														psbuParser->psErrorHandler);

	if (true == psbuParser->psLexer->bDebug)
	{
		lexPrintToken (sTempToken);
	}

	if (sTempToken.class == LPAREN)
	{
		numLParen++;
	}

	if (psbuParser->bFullExp == true)
	{
		while (sTempToken.class != SEMICOLON)
		{
			psbuParser->sInputBuffer [psbuParser->inputIndex] = sTempToken;
			sTempToken = lexGetToken (psbuParser->psLexer, &lexError,
																psbuParser->psErrorHandler);

			if (true == psbuParser->psLexer->bDebug)
			{
				lexPrintToken (sTempToken);
			}

			if (true == psbuParser->bDebug)
			{
				printf ("%s", psbuParser->sInputBuffer [psbuParser->inputIndex].lexeme);
			}

			if (psbuParser->inputIndex < MAX_OPERATORS)
			{
				psbuParser->inputIndex++;
			}

			psbuParser->inputSize++;
		}
	}

	else if (psbuParser->bInitExp == true)
	{
		while (sTempToken.class != SEMICOLON)
		{
			psbuParser->sInputBuffer [psbuParser->inputIndex] = sTempToken;
			sTempToken = lexGetToken (psbuParser->psLexer, &lexError,
																psbuParser->psErrorHandler);

			if (true == psbuParser->psLexer->bDebug)
			{
				lexPrintToken (sTempToken);
			}

			if (true == psbuParser->bDebug)
			{
				printf ("%s", psbuParser->sInputBuffer [psbuParser->inputIndex].lexeme);
			}

			if (psbuParser->inputIndex < MAX_OPERATORS)
			{
				psbuParser->inputIndex++;
			}

			psbuParser->inputSize++;

			if (sTempToken.class == COMMA)
			{
				break;
			}
		}
	}

	//Read until a RParen or Comma
	else
	{
		while (false == bEnd)
		{
			psbuParser->sInputBuffer [psbuParser->inputIndex] = sTempToken;
			sTempToken = lexGetToken (psbuParser->psLexer, &lexError,
																psbuParser->psErrorHandler);

			if (sTempToken.class == LPAREN)
			{
				numLParen++;
			}

			else if (sTempToken.class == RPAREN)
			{
				numRParen++;

				if ((numLParen) == (numRParen))
				{
					bEnd = true;
				}
			}

			if (sTempToken.class == COMMA)
			{
				if ((numLParen - 1) > numRParen)
				{
					bEnd = false;
				}

				else
				{
					bEnd = true;
				}
			}

			else if (sTempToken.class == BRACE &&
							 sTempToken.value.BRACEType == END_BRACE)
			{
				bEnd = true;
			}

			if (true == psbuParser->psLexer->bDebug)
			{
				lexPrintToken (sTempToken);
			}

			if (true == psbuParser->bDebug)
			{
				printf ("%s", psbuParser->sInputBuffer [psbuParser->inputIndex].lexeme);
			}

			if (psbuParser->inputIndex < MAX_OPERATORS)
			{
				psbuParser->inputIndex++;
			}

			psbuParser->inputSize++;
		}
	}

	psbuParser->sInputBuffer [psbuParser->inputIndex] = sTempToken;

	if (true == psbuParser->bDebug)
	{
		printf ("%s\n", psbuParser->sInputBuffer [psbuParser->inputIndex].lexeme);
	}

	psbuParser->inputIndex++;
	psbuParser->sInputBuffer [psbuParser->inputIndex] = sEndToken;
	psbuParser->inputIndex = 0;
}

/****************************************************************************
Function:			buInitInput

Description:	This function is used to initialize the data in the input
							buffer

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buInitInput (buParser *psbuParser)
{
	Token sNothingToken;
	int index;

	psbuParser->inputIndex = 0;
	index = psbuParser->inputIndex;
	sNothingToken.class = -1;

	for (index = 0; index < MAX_OPERATORS; index++)
	{
		psbuParser->sInputBuffer [index] = sNothingToken;
	}
}

/****************************************************************************
Function:			buParse

Description:	This function is will parse the tokens from the input buffer
							using a stack.

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buParse (buParser *psbuParser)
{
	ErrorCode buError = NO_ERROR;
	Token topStack, topInput;
	Quad sTempQuad;
	bool bEnd = false;
	int action = 0, tempIndex, i;

	buFillInput (psbuParser);

	while (false == bEnd)
	{
		topStack = psbuParser->sBuStack [psbuParser->stackIndex];
		topInput = psbuParser->sInputBuffer [psbuParser->inputIndex];

		if (UNDEF != topInput.class)
		{
			tempIndex = psbuParser->stackIndex;

			if (NO_ERROR == buError)
			{
				while (DUMMY == topStack.class)
				{
					tempIndex--;
					topStack = psbuParser->sBuStack [tempIndex];
				}

				if (SEMICOLON == topInput.class)
				{
					topInput.class = DOLLAR;
				}

				else if ((RPAREN == topInput.class && DOLLAR == topStack.class) ||
									(BRACE == topInput.class && DOLLAR == topStack.class))
				{
					topInput.class = DOLLAR;
				}

				action = glOperatorPrecedenceTable [topStack.class][topInput.class];

				if (action == ERROR)
				{
					buError = BAD_EXPRESSION;
					erAddError (psbuParser->psErrorHandler, buError, EMPTYSTRING);
				}

				else if (action == YIELDS || action == EQUALS)
				{
					buShift (psbuParser);
				}

				else if (action == TAKES)
				{
					topStack = psbuParser->sBuStack [psbuParser->stackIndex];
					buReduce (psbuParser, topStack, topInput, tempIndex);
				}

				else
				{
					if (psbuParser->bDebug == true)
					{
						printf ("---ACCEPT---\n\n");
					}


					psbuParser->sTopStack = psbuParser->sBuStack [1];

					//Add autoops at end. If not in argument list
					if (psbuParser->bArguments == false)
					{
						if (psbuParser->sTempQuad.quadLevel > 0)
						{
							for (i = 0; i < psbuParser->sTempQuad.quadLevel; i++)
							{
								sTempQuad = psbuParser->sTempQuad.sQuadFile [i];
								quadInsert (psbuParser->psQuadFile, sTempQuad);
							}

							psbuParser->sTempQuad.quadLevel = 0;
						}
					}

					bEnd = true;
				}
			}

			else
			{
				if (psbuParser->bDebug == true)
				{
					printf ("---REJECT---\n\n");
				}

				bEnd = true;
			}
		}

		else
		{
			bEnd = true;
		}
	}
}

/****************************************************************************
Function:			buShift

Description:	This function is used to push a token on to the stack when a
							there is a yield comparison between two tokens.

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buShift (buParser *psbuParser)
{
	Token sDummyToken, sTempToken;

	sDummyToken.class = DUMMY;
	sTempToken = psbuParser->sInputBuffer [psbuParser->inputIndex];

	if (sTempToken.class == ADDRESSOP)
	{
		psbuParser->bAddressOp = true;
	}

	psbuParser->stackIndex++;
	psbuParser->sBuStack [psbuParser->stackIndex] = sTempToken;
	psbuParser->sInputBuffer [psbuParser->inputIndex] = sDummyToken;
	psbuParser->inputIndex++;
}

/****************************************************************************
Function:			buReduce

Description:	This function is used to reduce tokens into terminals using a
							Operator Precedence table.

Parameters:		buParser  - psbuParser
							sTopStack - the token on the top of the stack
							sTopInput - the token on the top of the input buffer

Returned:			None
*****************************************************************************/
void buReduce (buParser *psbuParser, Token sTopStack, Token sTopInput,
							 int tempIndex)
{
	Token sDummyToken, sPopToken, sTempToken, sTempToken1, sTempToken2;
	HT_DATATYPE *pEntry;
	Quad sQuad;
	int index = 0, action = 0;

	quadInit (&sQuad);

	sDummyToken.class = DUMMY;
	sDummyToken.bPointer = false;


	if (psbuParser->stackIndex > 0)
	{
		sPopToken = buPopStack (psbuParser);
	}

	while (DUMMY == sPopToken.class)
	{
		if (psbuParser->stackIndex > 0)
		{
			sPopToken = buPopStack (psbuParser);
		}
	}

	if (psbuParser->stackIndex < MAX_OPERATORS && psbuParser->stackIndex > 0)
	{
		index = psbuParser->stackIndex;
	}

	while (DUMMY == psbuParser->sBuStack [index].class)
	{
		if (psbuParser->stackIndex > 0)
		{
			buPopStack (psbuParser);
		}

		index = psbuParser->stackIndex;
	}

	if (sPopToken.class == IDENT)
	{
		pEntry = (HT_DATATYPE *) sPopToken.value.identifier;
		sDummyToken.field = pEntry->stData.idAddressField;
		sDummyToken.level = pEntry->stData.level;

		if (sTopInput.class == ASSIGNOP)
		{
			sDummyToken.brValue = false;
		}

		else if (sTopInput.class == ADDOP || sTopInput.class == MULOP)
		{
			sDummyToken.brValue = true;
		}

		else if (sTopInput.class == RELOP || sTopInput.class == EQUOP ||
						 sTopInput.class == ANDOP || sTopInput.class == OROP)
		{
			sDummyToken.brValue = true;
		}

		else if (sTopInput.class == AUTOOP)
		{
			sDummyToken.bAutoop = true;
			sDummyToken.brValue = false;
		}

		if (pEntry->stData.idType == FUNCNAME)
		{
			sQuad.level = psbuParser->psST->currentLevel;
			sQuad.opCode = OP_FUNCCALL;

			sTempToken.level = RSRVWRD_LEVEL;
			sTempToken.field = pEntry->stData.idAddressField;

			buAddQuad (psbuParser, NULL, NULL, &sTempToken, sQuad);
		}

		if (psbuParser->bOutput == true)
		{
			sDummyToken.brValue = true;
		}
	}

	else if (sPopToken.class == DEREF)
	{
		sDummyToken.brValue = true;
		sDummyToken.field = sTopStack.field;
		sDummyToken.bPointer = true;

		if (sTopInput.class != ASSIGNOP)
		{
			buCreateTemp (psbuParser, &sTempToken1);
			sTempToken1.brValue = false;
			sDummyToken.field = sTempToken1.field;
			sDummyToken.bPointer = false;

			sTempToken = sTopStack;
			sTempToken.brValue = true;

			sQuad.opCode = OP_DEREFERENCE;
			sQuad.level = psbuParser->psST->currentLevel;
			buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken1, sQuad);
		}
	}

	else if (sPopToken.class == RBRACKET)
	{
		buCreateTemp (psbuParser, &sTempToken2);

		//Deref arrays.
		sTempToken2.brValue = false;
		sTempToken2.level = psbuParser->psST->currentLevel;
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_ADD;
		sTempToken1 = psbuParser->sBuStack [index + 1];
		sTempToken1.brValue = true;

		if (sTopInput.class == AUTOOP)
		{
			sTempToken2.bAutoop = true;
		}

		else if (sTopInput.class == ASSIGNOP)
		{
			sTempToken2.brValue = false;
		}
	}

	else if (sPopToken.class == ADDRESSOP)
	{
		sQuad.opCode = OP_ASSIGN;
		sQuad.level = psbuParser->psST->currentLevel;

		//Create Temp Variable
		buCreateTemp (psbuParser, &sTempToken1);

		sTempToken1.brValue = false;

		sTempToken = sTopStack;
		sTempToken.brValue = false;

		sDummyToken.brValue = false;
		sDummyToken.field = sTempToken1.field;
		sDummyToken.bPointer = sTempToken.bPointer;

		if (sTempToken.bPointer == true)
		{
			sDummyToken.brValue = true;
		}

		sDummyToken.level = sTempToken1.level;

		psbuParser->bAddressOp = false;

		buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken1, sQuad);
	}

	else if (sPopToken.class == NEGOP)
	{
		sQuad.opCode = OP_MINUS;
		sQuad.level = psbuParser->psST->currentLevel;

		//Create Temp Variable
		buCreateTemp (psbuParser, &sTempToken1);

		sDummyToken.brValue = true;
		sDummyToken.field = sTempToken1.field;
		sDummyToken.level = psbuParser->psST->currentLevel;
		sTopStack.brValue = true;

		buAddQuad (psbuParser, &sTopStack, NULL, &sTempToken1, sQuad);
	}

	else if (sPopToken.class == CONSTANT)
	{
		sDummyToken.level = GLOBAL_LEVEL;
		sDummyToken.field = sPopToken.value.constant;
		sDummyToken.brValue = true;
	}

	else if (sPopToken.class == MULOP)
	{
		if (sPopToken.value.MULOPType == MULTIPLY)
		{
			sQuad.opCode = OP_MULTIPLY;
		}

		else if (sPopToken.value.MULOPType == DIVIDE)
		{
			sQuad.opCode = OP_DIVIDE;
		}

		else if (sPopToken.value.MULOPType == MOD)
		{
			sQuad.opCode = OP_MODULUS;
		}

		sQuad.level = psbuParser->psST->currentLevel;

		//Create Temp Variable
		buCreateTemp (psbuParser, &sTempToken1);
		sTempToken1.brValue = false;

		sTempToken = psbuParser->sBuStack [index + 1];

		sDummyToken.level = sTempToken1.level;
		sDummyToken.field = sTempToken1.field;
		sDummyToken.brValue = true;

		buAddQuad (psbuParser, &sTempToken, &sTopStack, &sTempToken1, sQuad);
	}

	else if (sPopToken.class == ADDOP)
	{
		if (sPopToken.value.ADDOPType == ADD_OP)
		{
			sQuad.opCode = OP_ADD;
		}

		else if (sPopToken.value.ADDOPType == SUBTRACT_OP)
		{
			sQuad.opCode = OP_SUBTRACT;
		}

		sQuad.level = psbuParser->psST->currentLevel;

		//Create Temp Variable
		buCreateTemp (psbuParser, &sTempToken1);
		sTempToken1.brValue = false;

		sTempToken = psbuParser->sBuStack [index + 1];

		sDummyToken.level = sTempToken1.level;
		sDummyToken.field = sTempToken1.field;
		sDummyToken.brValue = true;

		buAddQuad (psbuParser, &sTempToken, &sTopStack, &sTempToken1, sQuad);
	}

	else if (sPopToken.class == RELOP)
	{
		buRelop (psbuParser, sPopToken, sTopStack, &sDummyToken, index);
	}

	else if (sPopToken.class == ANDOP || sPopToken.class == OROP)
	{
		buBoolop (psbuParser, sPopToken, sTopStack, &sDummyToken, index);
	}

	else if (sPopToken.class == EQUOP)
	{
		buEquop (psbuParser, sPopToken, sTopStack, &sDummyToken, index);
	}

	else if (sPopToken.class == ASSIGNOP)
	{
		//sTempToken is now the token to the left of the assign op
		sTempToken = psbuParser->sBuStack [index + 1];
		sQuad.level = psbuParser->psST->currentLevel;

		//If the dummy token on the left side of the ASSIGN_OP is a pointer,
		//make it a rvalue.
		//Pointer = non-pointer
		if (sTempToken.bPointer == true && sTopStack.bPointer == false)
		{
			sTempToken.brValue = true;
			sQuad.opCode = OP_ASSIGN;
			sDummyToken.level = sTempToken.level;
			sDummyToken.field = sTempToken.field;
			sDummyToken.brValue = true;

			buAddQuad (psbuParser, &sTopStack, NULL, &sTempToken, sQuad);
		}

		//If the dummy token on RIGHT side of ASSIGN_OP is a pointer, dereference
		//it
		else if (sTopStack.bPointer == true)
		{
			sQuad.opCode = OP_DEREFERENCE;

			//Create Temp Variable
			buCreateTemp (psbuParser, &sTempToken1);
			sTempToken1.brValue = false;

			sDummyToken.field = sTempToken1.field;
			sDummyToken.level = sTempToken1.level;
			sDummyToken.brValue = sTempToken1.brValue;

			buAddQuad (psbuParser, &sTopStack, NULL, &sTempToken1, sQuad);

			quadInit (&sQuad);
			sQuad.opCode = OP_ASSIGN;
			sQuad.level = psbuParser->psST->currentLevel;
			sTempToken1.brValue = true;

			buAddQuad (psbuParser, &sTempToken1, NULL, &sTempToken, sQuad);
		}

		//Non-pointer = Non-pointer
		else if (sTempToken.bPointer == false && sTopStack.bPointer == false)
		{
			sQuad.opCode = OP_ASSIGN;
			sDummyToken.level = sTempToken.level;
			sDummyToken.field = sTempToken.field;
			sDummyToken.brValue = sTempToken.brValue;
			sTopStack.brValue = true;

			buAddQuad (psbuParser, &sTopStack, NULL, &sTempToken, sQuad);
		}
	}

	//For function calls with PARAMETERS
	else if (sPopToken.class == COMMA)
	{
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_PUSHPARAM;

		sTempToken = psbuParser->sBuStack [index + 1];

		sDummyToken.level = sTempToken.level;
		sDummyToken.field = sTempToken.field;
		sDummyToken.brValue = sTempToken.brValue;
		sTopStack.brValue = true;

		buAddQuad (psbuParser, &sTopStack, NULL, NULL, sQuad);
	}

	//For function calls with PARAMETERS
	else if (sPopToken.class == RPAREN)
	{
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_PUSHPARAM;

		sTempToken = psbuParser->sBuStack [index + 1];

		sDummyToken.level = sTempToken.level;
		sDummyToken.field = sTempToken.field;
		sDummyToken.brValue = sTempToken.brValue;
		sDummyToken.bPointer = sTempToken.bPointer;
	}

	else if (sPopToken.class == AUTOOP)
	{
		//Get the identifier that needs AUTOOP
		sTempToken = psbuParser->sBuStack [index + 1];

		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.addressMode [0] = 0;
		sQuad.addressMode [1] = 0;

		if (sTempToken.level == GLOBAL_LEVEL)
		{
			if (sTempToken.brValue == false)
			{
				sQuad.addressMode [2] =  GLOBAL_LVALUE;
			}

			else
			{
				sQuad.addressMode [2] =  GLOBAL_RVALUE;
			}

		}

		else
		{
			if (sTempToken.brValue == false)
			{
				sQuad.addressMode [2] =  LOCAL_LVALUE;
			}

			else
			{
				sQuad.addressMode [2] =  LOCAL_RVALUE;
			}
		}

		sQuad.operands [0] = 0;
		sQuad.operands [1] = 0;
		sQuad.operands [2] = sTempToken.field;

		if (sPopToken.value.AUTOOPType == INCREMENT)
		{
			sQuad.opCode = OP_INCREMENT;
		}

		else
		{
			sQuad.opCode = OP_DECREMENT;
		}

		sDummyToken.brValue = true;

		quadInsert (&psbuParser->sTempQuad, sQuad);

		/*
		if (psbuParser->bForLoop == false)
		{
			quadInsert (psbuParser->psQuadFile, sTempQuad);
		}

		else
		{
			quadInsert (&psbuParser->sForQuad, sTempQuad);
		}
		*/
	}

	if (psbuParser->sBuStack != NULL)
	{
		action = glOperatorPrecedenceTable [psbuParser->sBuStack [index].class]
		                                    [sPopToken.class];
	}

	while (YIELDS != action)
	{
		if (DUMMY != sPopToken.class)
		{
			sPopToken = buPopStack (psbuParser);
		}

		if (psbuParser->stackIndex >= 0)
		{
			index--;
		}

		while (DUMMY == psbuParser->sBuStack [index].class)
		{
			buPopStack (psbuParser);
			index--;
		}

		//For deref arrays.
		if (sTopStack.class == RBRACKET && sPopToken.class == IDENT)
		{
			pEntry = (HT_DATATYPE *) sPopToken.value.identifier;
			sTempToken.field = pEntry->stData.idAddressField;
			sTempToken.level = pEntry->stData.level;
			sTempToken.brValue = true;
			sDummyToken.field = sTempToken2.field;
			sDummyToken.level = sTempToken2.level;
			sDummyToken.brValue = true;
			sDummyToken.bPointer = true;

			buAddQuad (psbuParser, &sTempToken, &sTempToken1, &sTempToken2, sQuad);

			//If array is on RHS of ASSIGNOP, dereference.
			if (sTopInput.class != ASSIGNOP && psbuParser->bAddressOp == false)
			{
				sQuad.opCode = OP_DEREFERENCE;

				buCreateTemp (psbuParser, &sTempToken1);
				sTempToken1.brValue = false;
				sTempToken2.brValue = true;

				if (sTopInput.class == AUTOOP)
				{
					sDummyToken.brValue = true;
					sDummyToken.bAutoop = true;
					sDummyToken.bPointer = true;
					sDummyToken.field = sTempToken2.field;
					sDummyToken.level = sTempToken2.level;
					buPushStack (psbuParser, sDummyToken);
				}

				sDummyToken.field = sTempToken1.field;
				sDummyToken.level = sTempToken1.level;
				sDummyToken.bAutoop = false;
				sDummyToken.bPointer = false;


				buAddQuad (psbuParser, &sTempToken2, NULL, &sTempToken1, sQuad);
			}

		}

		//For the end of function calls.
		else if (sTopStack.class == RPAREN && sPopToken.class == IDENT)
		{
			pEntry = (HT_DATATYPE *) sPopToken.value.identifier;

			if (pEntry->stData.idType == FUNCNAME)
			{
				sTempToken.brValue = true;
				buAddQuad (psbuParser, &sTempToken, NULL, NULL, sQuad);

				sQuad.level = psbuParser->psST->currentLevel;
				sQuad.opCode = OP_FUNCCALL;

				sTempToken.level = RSRVWRD_LEVEL;
				sTempToken.field = pEntry->stData.paramCount;

				sTempToken2.level = RSRVWRD_LEVEL;
				sTempToken2.field = pEntry->stData.idAddressField;

				buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken2, sQuad);

				sDummyToken.field = 0;
				sDummyToken.level = GLOBAL_LEVEL;
			}

			else if (pEntry->stData.idType == POINTER)
			{
				sDummyToken.bPointer = true;
			}
		}

		action = glOperatorPrecedenceTable [psbuParser->sBuStack [index].class]
		                                    [sPopToken.class];
	}

	buPushStack (psbuParser, sDummyToken);
	printf ("\n");
}

/****************************************************************************
Function:			buPopStack

Description:	This function is will pop a token off the stack.

Parameters:		buParser - psbuParser

Returned:			The popped token from the stack.
*****************************************************************************/
static Token buPopStack (buParser *psbuParser)
{
	Token sTempToken;
	sTempToken.class = -1;

	if (psbuParser->stackIndex < MAX_OPERATORS && psbuParser->stackIndex > 0)
	{
		if (NULL != psbuParser->sBuStack)
		{
			sTempToken = psbuParser->sBuStack [psbuParser->stackIndex];
		}
	}

	if (psbuParser->stackIndex < MAX_OPERATORS && psbuParser->stackIndex >= 0)
	{
		psbuParser->stackIndex--;
	}

	if (psbuParser->bDebug == true)
	{
		printf ("%d ", sTempToken.class);
	}

	return sTempToken;
}

/****************************************************************************
Function:			buPushStack

Description:	This function is will push a token onto the stack.

Parameters:		buParser 	 - psbuParser
							sPushToken - the token to be pushed.

Returned:			None
*****************************************************************************/
static void buPushStack (buParser *psbuParser, Token sPushToken)
{
	psbuParser->stackIndex++;
	psbuParser->sBuStack [psbuParser->stackIndex] = sPushToken;
}

/****************************************************************************
Function:			buInitStack

Description:	This function is will initialize the stack in the parser.

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buInitStack (buParser *psbuParser)
{
	Token sTempToken;
	int index;

	sTempToken.class = -1;

	for (index = 1; index < MAX_OPERATORS; index++)
	{
		psbuParser->sBuStack [index] = sTempToken;
	}
}

/****************************************************************************
Function:			buDispose

Description:	This function is will dispose the bottom up parser.

Parameters:		buParser - psbuParser

Returned:			None
*****************************************************************************/
void buDispose (buParser *psbuParser)
{
	free (psbuParser->sBuStack);
	free (psbuParser->sInputBuffer);
}

/****************************************************************************
Function:			buAddQuad

Description:	This function is will add a quad given the level and Rvalue or
							LValue and opcodes.

Parameters:		psbuParser - the bu parser
							psToken	 	 - one token loaded with operand and R or L value
							psToken2	 - one token loaded with operand and R or L value
							psToken3	 - one token loaded with operand and R or L value
							sQuad			 - the quad file that has the opcode

Returned:			None
*****************************************************************************/
void buAddQuad (buParser *psbuParser, Token *psToken, Token *psToken2,
								Token *psToken3, Quad sQuad)
{
	if (NULL != psToken)
	{
		if (psToken->level == GLOBAL_LEVEL)
		{
			if (psToken->brValue == true)
			{
				sQuad.addressMode [0] = GLOBAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [0] = GLOBAL_LVALUE;
			}
		}

		else if (psToken->level == LOCAL_LEVEL)
		{
			if (psToken->brValue == true)
			{
				sQuad.addressMode [0] = LOCAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [0] = LOCAL_LVALUE;
			}
		}

		else
		{
			sQuad.addressMode [0] = 0;
		}

		sQuad.operands [0] = psToken->field;
	}

	else
	{
		sQuad.addressMode [0] = 0;
		sQuad.operands [0] = 0;
	}

	if (NULL != psToken2)
	{
		if (psToken2->level == GLOBAL_LEVEL)
		{
			if (psToken2->brValue == true)
			{
				sQuad.addressMode [1] = GLOBAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [1] = GLOBAL_LVALUE;
			}
		}

		else if (psToken2->level == LOCAL_LEVEL)
		{
			if (psToken2->brValue == true)
			{
				sQuad.addressMode [1] = LOCAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [1] = LOCAL_LVALUE;
			}
		}

		else
		{
			sQuad.addressMode [1] = 0;
		}

		sQuad.operands [1] = psToken2->field;
	}

	else
	{
		sQuad.addressMode [1] = 0;
		sQuad.operands [1] = 0;
	}

	if (NULL != psToken3)
	{
		if (psToken3->level == GLOBAL_LEVEL)
		{
			if (psToken3->brValue == true)
			{
				sQuad.addressMode [2] = GLOBAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [2] = GLOBAL_LVALUE;
			}
		}

		else if (psToken3->level == LOCAL_LEVEL)
		{
			if (psToken3->brValue == true)
			{
				sQuad.addressMode [2] = LOCAL_RVALUE;
			}

			else
			{
				sQuad.addressMode [2] = LOCAL_LVALUE;
			}
		}

		else
		{
			sQuad.addressMode [2] = 0;
		}

		sQuad.operands [2] = psToken3->field;
	}

	else
	{
		sQuad.addressMode [2] = 0;
		sQuad.operands [2] = 0;
	}

	quadInsert (psbuParser->psQuadFile, sQuad);

	/*

	if (psbuParser->bForLoop == false)
	{
		quadInsert (psbuParser->psQuadFile, sQuad);
	}

	else
	{
		quadInsert (&psbuParser->sForQuad, sQuad);
	}
	*/
}

/****************************************************************************
Function:			buCreateTemp

Description:	Creates a temp variable in the local data

Parameters:		psbuParser - the bu parser
							psToken	 	 - the temp token that will be passed back

Returned:			None
*****************************************************************************/
static void buCreateTemp (buParser *psbuParser, Token *psToken)
{
	psToken->level = LOCAL_LEVEL;
	psToken->field = psbuParser->psQuadFile->localVarAddress + 1;
	psbuParser->psQuadFile->localVarAddress =
			psbuParser->psQuadFile->localVarAddress + 1;

	quadPatchFuncBegin (psbuParser->psQuadFile, OP_FUNCBEGIN, 1);
}

/****************************************************************************
Function:			buRelop

Description:	Handles all relops that comes across the BU

Parameters:		psbuParser 	 - the bu parser
							sPopToken	 	 - the token that was most recently popped
							sTopStack	 	 - the token on the top stack.
							psDummyToken - the dummy token that will get pushed
							index				 - the index of the stack

Returned:			None
*****************************************************************************/
void buRelop (buParser *psbuParser, Token sPopToken, Token sTopStack,
							Token *psDummyToken , int index)
{
	Quad sQuad;
	Token sTempToken, sTempToken1, sTempToken2;
	int currentQuad = 0;

	currentQuad = psbuParser->psQuadFile->quadLevel;

	if (sPopToken.class == RELOP)
	{
		if (sPopToken.value.RELOPType == LT_EQUAL)
		{
			sQuad.opCode = OP_BLE;
		}

		else if (sPopToken.value.RELOPType == GT_EQUAL)
		{
			sQuad.opCode = OP_BGE;
		}

		else if (sPopToken.value.RELOPType == LESS_THAN)
		{
			sQuad.opCode = OP_BLT;
		}

		else if (sPopToken.value.RELOPType == GREATER_THAN)
		{
			sQuad.opCode = OP_BGT;
		}

		sQuad.level = psbuParser->psST->currentLevel;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 3;

		sTempToken = psbuParser->sBuStack [index + 1];

		buAddQuad (psbuParser, &sTempToken, &sTopStack, &sTempToken1, sQuad);

		//Fall through
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_ASSIGN;

		//Create temp variable
		buCreateTemp (psbuParser, &sTempToken2);

		sTempToken2.brValue = false;
		sTempToken2.level = psbuParser->psST->currentLevel;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken2, sQuad);

		sQuad.opCode = OP_BRA;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 4;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken1, sQuad);

		//True portion
		sQuad.opCode = OP_ASSIGN;
		sTempToken.level = 0;
		sTempToken.field = 1;

		buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken2, sQuad);

		psDummyToken->level = sTempToken2.level;
		psDummyToken->brValue = true;
		psDummyToken->field = sTempToken2.field;
	}
}

/****************************************************************************
Function:			buBoolop

Description:	Handles all boolops that comes across the BU

Parameters:		psbuParser 	 - the bu parser
							sPopToken	 	 - the token that was most recently popped
							sTopStack	 	 - the token on the top stack.
							psDummyToken - the dummy token that will get pushed
							index				 - the index of the stack

Returned:			None
*****************************************************************************/
void buBoolop (buParser *psbuParser, Token sPopToken, Token sTopStack,
							Token *psDummyToken , int index)
{
	Quad sQuad;
	Token sTempToken, sTempToken1, sTempToken2;
	int currentQuad = 0;

	currentQuad = psbuParser->psQuadFile->quadLevel;

	if (sPopToken.class == ANDOP || sPopToken.class == OROP)
	{
		if (sPopToken.value.BOOLOPType == AND)
		{
			sQuad.opCode = OP_BAND;
		}

		else if (sPopToken.value.BOOLOPType == OR)
		{
			sQuad.opCode = OP_BOR;
		}

		sQuad.level = psbuParser->psST->currentLevel;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 3;

		sTempToken = psbuParser->sBuStack [index + 1];

		buAddQuad (psbuParser, &sTempToken, &sTopStack, &sTempToken1, sQuad);

		//Fall through
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_ASSIGN;

		//Create temp variable
		buCreateTemp (psbuParser, &sTempToken2);

		sTempToken2.brValue = false;
		sTempToken2.level = psbuParser->psST->currentLevel;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken2, sQuad);

		sQuad.opCode = OP_BRA;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 4;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken1, sQuad);

		//True portion
		sQuad.opCode = OP_ASSIGN;
		sTempToken.level = 0;
		sTempToken.field = 1;

		buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken2, sQuad);

		psDummyToken->level = sTempToken2.level;
		psDummyToken->brValue = true;
		psDummyToken->field = sTempToken2.field;
	}
}

/****************************************************************************
Function:			buEquop

Description:	Handles all Equops that comes across the BU

Parameters:		psbuParser 	 - the bu parser
							sPopToken	 	 - the token that was most recently popped
							sTopStack	 	 - the token on the top stack.
							psDummyToken - the dummy token that will get pushed
							index				 - the index of the stack

Returned:			None
*****************************************************************************/
void buEquop (buParser *psbuParser, Token sPopToken, Token sTopStack,
							Token *psDummyToken , int index)
{
	Quad sQuad;
	Token sTempToken, sTempToken1, sTempToken2;
	int currentQuad = 0;

	currentQuad = psbuParser->psQuadFile->quadLevel;

	if (sPopToken.class == EQUOP)
	{
		if (sPopToken.value.equopType == EQUAL)
		{
			sQuad.opCode = OP_BEQ;
		}

		else if (sPopToken.value.equopType == NOT_EQUAL)
		{
			sQuad.opCode = OP_BNE;
		}

		sQuad.level = psbuParser->psST->currentLevel;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 3;

		sTempToken = psbuParser->sBuStack [index + 1];

		buAddQuad (psbuParser, &sTempToken, &sTopStack, &sTempToken1, sQuad);

		//Fall through
		sQuad.level = psbuParser->psST->currentLevel;
		sQuad.opCode = OP_ASSIGN;

		//Create temp variable
		buCreateTemp (psbuParser, &sTempToken2);

		sTempToken2.brValue = false;
		sTempToken2.level = psbuParser->psST->currentLevel;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken2, sQuad);

		sQuad.opCode = OP_BRA;

		sTempToken1.level = 0;
		sTempToken1.field = currentQuad + 4;

		buAddQuad (psbuParser, NULL, NULL, &sTempToken1, sQuad);

		//True portion
		sQuad.opCode = OP_ASSIGN;
		sTempToken.level = 0;
		sTempToken.field = 1;

		buAddQuad (psbuParser, &sTempToken, NULL, &sTempToken2, sQuad);

		psDummyToken->level = sTempToken2.level;
		psDummyToken->brValue = true;
		psDummyToken->field = sTempToken2.field;
	}
}

/****************************************************************************
Function:			buSetExp

Description:	Sets a flag to see if we are reading until a semicolon or
							Rparen

Parameters:		psbuParser 	 - the bu parser

Returned:			None
*****************************************************************************/
void buSetExp (buParser *psbuParser)
{
	if (psbuParser->bFullExp == false)
	{
		psbuParser->bFullExp = true;
	}

	else
	{
		psbuParser->bFullExp = false;
	}
}

/****************************************************************************
Function:			buInitExp

Description:	Sets a flag to see if we are reading until a semicolon or
							Rparen

Parameters:		psbuParser 	 - the bu parser

Returned:			None
*****************************************************************************/
void buSetInitExp (buParser *psbuParser)
{
	if (psbuParser->bInitExp == false)
	{
		psbuParser->bInitExp = true;
	}

	else
	{
		psbuParser->bInitExp = false;
	}
}
