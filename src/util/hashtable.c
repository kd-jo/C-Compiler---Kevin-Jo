/***********************************************************
File name:	hashtable.c
Author:			Kevin Jo
Date:				2/11/15
Class: 			CS480
Assignment:	Symbol Table
Purpose: 		This is the implementation file for each of the hash table
						function associated with the hash table module.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/hashtable.h"
#include "../include/list.h"

/*****************************************************************************
Function:			htCreate

Description:	Creates a hash table given the hash function, compare function,
							and size.

Parameters:		psHashTable - the hash table
							htFunction	- the hash function
							htCompare		- the compare function
							size				- the size of the hash table

Returned:			HT_ERROR_NO_CREATE if the hashtable cannot be created
							otherwise HT_NO_ERROR
*****************************************************************************/
HT_ERRORCODE htCreate (HashTablePtr psHashTable, hashFunction htFunction,
											compareFunction htCompare, int size)
{
	HT_ERRORCODE errorCode = HT_NO_ERROR;
	int i;

	psHashTable->psListArray = (ListPtr) malloc (sizeof (List) * (size + 1));

	for (i = 0; i < size; i++)
	{
		if (ERROR_NO_LIST_CREATE == lstCreate (psHashTable->psListArray + i))
		{
			errorCode = HT_ERROR_NO_HT_CREATE;
		}
	}

	if (HT_ERROR_NO_HT_CREATE == errorCode)
	{
		free (psHashTable->psListArray);
	}

	else
	{
		psHashTable->htFunction = htFunction;
		psHashTable->htCompare = htCompare;
		psHashTable->size = size;
		psHashTable->htStats.averageProbes = 0.0;
		psHashTable->htStats.emptyBuckets = size;
		psHashTable->htStats.longestChain = 0;
		psHashTable->htStats.numElements = 0;

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htDispose

Description:	Disposes a hash table given the size

Parameters:		psHashTable 	- the hash table
							size					- the size of the hash table

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htDispose (HashTablePtr psHashTable, int size)
{
	HT_ERRORCODE errorCode;
	DATATYPE data;
	bool bIsEmpty;
	int i, j, listSize;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else
	{
		for (i = 0; i < size; i++)
		{
			lstIsEmpty (&psHashTable->psListArray [i], &bIsEmpty);

			if (false == bIsEmpty)
			{
				lstSize (&psHashTable->psListArray [i], &listSize);
				lstFirst (&psHashTable->psListArray [i], &data);

				for (j = 0; j < listSize; j++)
				{
					lstPeek (&psHashTable->psListArray [i], &data);
					free (data.htData.key);
					lstNext (&psHashTable->psListArray [i], &data);
				}

				lstDispose (&psHashTable->psListArray [i]);
			}
		}

		free (psHashTable->psListArray);
		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htInsertKey

Description:	Inserts a key into the hash table.

Parameters:		psHashTable 	- the hash table
							htData					- the data to insert

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htInsertKey (HashTablePtr psHashTable, HT_DATATYPE htData)
{
	HT_ERRORCODE errorCode;
	bool bListEmpty = false, bHasNext, bHasPrev, bExists = false;
	int bucketAddress, size, i = 0, keySize = 0;
	DATATYPE data, insertData;
	char *tempString;
	memset (&insertData, 0, sizeof (DATATYPE));

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else
	{
		keySize = strlen (htData.key);

		insertData.htData.key = (char *) malloc (sizeof (char) * (keySize + 1));
		insertData.htData.stData.idType = htData.stData.idType;
		insertData.htData.stData.idAddressField = htData.stData.idAddressField;
		insertData.htData.stData.level = htData.stData.level;
		insertData.htData.stData.arrayDim = htData.stData.arrayDim;
		insertData.htData.stData.paramCount = htData.stData.paramCount;
		insertData.htData.ctData.value = htData.ctData.value;
		insertData.htData.ctData.stackAddress = htData.ctData.stackAddress;
		tempString = htData.key;
		strcpy (insertData.htData.key, tempString);
		psHashTable->htFunction (insertData.htData.key, &bucketAddress, psHashTable->size);
		lstIsEmpty (&(psHashTable->psListArray [bucketAddress]), &bListEmpty);

		if (true == bListEmpty)
		{
			insertData.htData.count = 1;
			lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
			psHashTable->htStats.numElements++;
			psHashTable->htStats.emptyBuckets--;
			psHashTable->htStats.averageProbes++;

			if (psHashTable->htStats.numElements == 1)
			{
				psHashTable->htStats.longestChain = 1;
			}

			errorCode = HT_NO_ERROR;
		}

		else
		{
			//Go through list to check if word already exists.
			lstFirst (&(psHashTable->psListArray [bucketAddress]), &data);
			lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);

			if (0 == strcmp (data.htData.key, insertData.htData.key))
			{
				if (data.htData.stData.level == insertData.htData.stData.level)
				{
					free (insertData.htData.key);
					bExists = true;
				}

				/*
				while (data.htData.stData.level < insertData.htData.stData.level)
				{
					lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
					lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);
				}

				if (data.htData.stData.level != insertData.htData.stData.level)
				{
					lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
					psHashTable->htStats.numElements++;
					bExists = true;
				}
				*/


				else
				{
					lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);

					if (false == bHasNext)
					{
						lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
						psHashTable->htStats.numElements++;
						bExists = true;
					}

					else
					{
						while (true == bHasNext && (data.htData.stData.level < insertData.htData.stData.level) && (0 == strcmp (data.htData.key, insertData.htData.key)))
						{
							lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
							lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);
							lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);
						}

						lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
						psHashTable->htStats.numElements++;
						bExists = true;
					}
				}
			}

			lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);

			while (true == bHasNext && false ==bExists)
			{
				lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
				lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);

				if (0 == strcmp (data.htData.key, insertData.htData.key))
				{
					if (data.htData.stData.level == insertData.htData.stData.level)
					{
						free (insertData.htData.key);
						bExists = true;
					}

					else
					{
						while (data.htData.stData.level < insertData.htData.stData.level)
						{
							lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
						}

						lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
						psHashTable->htStats.numElements++;
						//free (insertData.htData.key);
						bExists = true;
					}
				}

				lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);
			}

			//Go through list and insert if word does not exist.
			if (false == bExists)
			{
				lstLast (&(psHashTable->psListArray [bucketAddress]), &data);
				lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);

				lstHasPrev (&(psHashTable->psListArray [bucketAddress]), &bHasPrev);

				while ((data.htData.key [i] > insertData.htData.key [i]) && (true == bHasPrev))
				{
					lstPrev (&(psHashTable->psListArray [bucketAddress]), &data);
					lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);
					lstHasPrev (&(psHashTable->psListArray [bucketAddress]), &bHasPrev);
				}

				if (data.htData.key [i] < insertData.htData.key [i])
				{
					insertData.htData.count = 1;
					lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
					lstSize (&psHashTable->psListArray [bucketAddress], &size);
					psHashTable->htStats.numElements++;
					psHashTable->htStats.averageProbes += size;
				}

				else if (data.htData.key [i] > insertData.htData.key [i])
				{
					insertData.htData.count = 1;
					lstInsertBefore (&(psHashTable->psListArray [bucketAddress]), insertData);
					lstSize (&psHashTable->psListArray [bucketAddress], &size);
					psHashTable->htStats.averageProbes += size;
					psHashTable->htStats.numElements++;
				}

				//If first letter of an existing word is the same as the first letter
				//of inserting word.
				else
				{
					i++;

					while (data.htData.key [i] == insertData.htData.key [i])
					{
						i++;
					}

					if (data.htData.key [i] < insertData.htData.key [i])
					{
						insertData.htData.count = 1;
						lstInsertAfter (&(psHashTable->psListArray [bucketAddress]), insertData);
						lstSize (&psHashTable->psListArray [bucketAddress], &size);
						psHashTable->htStats.averageProbes += size;
						psHashTable->htStats.numElements++;
					}

					else if (data.htData.key [i] > insertData.htData.key [i])
					{
						insertData.htData.count = 1;
						lstInsertBefore (&(psHashTable->psListArray [bucketAddress]), insertData);
						lstSize (&psHashTable->psListArray [bucketAddress], &size);
						psHashTable->htStats.averageProbes += size;
						psHashTable->htStats.numElements++;
					}
				}
			}

			//Check size of list for longest chain.
			lstSize (&(psHashTable->psListArray [bucketAddress]), &size);

			if (size > psHashTable->htStats.longestChain)
			{
				psHashTable->htStats.longestChain = size;
			}

			if (true == bExists &&
					(data.htData.stData.level == insertData.htData.stData.level))
			{
				errorCode = HT_DUPLICATE;
			}

			else
			{
				errorCode = HT_NO_ERROR;
			}
		}
	}

	return errorCode;
}

