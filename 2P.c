#include "gnuplot.h"
#include <stdio.h>
#include <time.h>

#define ROW 3
#define COLUMN 3

//Pass by value, 2D Array values passed from main to sub-function to check for winners
char checkWin(char b[ROW][COLUMN], int winline[3]) {
    for (int r = 0; r < 3; r++) {
        if (b[r][0] != ' ' && ((b[r][0] == b[r][1]) && (b[r][1] == b[r][2]))) {
            //If the condition fits, assign the winner using the value inside
            winner = b[r][0];
            //Assign the box number for winline. Used for draw function
            winline[0] = (r*3+0); //current row position + 0 (first value)
            winline[1] = (r*3+1); //current row position + 1 (second value)
            winline[2] = (r*3+2); //current row position + 2 (third value)
            return winner;
        }
    }

    // Check columns. If first value of column is not blank, check if all 3 are the same
    for (int c = 0; c < 3; c++) {
        if (b[0][c] != ' ' && ((b[0][c] == b[1][c]) && (b[1][c] == b[2][c]))) {
            //If the condition fits, assign the winner using the value inside
            winner = b[0][c];
            //Assign the box number for winline. Used for draw function
            winline[0] = (0*3+c); //row 0 + current column position
            winline[1] = (1*3+c); //row 1 + current column position
            winline[2] = (2*3+c); //row 2 + current column position
            return winner;
        }
    }

    //If center is not blank
    if (b[1][1] != ' ') {
        // Check topleft to bottomright diagonal
        if( (b[0][0] == b[1][1]) && (b[1][1] == b[2][2])) {
            //If the condition fits, assign the winner using the value inside
            winner = b[1][1];
            //Assign the box number for winline. Used for draw function
            winline[0] = 0;
            winline[1] = 4;
            winline[2] = 8;
            return winner;
        }
        // Check topright to bottomleft diagonal
        else if ((b[0][2] == b[1][1]) && (b[1][1] == b[2][0]))
        {
            //If the condition fits, assign the winner using the value inside
            winner = b[1][1];
            //Assign the box number for winline. Used for draw function
            winline[0] = 2; 
            winline[1] = 4; 
            winline[2] = 6;
            return winner;
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

//2 Player Game to be run in main
char Run2P(char board[][3], char* boardPtr, char player, char player1[], char player2[]){
    char winner, loser;
    int input, row, col, result;
    int move_number = 1;
    static int winLine[3] = {-1, -1, -1};

    if (!init_gnuplot()) {
        printf("Could not open gnuplot.\n");
        return 1;
    }

    while (1)
    {
        char title[120];
        snprintf(title, sizeof title, "Tic-Tac-Toe - 2P - Player %c's turn - Move %d", player, move_number);
        draw(board, winner, winLine, title); //Call Draw function from GUI
        clock_t start = clock();
        if(player == 'O')
            printf("\n%s (%c), select a box (1-9): ", player1, player);
        else
            printf("\n%s (%c), select a box (1-9): ", player2, player);
        
        if (scanf("%d", &input) != 1 || input < 1 || input > 9) {    
            printf("Invalid input! Please enter a number between 1 and 9.\n");
            while(getchar()!='\n');
            continue;
        }

        // Adjust for 0-based index
        row = (input - 1) / 3;
        col = (input - 1) % 3;

        // Check if cell is empty
        if (board[row][col] != ' ' && input != 0) {
            printf("Cell already taken. Try again.\n");
            continue;
        }

        // Make move
        board[row][col] = player;
        clock_t end = clock();
        double time_spent = 1000.0*(end - start) / CLOCKS_PER_SEC;
        printf("Player %c turn time: %.2f ms\n", player, time_spent);
        snprintf(title, sizeof title, "Tic-Tac-Toe - 2P - Player %c moved - Move %d", player, move_number);
        draw(board, winner, winLine, title);

        // Check for win
        result = checkWin(board, winLine);
        if (result){
            winner = player;
            snprintf(title, sizeof title, "Tic-Tac-Toe - 2P - Player %c wins!", player);
            draw(board, winner, winLine, title); //Call Draw function from GUI
            loser = (player == 'O') ? 'X' : 'O';
            break;
        }
        else if (checkFull(boardPtr)) { // Check for draw
            printf("It's a draw!\n");
            winner = 'D';
            draw(board, winner, winLine, "Tic-Tac-Toe - 2P - Draw"); //Call Draw function from GUI
            break;
        }
        // advance to next move / player
        move_number++;
        player = (player == 'O') ? 'X' : 'O';
    }
    close_gnuplot();

    return winner;
}
