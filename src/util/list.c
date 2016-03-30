/***********************************************************
File name:			list.c
Author:					Kevin Jo
Date:						2/11/15
Class: 					CS480
Assignment: 		Symbol Table
Purpose: 				This is the implementation file for each of the list functions
								associated with the list module.
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/list.h"

/***********************************************************
 Function:			lstCreate

 Description:		Creates a list with the list pointer that is passed in.

 Parameters: 		psList - a list pointer

 Returned:			ERROR_NO_LIST_CREATE if a null list pointer is passed in.
 	 	 	 	 				NO_ERROR if the list is successfully created.
***********************************************************/

ERRORCODE lstCreate (ListPtr psList)
{
	int errorCode;

	if (NULL == psList)
	{
		errorCode = ERROR_NO_LIST_CREATE;
	}

	else
	{
		psList->psHead = NULL;
		psList->psCurrent = NULL;
		psList->psLast = NULL;
		psList->numElements = 0;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:			lstDispose

 Description:		Disposes the list that the list pointer, that is passed in,
 	 	 	 	 	 	 	  is pointing to.

 Parameters: 		psList - a list pointer

 Returned:			ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	number of elements is negative.
 	 	 	 	 				NO_ERROR if the list is successfully disposed.
***********************************************************/

ERRORCODE lstDispose (ListPtr psList)
{
	int errorCode;
	ListElementPtr pTempElement;


	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}


	if (NULL != psList->psHead)
	{
		psList->psCurrent = psList->psLast;
		pTempElement = psList->psCurrent;

		while (NULL != pTempElement->psPrev)
		{
			psList->psCurrent = pTempElement->psPrev;
			free (pTempElement);
			psList->numElements--;
			pTempElement = psList->psCurrent;
		}

		free (pTempElement);
		psList->numElements--;
		psList->psCurrent = NULL;
		psList->psHead = NULL;
		psList->psLast = NULL;

		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:			lstSize

 Description:		Returns the size of the list through the function parameter.

 Parameters:		psList			- a list pointer
 	 	 	 	 	 	  	pListSize		- the variable that will take the size of the list

 Returned:			ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 		ERROR_NULL_PTR if a null integer pointer is passed in.
 	 	 	 	 				NO_ERROR if the size of the list is successfully returned.
***********************************************************/

ERRORCODE lstSize (ListPtr psList, int *pListSize)
{
	int errorCode;

	if ((NULL == psList) || (psList->numElements < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == pListSize)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else
	{
		*pListSize = psList->numElements;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:			lstIsFull

 Description:		Checks to see if list is full.

 Parameters: 		psList 	- a list pointer
 	 	 	 	 	 	  	bIsFull	- returns true or false through parameter.

 Returned:			ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 		ERROR_NULL_PTR if a null boolean pointer is passed in.
 	 	 	 	 				NO_ERROR if check full of the list is successfully returned.
***********************************************************/

ERRORCODE lstIsFull (ListPtr psList, bool *bIsFull)
{
	int errorCode;
	ListElementPtr psTempElement;
	psTempElement = (ListElementPtr) malloc (sizeof (ListElement));

	if ((NULL == psList) || (psList->numElements < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == bIsFull)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == psTempElement)
	{
		*bIsFull = true;
		errorCode = NO_ERROR;
	}

	else
	{
	*bIsFull = false;
	errorCode = NO_ERROR;
	}

	free (psTempElement);
	return errorCode;
}

/***********************************************************
 Function:		lstIsEmpty

 Description:	Checks to see if list is empty.

 Parameters:	psList		- a list pointer
 	 	 	 	 	 	  bIsEmpty	- returns true or false through parameter.

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 							number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null boolean pointer is passed in.
 	 	 	 	 			NO_ERROR if check empty of the list is successfully returned.
***********************************************************/

ERRORCODE lstIsEmpty (ListPtr psList, bool *bIsEmpty)
{
	int errorCode;

	if ((NULL == psList) || (psList->numElements < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == bIsEmpty)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == psList->psHead)
	{
		*bIsEmpty = true;
		errorCode = NO_ERROR;
	}

	else
	{
		*bIsEmpty = false;
		errorCode = NO_ERROR;
	}


	return errorCode;
}


/***********************************************************
 Function:		lstPeek

 Description:	Retrieves the current data through function parameter

 Parameters:  psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstPeek (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{

		*data = psList->psCurrent->data;
		errorCode = NO_ERROR;
	}

	return errorCode;
}



/***********************************************************
 Function:		lstPeekPrev

 Description:	Retrieves the previous data through function parameter

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element
 	 	 	 	 	 	 	ERROR_NO_PREV if psPrev is not pointing to a list element
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstPeekPrev (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else if (NULL == (psList->psCurrent->psPrev))
	{
		errorCode = ERROR_NO_PREV;
	}

	else if ((psList->psHead != psList->psCurrent) && (psList->numElements >= 2))
	{
		*data = psList->psCurrent->psPrev->data;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstPeekNext

 Description:	Retrieves the next data through function parameter

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element
 	 	 	 	 	 	 	ERROR_NO_NEXT if psNext is not pointing to a list element
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstPeekNext (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else if (NULL == (psList->psCurrent->psNext))
	{
		errorCode = ERROR_NO_NEXT;
	}

	else if ((psList->psLast != psList->psCurrent) && (psList->numElements >= 2))
	{
		*data = psList->psCurrent->psNext->data;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstFirst

 Description:	Retrieves the first data in the list and sets current to it.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstFirst (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else
	{
		*data = psList->psHead->data;
		psList->psCurrent = psList->psHead;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstLast

 Description:	Retrieves the last data in the list and sets current to it.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	 	data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  					is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstLast (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else
	{

		if (NULL != (psList->psLast))
		{
			*data = psList->psLast->data;
			psList->psCurrent = psList->psLast;
		}

		errorCode = NO_ERROR;
	}

	return errorCode;
}

/**********************************************************
 Function:			lstNext

 Description:		Retrieves the current data in the list and sets current to the
 	 	 	 	 	 	 		successor of the current element.

 Parameters: 		psList 	- a list pointer
 	 	 	 	 	 	  	data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstNext (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		*data = psList->psCurrent->data;
		psList->psCurrent = psList->psCurrent->psNext;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/**********************************************************
 Function:		lstPrev

 Description:	Retrieves the current data in the list and sets current to the
 	 	 	 	 	 	 	previous of the current element.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstPrev (ListPtr psList, DATATYPE *data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		*data = psList->psCurrent->data;
		psList->psCurrent = psList->psCurrent->psPrev;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstDeleteCurrent

 Description:	Deletes the current element. Makes its successor and
 	 	 	 	 	 	  predecessor become successor and predecessor of each other. Sets
 	 	 	 	 	 	  current to the predecessor of deleted current element. If there
 	 	 	 	 	 	  is no predecessor, the first element becomes new current element.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  					is pointing to

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 						number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstDeleteCurrent (ListPtr psList, DATATYPE *data)
{
	int errorCode;
	ListElementPtr psTempElement;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == data)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		*data = psList->psCurrent->data;

		if ((NULL == psList->psCurrent->psPrev) &&
				(NULL == psList->psCurrent->psNext))
		{
			free (psList->psCurrent->data.htData.key); //Added for ST delete level
			free (psList->psCurrent);
			psList->psCurrent = NULL;
			psList->psHead = NULL;
			psList->psLast = NULL;
		}

		//If psCurrent is psHead
		else if (NULL == psList->psCurrent->psPrev)
		{
			psTempElement = psList->psCurrent;
			psList->psCurrent->psNext->psPrev = psList->psCurrent->psPrev;
			psList->psCurrent = psTempElement->psNext;
			psList->psHead = psList->psCurrent;
			free (psTempElement->data.htData.key); //Added for ST delete level
			free (psTempElement);
		}

		//If psCurrent is psLast
		else if (NULL == psList->psCurrent->psNext)
		{
			psTempElement = psList->psCurrent;
			psList->psCurrent->psPrev->psNext = psList->psCurrent->psNext;
			psList->psCurrent = psList->psCurrent->psPrev;
			psList->psLast = psTempElement->psPrev;
			free (psTempElement->data.htData.key); //Added for ST delete level
			free (psTempElement);
		}

		//If psCurrent is between psHead and psLast
		else
		{
			psTempElement = psList->psCurrent;
			psTempElement->psNext->psPrev = psList->psCurrent->psPrev;
			psTempElement->psPrev->psNext = psList->psCurrent->psNext;
			psList->psCurrent = psTempElement->psPrev;
			free (psTempElement->data.htData.key); //Added for ST delete level
			free (psTempElement);
		}

		psList->numElements--;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstInsertBefore

 Description:	Will insert data provided before the current element.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	 	 data		- the data that will be inserted

 Returned:		ERROR_FULL_LIST if the list is full.
 	 	 	 	 	 		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 		ERROR_NO_CURRENT if the psCurrent pointer is not pointing to
 	 	 	 	 	 	 									 	 	 	 	 	 	 a list element.
 	 	 	 	 			NO_ERROR if the data is successfully inserted.
***********************************************************/

ERRORCODE lstInsertBefore (ListPtr psList, DATATYPE data)
{
	int errorCode;
	ListElementPtr psTempElement;
	psTempElement = (ListElementPtr) malloc (sizeof (ListElement));
	psTempElement->data = data;

	if (NULL == psTempElement)
	{
		errorCode = ERROR_FULL_LIST;
	}

	else if ((NULL == psList) || ((psList->numElements) < 0))
	{
		free (psTempElement);
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == (psList->psCurrent) && ((psList->numElements) > 0))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		//If the list is not empty, and there is only one element on the list or
		//trying to insert before first element.
		if ((NULL != psList->psCurrent) && (NULL == psList->psCurrent->psPrev))
		{
			psList->psCurrent->psPrev = psTempElement;
			psTempElement->psPrev = NULL;
			psTempElement->psNext = psList->psCurrent;
			psList->psCurrent = psTempElement;
			psList->psHead = psList->psHead->psPrev;
		}

		//If the list is empty.
		else if ((NULL == psList->psCurrent) && (NULL == psList->psHead) &&
						(NULL == psList->psLast))
		{
			psTempElement->psNext = NULL;
			psTempElement->psPrev = NULL;
			psList->psCurrent = psTempElement;
			psList->psHead = psTempElement;
			psList->psLast = psTempElement;
		}

		//If there are 2 or more elements on the list.
		else if (psList->numElements >= 2)
		{
			psList->psCurrent->psPrev->psNext = psTempElement;
			psTempElement->psPrev = psList->psCurrent->psPrev;
			psTempElement->psNext = psList->psCurrent;
			psList->psCurrent->psPrev = psTempElement;
			psList->psCurrent = psTempElement;
		}

		psList->numElements++;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/**********************************************************
 Function:			lstInsertAfter

 Description:		Inserts a data after the current element, and makes new data
 	 	 	 	 	 	  	inserted the new current element.

 Parameters: 		psList 	- a list pointer
 	 	 	 	 	 	  	data		- the data pointer that will take the data psCurrent
 	 	 	 	 	 	  								is pointing to

 Returned:			ERROR_FULL_LIST if the list is full.
 	 	 	 	 	 	 		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 		ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 				NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstInsertAfter (ListPtr psList, DATATYPE data)
{
	int errorCode;
	ListElementPtr psTempElement;
	psTempElement = (ListElementPtr) malloc (sizeof (ListElement));
	psTempElement->data = data;

	if (NULL == psTempElement)
	{
		errorCode = ERROR_FULL_LIST;
	}

	else if ((NULL == psList) || ((psList->numElements) < 0))
	{
		free (psTempElement);
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == (psList->psCurrent) && ((psList->numElements) > 0))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		//If the list is not empty, and there is only one element on the list or
		//trying to insert after last element
		if ((NULL != psList->psCurrent) &&
				(NULL == psList->psCurrent->psNext))
		{
			psList->psCurrent->psNext = psTempElement;
			psTempElement->psNext = NULL;
			psTempElement->psPrev = psList->psCurrent;
			psList->psCurrent = psTempElement;
			psList->psLast = psTempElement;
		}

		//If the list is empty.
		else if ((NULL == psList->psCurrent) && (NULL == psList->psHead) &&
						(NULL == psList->psLast))
		{
			psTempElement->psNext = NULL;
			psTempElement->psPrev = NULL;
			psList->psCurrent = psTempElement;
			psList->psHead = psTempElement;
			psList->psLast = psTempElement;
		}

		//If psCurrent is psLast.
		else if (psList->numElements >= 2)
		{
			psList->psCurrent->psNext->psPrev = psTempElement;
			psTempElement->psNext = psList->psCurrent->psNext;
			psTempElement->psPrev = psList->psCurrent;
			psList->psCurrent->psNext = psTempElement;
			psList->psCurrent = psTempElement;
		}

		psList->numElements++;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstUpdateCurrent

 Description:	Updates the current element to the data type provided in
 	 	 	 	 	 		argument.

 Parameters: 	psList 	- a list pointer
 	 	 	 	 	 	  data		- the data that current will be updated to

 Returned:		ERROR_FULL_LIST if the list is full.
 	 	 	 	 	 	 	ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstUpdateCurrent (ListPtr psList, DATATYPE data)
{
	int errorCode;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == (psList->psHead))
	{
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent) && ((psList->numElements) > 0))
	{
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		psList->psCurrent->data = data;
		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:		lstHasNext

 Description:	Checks to see if there is an element when traversing forward.

 Parameters: 	psList 	 	- a list pointer
 	 	 	 	 	 	  bHasNext	- used to see if there is a next element after current

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
************************************************************/

ERRORCODE lstHasNext (ListPtr psList, bool *bHasNext)
{
	int errorCode;
	*bHasNext = true;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == bHasNext)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		*bHasNext = false;
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent) && ((psList->numElements) > 0))
	{
		*bHasNext = false;
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		if (NULL == psList->psCurrent->psNext)
		{
			*bHasNext = false;
		}

		errorCode = NO_ERROR;
	}

	return errorCode;
}

/**********************************************************
 Function:			lstHasPrev

 Description:		Checks to see if there is an element when traversing backward.

 Parameters: 		psList 	 	- a list pointer
 	 	 	 	 	 	  	bHasPrev	- Used to see if there is an element when traversing
 	 	 	 	 	 	  					 		backwards.

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_NULL_PTR if a null data pointer is passed in.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element.
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/

ERRORCODE lstHasPrev (ListPtr psList, bool *bHasPrev)
{
	int errorCode;
	*bHasPrev = true;

	if ((NULL == psList) || ((psList->numElements) < 0))
	{
		errorCode = ERROR_INVALID_LIST;
	}

	else if (NULL == bHasPrev)
	{
		errorCode = ERROR_NULL_PTR;
	}

	else if (NULL == (psList->psHead))
	{
		*bHasPrev = false;
		errorCode = ERROR_EMPTY_LIST;
	}

	else if (NULL == (psList->psCurrent) && ((psList->numElements) > 0))
	{
		*bHasPrev = false;
		errorCode = ERROR_NO_CURRENT;
	}

	else
	{
		if (NULL == psList->psCurrent->psPrev)
		{
			*bHasPrev = false;
		}

		errorCode = NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
 Function:			lstPeekPointer

 Description:		Retrieves the pointer to the current data through function
 	 	 	 	 	 	 	 	parameter

 Parameters:  	psList 	- a list pointer
 	 	 	 	 	 	  	hsData	- the data handle that will return the pointer to
 	 	 	 	 	 	  						the data

 Returned:		ERROR_INVALID_LIST if a null list pointer is passed in or the
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 number of elements is negative.
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty.
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is not pointing to a list element
 	 	 	 	 			NO_ERROR if the data is successfully retrieved.
***********************************************************/
ERRORCODE lstPeekPointer (ListPtr psList, DATATYPE **hsData)
{
	int errorCode;

		if ((NULL == psList) || ((psList->numElements) < 0))
		{
			errorCode = ERROR_INVALID_LIST;
		}

		else if (NULL == (psList->psHead))
		{
			errorCode = ERROR_EMPTY_LIST;
		}

		else if (NULL == (psList->psCurrent))
		{
			errorCode = ERROR_NO_CURRENT;
		}

		else
		{
			(*hsData) = &(psList->psCurrent->data);
			errorCode = NO_ERROR;
		}

		return errorCode;
}
