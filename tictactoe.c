/* Tic Tac Toe:
	 	Players take turns inputing coordinates between 1 and 3. A '0' represents
	 	an open spot, a '1' represents player one, a '2' player 2. If player
		one wins, a '1' is displayed. If player two wins a '2' is displayed.
		If it is a tie, a '3' is displayed. If there is an error during the
		course of the game, a row of '0's will be displayed and the current
		player must enter valid coordinates.
		Nicole Lewey
		*/

int BOARDSIZE = 9;
int ROWSIZE = 3;
int PLAYERONE = 1;
int PLAYERTWO = 2;
int TIE = 3;
int TRUE = 1;
int FALSE = 0;
int gWinner = 0;

int outputBoard (board)
int board[];
{
	output (board[0], board[1], board[2]);
	output (board[3], board[4], board[5]);
	output (board[6], board[7], board[8]);
	
	return 1;
}

int markCell (board, x, y, bIsPlayerOne)
int board[];
int x, y, bIsPlayerOne;
{
	int bIsError = FALSE;
	
	if (x > -1 && x < ROWSIZE && y > -1 && y < ROWSIZE)
	{
		if (0 == board[(x * ROWSIZE) + y])
		{
			if (1 == bIsPlayerOne)
			{
				board[(x * ROWSIZE) + y] = PLAYERONE;
			}
			else
			{
				board[(x * ROWSIZE) + y] = PLAYERTWO;
			}
		}
		else 
		{
			bIsError = TRUE;
			output (0,0,0,0,0,0,0);
		}
	}
	else
	{
		bIsError = TRUE;
		output (0,0,0,0,0,0,0);
	}
	
	return bIsError;
}

int checkWin (board, bIsPlayerOne)
int board[];
int bIsPlayerOne;
{
	int bIsWinner = 0;
	
	if ((board[0] != 0 && board[0] == board[1] && board[0] == board[2]) ||
			(board[3] != 0 && board[3] == board[4] && board[3] == board[5]) ||
			(board[6] != 0 && board[6] == board[7] && board[6] == board[8]) ||
			(board[0] != 0 && board[0] == board[3] && board[0] == board[6]) ||
			(board[1] != 0 && board[1] == board[4] && board[1] == board[7]) ||
			(board[2] != 0 && board[2] == board[5] && board[2] == board[8]) ||
			(board[0] != 0 && board[0] == board[4] && board[0] == board[8]) ||
			(board[2] != 0 && board[2] == board[4] && board[2] == board[6]))
	{
		bIsWinner = TRUE;
	}
	else if (0 != board[0] && 0 != board[1] && 0 != board[2] && 
			 		 0 != board[3] && 0 != board[4] && 0 != board[5] && 
			 		 0 != board[6] && 0 != board[7] && 0 != board[8])
	{
		bIsWinner = TIE;
	}
	
	if (1 == bIsWinner)
	{
		if (1 == bIsPlayerOne)
		{
			gWinner = PLAYERONE;
		}
		else
		{
			gWinner = PLAYERTWO;
		}
	}
	else if (TIE == bIsWinner)
	{
		gWinner = TIE;
	}
	
	return gWinner;
}

int displayMessage ()
{
	output (gWinner);
}

int switchPlayer (pbIsPlayerOne)
int *pbIsPlayerOne;
{
	if (TRUE == *pbIsPlayerOne)
	{
		*pbIsPlayerOne = FALSE;
	}
	else
	{
		*pbIsPlayerOne = TRUE;
	}
	
	return 1;
}

main ()
{
	int board[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int bIsPlayerOne = FALSE;
	int bIsError = FALSE;
	int x, y, i, j;
	
	for (i = 0; i < BOARDSIZE && FALSE == checkWin (board, bIsPlayerOne) && FALSE == bIsError; i++)
	{
		switchPlayer (&bIsPlayerOne);
		
		outputBoard (board);
		input (x, y);
		x--;
		y--;
		bIsError = markCell (board, x, y, bIsPlayerOne);
		for (j = 0; TRUE == bIsError; j++)
		{
			input (x, y);
			x--;
			y--;
			bIsError = markCell (board, x, y, bIsPlayerOne);
		}
	}
	
	displayMessage;
}
