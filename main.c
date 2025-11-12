#include "2p.c"
#include "scoreboard.c"


int main() {
    static char player1[50], player2[50];
    printf("\nEnter name for Player O: ");
    fgets(player1, 50, stdin);
    trim(player1);
    printf("Enter name for Player X: ");
    fgets(player2, 50, stdin);
    trim(player2);

    //Storing of values in a char array
    char board[ROW][COLUMN] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };
    char* boardPtr = &board[0][0];
    
    char player = 'O';
    int row, col, result = 0;
    char winner, loser;

    printf("Tic Tac Toe\n");
    printf("Player 1: O | Player 2: X\n\n");

    while (1) {
        printBoard(board);
        printf("Player %c, enter row and column (1-3 1-3): ", player);
        scanf("%d %d", &row, &col);

        //Validate input
        if ((row<1)||(row>3)||(col<1)||(col>3)) {
            printf("Invalid position. Try again.\n");
            continue;
        }

        //Adjust for 0-based index
        row--;
        col--;

        //Check if cell is empty
        if (board[row][col] != ' ') {
            printf("Cell already taken. Try again.\n");
            continue;
        }

        //Make move
        board[row][col] = player;

        //Check for win
        result = checkWin(board);
        if (result) {
            printBoard(board);
            printf("Player %c wins!\n", player);
            winner = player;
            loser = (player=='O')?'X':'O';
            break;
        } else if (checkFull(boardPtr)) { //Check for draw
            printBoard(board);
            printf("It's a draw!\n");
            winner = 'D';
            break;
        }
        // One line if else statement based on lecture notes
        // If player==O is true player becomes X. Else if player==O is false, player becomes O 
        player = (player=='O')?'X':'O'; 
        
    }

    if (winner == 'O') { //Player 1 win
        updatePlayerScore(player1);
        updateLoserScore(player2);
    }
    else if (winner == 'X') {//Player 2 win
        updatePlayerScore(player2);
        updateLoserScore(player1);
    }
    else if (winner == 'D') { //DRAW
        updateDraw(player1, player2);
    }
    showScores();
    
    return 0;
}
