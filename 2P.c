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