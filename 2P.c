#include "gnuplot.c"
#include <stdio.h>

#define ROW 3
#define COLUMN 3

//Pass by value, 2D Array values passed from main to sub-function to check for winners
int checkWin(char b[ROW][COLUMN], int winline[3]) {
    for (int r = 0; r < 3; r++) {
        if (b[r][0] != ' ' && ((b[r][0] == b[r][1]) && (b[r][1] == b[r][2]))) {
            //If the condition fits, assign the winner using the value inside
            winner = b[r][0];
            //Assign the box number for winline. Used for draw function
            winline[0] = (r*3+0);
            winline[1] = (r*3+1);
            winline[2] = (r*3+2);
            return winner;
        }
    }

    // Check columns. If first value of column is not blank, check if all 3 are the same
    for (int c = 0; c < 3; c++) {
        if (b[0][c] != ' ' && ((b[0][c] == b[1][c]) && (b[1][c] == b[2][c]))) {
            //If the condition fits, assign the winner using the value inside
            winner = b[0][c];
            //Assign the box number for winline. Used for draw function
            winline[0] = (0*3+c);
            winline[1] = (1*3+c);
            winline[2] = (2*3+c);
            return winner;
        }
    }

    // Check topleft to bottomright diagonal
    if (b[1][1] != ' ' && ((b[0][0] == b[1][1]) && (b[1][1] == b[2][2]))) {
        //If the condition fits, assign the winner using the value inside
        winner = b[1][1];
        //Assign the box number for winline. Used for draw function
        winline[0] = 0;
        winline[1] = 4;
        winline[2] = 8;
        return winner;
    }

    // Check topright to bottomleft diagonal
    if (b[1][1] != ' ' && ((b[0][2] == b[1][1]) && (b[1][1] == b[2][0]))) {
        //If the condition fits, assign the winner using the value inside
        winner = b[1][1];
        //Assign the box number for winline. Used for draw function
        winline[0] = 2; 
        winline[1] = 4; 
        winline[2] = 6;
        return winner;
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


char Run2P(char board[][3], char* boardPtr, char player){
    char winner, loser;
    int input, row, col, result;
    static int winLine[3] = {-1, -1, -1};

    if (!init_gnuplot()) {
        printf("Could not open gnuplot.\n");
        return 1;
    }

    while (1)
    {
        draw(board, winner, winLine);
        fflush(stdin);
        printf("\nPlayer %c, select a box (1-9): ", player);
        scanf("%d", &input);

        // Validate input
        if ((input < 1) || (input > 9)) {
            printf("Invalid position. Try again.\n");
            continue;
        }

        // Adjust for 0-based index
        row = (input - 1) / 3;
        col = (input - 1) % 3;

        // Check if cell is empty
        if (board[row][col] != ' ') {
            printf("Cell already taken. Try again.\n");
            continue;
        }

        // Make move
        board[row][col] = player;
        draw(board, winner, winLine);

        // Check for win
        result = checkWin(board, winLine);
        if (result){
            printf("Player %c wins!\n", player);
            winner = player;
            draw(board, winner, winLine);
            loser = (player == 'O') ? 'X' : 'O';
            break;
        }
        else if (checkFull(boardPtr)) { // Check for draw
            printf("It's a draw!\n");
            winner = 'D';
            draw(board, winner, winLine);
            break;
        }
        // One line if else statement based on lecture notes
        // If player==O is true player becomes X. Else if player==O is false, player becomes O
        player = (player == 'O') ? 'X' : 'O';
    }
    close_gnuplot();

    return winner;
}
