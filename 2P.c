#include <stdio.h>

#define ROW 3
#define COLUMN 3

void printBoard(char board[ROW][COLUMN]) {
    printf("\n");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf(" %c ", board[i][j]);
            if (j < COLUMN - 1) printf("|");
        }
        printf("\n");
        if (i < ROW - 1) printf("---+---+---\n");
    }
    printf("\n");
}

//Pass by value, 2D Array values passed from main to sub-function to check for winners
int checkWin(char board[ROW][COLUMN]) {
    for (int i = 0; i < ROW; i++) {
        // Check rows and columns
        if ((board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) || (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]))
            return 1;
    }
    // Check diagonals
    if (board[1][1] != ' ') { //If centre is taken, then check further. If centre is not taken. there is no diagonals. Nested if loops
        if((board[0][0] == board[1][1] && board[1][1] == board[2][2])) {
            return 1;
        }
        if(board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
            return 1;
        }
    }
    return 0;
}

//Pass by reference, using pointers
int checkFull(char* charPtr) {
    for (int i = 0; i < (ROW*COLUMN); i++) {
        if(*(charPtr+i) == ' ') //Detects a blank
            return 0; //Returns 0, to say that board is not full
    } return 1; //If all iterations run and there is no blanks, return 1 to indicate it is full
}


char Run2P(char board[][3], char* boardPtr, char player)
{
    char winner, loser;
    int row, col, result;

    while (1)
    {
        printBoard(board);
        printf("Player %c, enter row and column (1-3 1-3): ", player);
        scanf("%d %d", &row, &col);

        // Validate input
        if ((row < 1) || (row > 3) || (col < 1) || (col > 3))
        {
            printf("Invalid position. Try again.\n");
            continue;
        }

        // Adjust for 0-based index
        row--;
        col--;

        // Check if cell is empty
        if (board[row][col] != ' ')
        {
            printf("Cell already taken. Try again.\n");
            continue;
        }

        // Make move
        board[row][col] = player;

        // Check for win
        result = checkWin(board);
        if (result)
        {
            printBoard(board);
            printf("Player %c wins!\n", player);
            winner = player;
            loser = (player == 'O') ? 'X' : 'O';
            break;
        }
        else if (checkFull(boardPtr))
        { // Check for draw
            printBoard(board);
            printf("It's a draw!\n");
            winner = 'D';
            break;
        }
        // One line if else statement based on lecture notes
        // If player==O is true player becomes X. Else if player==O is false, player becomes O
        player = (player == 'O') ? 'X' : 'O';
    }
    return winner;
}
