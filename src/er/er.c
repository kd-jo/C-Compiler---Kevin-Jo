/****************************************************************************
File name:	er.c
Author:			Kevin Jo
Date:				2/11/15
Class:			CS480
Assignment:	Symbol Table
Purpose:		This is the implementation file for each of the error handler
						functions associated with the error handler module.
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pcc13defines.h"
#include "../include/er.h"

char gszErrors [ERROR_TBL_SIZE][MAX_ERROR_LINE];

/****************************************************************************
Function:			erCreateErrorHandler

Description:	Creates the error handler.

Parameters:		psErrorHandler - the error handler

Returned: 		None
****************************************************************************/
void erCreateErrorHandler (ErrorHandlerPtr psErrorHandler)
{
	LOAD_ERRORS

	psErrorHandler->psErrorList = (ListPtr) malloc (sizeof (List));
	lstCreate (psErrorHandler->psErrorList);
	psErrorHandler->numErrors = 0;
	psErrorHandler->lineNumber = 0;
}

/****************************************************************************
Function:			erDisposeErrorHandler

Description:	Disposes the error handler.

Parameters:		psErrorHandler - the error handler

Returned:			None
****************************************************************************/
void erDisposeErrorHandler (ErrorHandlerPtr psErrorHandler)
{
	lstDispose (psErrorHandler->psErrorList);
	free (psErrorHandler->psErrorList);
}

/****************************************************************************
Function:			erAddError

Description:	Adds an error to the error handler.

Parameters:		psErrorHandler 	- the error handler
							stError					- the error
							optionalContent - any optional content such as filename,
																identifier

Returned:			None
****************************************************************************/
void erAddError (ErrorHandlerPtr psErrorHandler, ErrorCode stError,
												char *optionalContent)
{
	DATATYPE sData;

	if (NO_ERROR != stError)
	{
		sData.erData.errorMessage = gszErrors [stError];
		sData.erData.lineNumber = psErrorHandler->lineNumber;
		sData.erData.optionalContent = optionalContent;
		sData.erData.errorCode = stError;

		lstInsertAfter (psErrorHandler->psErrorList, sData);
	}
}

/****************************************************************************
Function:			erClear

Description:	Clears the errors in the error handler, but does not dispose.

Parameters:		psErrorHandler - the error handler

Returned:			None
****************************************************************************/
void erClear (ErrorHandlerPtr psErrorHandler)
{
	lstDispose (psErrorHandler->psErrorList);
}

/****************************************************************************
Function:			erIncrementLine

Description:	Increments the line number to keep track of file line number.

Parameters:		psErrorHandler - the error handler

Returned:			None
****************************************************************************/
void erIncrementLine (ErrorHandlerPtr psErrorHandler)
{
	psErrorHandler->lineNumber++;
}

/****************************************************************************
Function:			erGetCurrentEntry

Description:	Returns the current error entry through arguments.

Parameters:		psErrorHandler	- the error handler
							psERData				- the error datatype to be returned through

Returned:			None
****************************************************************************/
void erGetCurrentEntry (ErrorHandlerPtr psErrorHandler,
																		ER_DATATYPE *psERData)
{
	DATATYPE sData;

	lstPeek (psErrorHandler->psErrorList, &sData);

	if (NO_ERROR != sData.erData.errorCode)
	{
		(*psERData) = sData.erData;
	}
}

/****************************************************************************
Function:			erPrintEntry

Description:	Prints the current error entry with line number and optional
							content.

Parameters:		psErrorHandler 	- the error handler
							stError					- the error
							optionalContent - any optional content such as filename,
																identifier

Returned:			None
****************************************************************************/
void erPrintEntry (ErrorHandlerPtr psErrorHandler, ErrorCode stError,
													char *optionalContent)
{
	ER_DATATYPE sData;

	if (NO_ERROR != stError)
	{
		if (OUT_OF_MEMORY != stError)
		{
			erGetCurrentEntry (psErrorHandler, &sData);
			printf ("Line %d: %s %s.\n\n", sData.lineNumber, sData.errorMessage,
								sData.optionalContent);
		}

		else
		{
			erGetCurrentEntry (psErrorHandler, &sData);
			printf ("Line %d: %s.\n\n", sData.lineNumber, sData.errorMessage);
		}

	}
}

/****************************************************************************
Function:			erPrintAll

Description:	Prints all the errors in the error handler with line number and
							optional content..

Parameters:		psErrorHandler 	- the error handler

Returned:			None
****************************************************************************/
void erPrintAll (ErrorHandlerPtr psErrorHandler)
{
	DATATYPE sListData;
	ER_DATATYPE sERData, sPrevData;
	//int index, size, tempSize = 0;
	int size, tempSize = 0;

	sPrevData.lineNumber = -1;
	lstSize (psErrorHandler->psErrorList, &size);
	lstFirst (psErrorHandler->psErrorList, &sListData);

	while (tempSize < size)
	{
		erGetCurrentEntry (psErrorHandler, &sERData);

		if (sPrevData.lineNumber != sERData.lineNumber)
		{
			printf ("Line %d: %s %s\n\n", sERData.lineNumber, sERData.errorMessage,
								sERData.optionalContent);
		}

		lstNext (psErrorHandler->psErrorList, &sListData);
		sPrevData = sERData;

		tempSize++;
	}
}