/***********************************************************
Function:			htDeleteKey

Description:	Deletes a key into the hash table, given the key and returns the
							deleted entry through the argument list.

Parameters:		psHashTable 	- the hash table
							key							- the key to delete
							psResult				- the data to be returned through
							size							- the size of the hash table

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
							HT_ERROR_NULL_PTR if user does not pass in a key
							HT_ERROR_DATA_NOT_FOUND if the data does not exist in the HT
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htDeleteKey (HashTablePtr psHashTable, char *key,
																					HT_DATATYPE *psResult, int size)
{
	HT_ERRORCODE errorCode;
	int bucketAddress;
	DATATYPE data;
	bool bHasNext, bIsEmpty;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (NULL == key)
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	else
	{
		psHashTable->htFunction (key, &bucketAddress, psHashTable->size);

		lstFirst (&psHashTable->psListArray [bucketAddress], &data);
		lstPeek (&psHashTable->psListArray [bucketAddress], &data);
		lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);

		while ((data.htData.key != key) && (true == bHasNext))
		{
			lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
			lstPeek (&psHashTable->psListArray [bucketAddress], &data);
			lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);
		}

		if (data.htData.key == key)
		{
			if (NO_ERROR == lstDeleteCurrent
					(&psHashTable->psListArray [bucketAddress], &data))
			{

				psHashTable->htStats.numElements--;
				lstIsEmpty (&psHashTable->psListArray [bucketAddress], &bIsEmpty);

				if (true == bIsEmpty)
				{
					psHashTable->htStats.emptyBuckets++;
				}

				htUpdateLongest (psHashTable, size);

				*psResult = data.htData;
				errorCode = HT_NO_ERROR;
			}
		}

		else
		{
			errorCode = HT_ERROR_DATA_NOT_FOUND;
		}
	}

	return errorCode;
}

/***********************************************************
Function:			htUpdateKey

Description:	Updates the data in the hashtable with the key passed in.

Parameters:		psHashTable 	- the hash table
							key							- the key that will replace the existing entry

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
							HT_ERROR_NULL_PTR if user does not pass in a key
							HT_ERROR_DATA_NOT_FOUND if the data does not exist in the HT
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htUpdateKey (HashTablePtr psHashTable, char *key)
{
	HT_ERRORCODE errorCode;
	DATATYPE data;
	bool bHasNext, bFound = false;
	int bucketAddress;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (NULL == key)
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	else
	{
		psHashTable->htFunction (key, &bucketAddress, psHashTable->size);

		lstFirst (&(psHashTable->psListArray [bucketAddress]), &data);
		lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);

		while (true == bHasNext)
		{
			lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);

			if (key == data.htData.key)
			{
				data.htData.count++;
				lstUpdateCurrent (&(psHashTable->psListArray [bucketAddress]), data);
				bFound = true;
			}

			lstNext (&(psHashTable->psListArray [bucketAddress]), &data);
			lstHasNext (&(psHashTable->psListArray [bucketAddress]), &bHasNext);
		}

		lstPeek (&(psHashTable->psListArray [bucketAddress]), &data);

		if (0 == strcmp (data.htData.key, key))
		{
			data.htData.count++;
			lstUpdateCurrent (&(psHashTable->psListArray [bucketAddress]), data);
			bFound = true;
		}

		if (false == bFound)
		{
			errorCode = HT_ERROR_DATA_NOT_FOUND;
		}

		else
		{
			errorCode = HT_NO_ERROR;
		}
	}

	return errorCode;
}

/***********************************************************
Function:			htFindKey

Description:		Finds a key in the hashtable and returns the datatype through
									arguments.

Parameters:	psHashTable 	- the hash table
									key							- the key to be found
									psResult				- the datatype to be returned through

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
									HT_ERROR_NULL_PTR if user does not pass in a key
									HT_ERROR_DATA_NOT_FOUND if the data does not exist in the HT
									otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htFindKey (HashTablePtr psHashTable, char *key,
																			HT_DATATYPE *psResult)
{
	HT_ERRORCODE errorCode;
	int bucketAddress;
	DATATYPE data;
	bool bHasPrev;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (NULL == key)
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	data.htData.key = '\0';

	psHashTable->htFunction (key, &bucketAddress, psHashTable->size);

	lstLast (&psHashTable->psListArray [bucketAddress], &data);
	lstPeek (&psHashTable->psListArray [bucketAddress], &data);
	lstHasPrev (&(psHashTable->psListArray [bucketAddress]), &bHasPrev);

	if (NULL != data.htData.key)
	{
		while ((0 != strcmp (data.htData.key, key)) && (true == bHasPrev))
		{
			lstPrev (&(psHashTable->psListArray [bucketAddress]), &data);
			lstPeek (&psHashTable->psListArray [bucketAddress], &data);
			lstHasPrev (&(psHashTable->psListArray [bucketAddress]), &bHasPrev);
		}

		if (0 == strcmp (key, data.htData.key))
		{
			*psResult = data.htData;
			errorCode = HT_NO_ERROR;
		}

		else
		{
			errorCode = HT_ERROR_DATA_NOT_FOUND;
		}
	}

	else
	{
		errorCode = HT_ERROR_DATA_NOT_FOUND;
	}



	return errorCode;
}

/***********************************************************
Function:			htGetStats

Description:		Gets the stats of the hashtable

Parameters:	psHashTable 	- the hash table
									psStats				- the stats struct that will return the statistics of the
																			HT

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
									HT_ERROR_NULL_PTR if user does not pass in a key
									otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htGetStats (HashTablePtr psHashTable,
																				HT_STATISTICS *psStats)
{
	HT_ERRORCODE errorCode;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (NULL == psStats)
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	else
	{
		psHashTable->htStats.averageProbes =
				(psHashTable->htStats.averageProbes /
						psHashTable->htStats.numElements);
		*psStats = psHashTable->htStats;

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htDecrementNumElements

Description:		Decrements the number of elements stat in the HT.

Parameters:	psHashTable 	- the hash table

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
									otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htDecrementNumElements (HashTablePtr psHashTable)
{
	HT_ERRORCODE errorCode;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else
	{
		psHashTable->htStats.numElements--;

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htUpdateLongest

Description:		Updates the longest chain stat in the HT.

Parameters:	psHashTable 	- the hash table
									size							- the size of the HT

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
									HT_ERROR_EMPTY_HT if the hashtable is empty
									otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htUpdateLongest (HashTablePtr psHashTable, int size)
{
	HT_ERRORCODE errorCode;
	int i, listSize, longestChain = 0;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (psHashTable->htStats.emptyBuckets == size)
	{
		errorCode = HT_ERROR_EMPTY_HT;
	}

	else
	{
		for (i = 0; i < size; i++)
		{
			lstSize (&psHashTable->psListArray [i], &listSize);

			if (listSize > longestChain)
			{
				longestChain = listSize;
			}
		}

		psHashTable->htStats.longestChain = longestChain;

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htSort

Description:	Sorts the hashtable using the compare function and puts the sorted
							values into an array. The array is returned through the arguments.

Parameters:		psHashTable 	- the hash table
							htArray				- the array that will return the sorted values
							size					- the size of the HT

Returned:			HT_ERROR_INVALID_HT if the hashtable doesn't exist
							HT_ERROR_NULL_PTR if the array doesn't exist
							HT_ERROR_EMPTY_HT if the hashtable is empty
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htSort (HashTablePtr psHashTable, HT_DATATYPE **hsArray,
																int size)
{
	HT_ERRORCODE errorCode;
	DATATYPE listData;
	int i, j = 0;
	bool bIsEmpty, bHasNext = false;

	if (NULL == psHashTable || NULL == psHashTable->psListArray)
	{
		errorCode = HT_ERROR_INVALID_HT;
	}

	else if (NULL == hsArray)
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	else if (0 == psHashTable->htStats.numElements)
	{
		errorCode = HT_ERROR_EMPTY_HT;
	}

	else
	{
		//Insert all data in hashtable to a list array.
		for (i = 0; i < size; i++)
		{
			lstIsEmpty (&psHashTable->psListArray [i], &bIsEmpty);

			if (false == bIsEmpty)
			{
				lstFirst (&psHashTable->psListArray [i], &listData);
				lstHasNext (&psHashTable->psListArray [i], &bHasNext);

				while (true == bHasNext)
				{
					lstPeek (&psHashTable->psListArray [i], &listData);
					(*hsArray) [j] = listData.htData;
					lstNext (&psHashTable->psListArray [i], &listData);
					lstHasNext (&psHashTable->psListArray [i], &bHasNext);
					j++;
				}

				lstPeek (&psHashTable->psListArray [i], &listData);
				(*hsArray) [j] = listData.htData;
				j++;
			}
		}

		qsort ((void *) (*hsArray), j, sizeof (HT_DATATYPE), psHashTable->htCompare);

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htPrintHead

Description:	Prints the header

Parameters:		psHashTable 	- the hash table

Returned:			HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htPrintHead (HashTablePtr psHashTable)
{
	HT_ERRORCODE errorCode;
	int i;

	for (i = 0; i < 65; i++)
	{
		printf ("*");
	}

	printf ("\n");
	printf ("*                   ");
	printf ("WORD FREQUENCY RESULTS");
	printf ("                      *\n");

	for (i = 0; i < 65; i++)
	{
		printf ("*");
	}

	printf ("\n\n");
	printf ("%30s", "WORD");
	printf ("%11s", "COUNT\n");
	printf ("%30s", "----");
	printf ("%11s", "-----\n");

	errorCode = HT_NO_ERROR;

	return errorCode;
}

/***********************************************************
Function:			htCRCFunction

Description:	The hash function taken from:
http://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10
/hashfuncs.html

Parameters:		key 						- the key
							bucketAddress 	- the bucket address that will be returned through
							size						- the size of the hash table.

Returned:			HT_ERROR_NULL_PTR if key or bucket address is not existent
							otherwise HT_NO_ERROR
***********************************************************/
HT_ERRORCODE htCRCFunction (char key [], int *bucketAddress, int size)
{
	HT_ERRORCODE errorCode;
	unsigned int bucket = 0, highOrder;
	int index = 0;

	if ((NULL == key) || (NULL == bucketAddress))
	{
		errorCode = HT_ERROR_NULL_PTR;
	}

	else
	{
		while ('\0' != key [index])
		{
			highOrder = bucket & 0xf8000000;
			bucket = bucket << 5;
			bucket = bucket ^ (highOrder >> 27);
			bucket = bucket ^ (unsigned int) (key [index]);
			index++;
		}

		*bucketAddress = (bucket % size);

		errorCode = HT_NO_ERROR;
	}

	return errorCode;
}

/***********************************************************
Function:			htCompareAlpha

Description:	Compares the alphabet of two datatype keys.

Parameters:		pLeft		- one of the keys
							pRight	- the other key to be compared with

Returned:			0 if the key's are identical otherwise 1
***********************************************************/
int htCompareAlpha (const void *pLeft, const void *pRight)
{
	HT_DATATYPE left, right;

	left = *(HT_DATATYPE *) pLeft;
	right = *(HT_DATATYPE *) pRight;

	return (strcmp (left.key, right.key));
}

/***********************************************************
Function:			htCompareInt

Description:	Compares the alphabet of two datatype counts.

Parameters:		pLeft		- one of the datatypes
							pRight	- the other datatype to be compared with

Returned:			-1 if the left data is less than right data, otherwise 1
***********************************************************/
int htCompareInt (const void *pLeft, const void *pRight)
{
	HT_DATATYPE left, right;

	left = *(HT_DATATYPE *) pLeft;
	right = *(HT_DATATYPE *) pRight;

	if ((left.ctData.value) == (right.ctData.value))
	{
		return htCompareAlpha (pLeft, pRight);
	}

	return (((left.ctData.value) < (right.ctData.value) ? -1 : 1));

}
