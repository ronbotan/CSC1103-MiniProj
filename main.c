#include "2P.c"
#include "1P.c"
#include "scoreboard.c"


int main() {
    int mode;
    static char player1[20], player2[20];
    //Storing of values in a char array
    char board[ROW][COLUMN] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '}
    };

    char* boardPtr = &board[0][0];
    
    char player = 'O'; //Initialise first player as O
    int row, col, result = 0;
    char winner, loser;

    do
    {
        printf("\nSelect a Mode\n");
        printf("%-10s | %-5s\n", "Selection:", "Mode");
        printf("-----------------------------------\n");
        printf("%-10s | %-10s\n", "1:", "2 Player");
        printf("%-10s | %-10s\n", "2:", "1 Player Easy");
        printf("%-10s | %-10s\n", "3:", "1 Player Normal");
        printf("%-10s | %-10s\n", "4:", "1 Player Hard");
        printf("Selection: ");
        scanf("%d", &mode);
        getchar(); //Clearing of input buffer
        /* code */
    } while ((mode<1) || (mode>4));
    
    switch (mode) {
        case (1): //Run 2 player mode
            printf("\n2 Player Mode!");
            readName(player1, 'O'); //Assign player 1 as O
            readName(player2, 'X'); //Assign player 2 as X
            winner = Run2P(board, boardPtr, player); //Run 2 player game and return the winner as X or O
            updateScores(winner, player1, player2); //Update the scoreboard for the winner. O = P1, X = P2, D = DRAW
            break;
        
        case (2): //Run 1 player easy mode
            readName(player1, 'X');
            strcpy(player2, "AI");
            winner = Run1P(board, 2);   
            updateScores(winner, player2, player1);
            break;
            
        case (3): //Run 1 player normal mode (CHARMAIN TAN JIA YI)
            readName(player1, player);
            break;

        case (4): //Run 1 player hard mode
            readName(player1, 'X');
            strcpy(player2, "AI");
            winner = Run1P(board, 4);
            updateScores(winner, player2, player1);
            break;
        }
    
    return 0;
}