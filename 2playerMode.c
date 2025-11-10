#include <stdio.h>

#define SIZE 3

void printBoard(char board[SIZE][SIZE]); // Function to print GUI. To be replaced by Shaun
int checkWin(char board[SIZE][SIZE]); // Function to check if anyone won
int isFull(char board[SIZE][SIZE]); // Function to declare no winners if board is full

int main() {
    // Storing of values in a char array
    char board[SIZE][SIZE] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };
    char player = 'X';
    int row, col, result = 0;

    printf("Tic Tac Toe\n");
    printf("Player 1: O | Player 2: X\n\n");

    while (1) {
        printBoard(board);
        printf("Player %c, enter row and column (1-3 1-3): ", player);
        scanf("%d %d", &row, &col);

        // Validate input
        if ((row<1)||(row>3)||(col<1)||(col>3)) {
            printf("Invalid position. Try again.\n");
            continue;
        }

        // Adjust for 0-based index
        row--;
        col--;

        // Check if cell is empty
        if (board[row][col] != ' ') {
            printf("Cell already taken. Try again.\n");
            continue;
        }

        // Make move
        board[row][col] = player;

        // Check for win or draw
        result = checkWin(board);
        if (result) {
            printBoard(board);
            printf("Player %c wins!\n", player);
            break;
        } else if (isFull(board)) {
            printBoard(board);
            printf("It's a draw!\n");
            break;
        }

        // One line if else statement based on notes. 
        // If player==O is true player becomes X. Else if player==O is false, player becomes O 
        player = (player=='O')?'X':'O'; 
        
    }

    return 0;
}

//To be removed. GUI 3x3 board for testing purposes
void printBoard(char board[SIZE][SIZE]) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf(" %c ", board[i][j]);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("---+---+---\n");
    }
    printf("\n");
}

// Check if anyone won
int checkWin(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        // Check rows and columns
        if ((board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) || (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]))
            return 1;
    }
    // Check diagonals
    if ((board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) || (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]))
        return 1;
    return 0;
}

// Check if board is full. Board is full, DRAWWWW
int isFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                return 0;
    return 1;
}