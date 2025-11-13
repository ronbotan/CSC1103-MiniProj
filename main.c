#include "2p.c"
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

    printf("\n ⭕ ❌ Select a Mode ❌ ⭕\n");
    printf("%-10s | %-5s\n", "Selection:", "Mode");
    printf("-----------------------------------\n");
    
    printf("%-10s | %-10s\n", "1:", "2 Player 🙋 🙋");
    printf("%-10s | %-10s\n", "2:", "1 Player Easy 🤖");
    printf("%-10s | %-10s\n", "3:", "1 Player Normal 🤖");
    printf("%-10s | %-10s\n", "4:", "1 Player Hard 🤖");
    scanf("%d", &mode);
    getchar(); //Clearing of input buffer
    
    switch (mode) {
        case (1): //Run 2 player mode
            printf("\n2 Player Mode!");
            readName(player1, player);
            player = (player == 'O') ? 'X' : 'O';

            readName(player2, player);
            player = (player == 'O') ? 'X' : 'O';

            winner = Run2P(board, boardPtr, player);
            updateScores(winner, player1, player2);
            break;
        
        case (2): //Run 1 player easy mode
            readName(player1, player);
            break;
            
        case (3): //Run 1 player normal mode (CHARMAIN TAN JIA YI)
            break;

        case (4): //Run 1 player hard mode
            break;

        default:
            break;
        }
    
    return 0;
}
