#include <stdio.h>

#define SIZE 3

void printBoard(char board[SIZE][SIZE]);
int checkWin(char board[SIZE][SIZE]);
int isFull(char board[SIZE][SIZE]);

int main() {
    char board[SIZE][SIZE] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };
    char player = 'X';
    int row, col, result = 0;

    printf("Tic Tac Toe Game\n");
    printf("Player 1: X | Player 2: O\n\n");

    while (1) {
        printBoard(board);
        printf("Player %c, enter row and column (1-3 1-3): ", player);
        scanf("%d %d", &row, &col);

        // Validate input
        if (row < 1 || row > 3 || col < 1 || col > 3) {
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

        // Switch player
        player = (player == 'X') ? 'O' : 'X';
    }

    return 0;
}

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

int checkWin(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        // Check rows and columns
        if ((board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) ||
            (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]))
            return 1;
    }

    // Check diagonals
    if ((board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
        (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]))
        return 1;

    return 0;
}

int isFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                return 0;
    return 1;
}