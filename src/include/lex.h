/****************************************************************************
File name:	lex.h
Author:			Kevin Jo
Date:				2/23/15
Class:			CS480
Assignment:	Lexical Analyzer
Purpose:		Defines header file for lexical analysis module.
****************************************************************************/
#ifndef LEX_H_
#define LEX_H_

#include <stdio.h>
#include <stdlib.h>
#include "../include/pcc13defines.h"
#include "../include/er.h"
#include "../include/ct.h"
#include "../include/rts.h"
#include "../include/st.h"

#define MAX_LINE 1024
#define MAX_LEXEME 1024
#define NUM_RW 8

/****************************************************************************
 * Token Class Types
****************************************************************************/
typedef enum relopType
{
	LT_EQUAL = 1,
	GT_EQUAL = 2,
	LESS_THAN,
	GREATER_THAN
} relopType;

typedef enum addopType
{
	ADD_OP = 1,
	SUBTRACT_OP = 2
} addopType;

typedef enum mulopType
{
	MULTIPLY = 1,
	DIVIDE = 2,
	MOD
} mulopType;

typedef enum autoopType
{
	INCREMENT = 1,
	DECREMENT = 2
} autoopType;

typedef enum assignopType
{
	ASSIGN = 0
} assignopType;

typedef enum boolopType
{
	OR = 1,
	AND = 2
} boolopType;

typedef enum parenthesisType
{
	BEG_PARENTHESIS = 1,
	END_PARENTHESIS = 2
} parenthesisType;

typedef enum bracketType
{
	BEG_BRACKET = 1,
	END_BRACKET = 2
} bracketType;

typedef enum braceType
{
	BEG_BRACE = 1,
	END_BRACE = 2
} braceType;

/****************************************************************************
 * Bottom up Token Classes
****************************************************************************/
typedef enum tokenClassStates
{
	UNDEF = -1,
	ASSIGNOP = 0, // 1
	OROP = 1,
	ANDOP = 2,
	EQUOP,
	RELOP, // 5
	ADDOP,
	MULOP,
	AUTOOP,
	NEGOP,
	ADDRESSOP,
	LPAREN, // 10
	RPAREN,
	LBRACKET,
	RBRACKET,
	CONSTANT,
	IDENT, //15
	COMMA,
	DEREF,
	DOLLAR, // 18
	RES_WORD,
	IDENTIFIER, // 20
	BOOLOP,
	SEMICOLON,
	PARENTHESIS,
	BRACKET,
	BRACE,
	DUMMY = 99
} tokenClassStates;

typedef enum equOP
{
	EQUAL = 1,
	NOT_EQUAL = 2
} equOP;

typedef enum negOP
{
	NEG_OP = 1
} negOP;

typedef enum derefOP
{
	DEREFOP = 1
} derefOP;

typedef struct tokenValue
{
	ReservedWord RWType;
	long int identifier;
	int constant;
	relopType RELOPType;
	addopType ADDOPType;
	mulopType MULOPType;
	autoopType AUTOOPType;
	int assignop;
	int addressofop;
	boolopType BOOLOPType;
	int semicolon;
	int comma;
	parenthesisType PARENTHESISType;
	bracketType BRACKETType;
	braceType BRACEType;
	equOP equopType;
	negOP negopType;
	derefOP derefType;
	int dollar;
	int dummyValue;
} tokenValue;

typedef struct Token
{
	char lexeme [MAX_LEXEME];
	int lexemeIndex;
	tokenValue value;
	tokenClassStates class;
	int level, field;
	bool brValue, bPointer, bAutoop;
} Token;

typedef struct Lexer* LexerPtr;
typedef struct Lexer
{
	int lineNumber;
	int buffIndex;
	char buffer [MAX_LINE];
	bool bCommentOn, bDebug;
	FILE *pFile;
	ConstantTable *psConstantTable;
	RuntimeStack *psRTStack;
	SymbolTablePtr psSymbolTable;
	Token currentToken, prevToken;
} Lexer;


void lexCreate (Lexer *, FILE *, ConstantTable *, RuntimeStack *,
								SymbolTablePtr);
void lexToggle (Lexer *);
void lexDebug (Lexer *);
void lexPrintToken (Token);
Token lexGetToken (Lexer *, ErrorCode *, ErrorHandler *pError);
void lexDecrementBuffIndex (Lexer *);
void lexSetBuffIndex (Lexer *, int);
bool lexCheckEOF (Lexer *);

#endif
