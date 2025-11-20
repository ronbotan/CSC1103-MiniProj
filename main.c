#include "2P.h"
#include "1P.h"
#include "scoreboard.c"
#include "ML1P.h"

#define scoreboard2P "scoreboard2P.txt"
#define scoreboardEasy "scoreboardEasy.txt"
#define scoreboardNormal "scoreboardNormal.txt"
#define scoreboardHard "scoreboardHard.txt"

#define LABEL_2P "2 Player"
#define LABEL_EASY "Easy"
#define LABEL_NORMAL "Normal"
#define LABEL_HARD "Hard"

int main()
{
    static char player1[20], player2[20];
    // Storing of values in a char array
    char player = 'O'; // Initialise first player as O
    int row, col, result = 0;
    char winner, loser;

    startLoop:
    int mode = 0;              // Assign mode to 0 after start loop. Ensures that mode is not a valid option on restart
    char play = ' ';
    char board[ROW][COLUMN] = {// Reinitialise board
                               {' ', ' ', ' '},
                               {' ', ' ', ' '},
                               {' ', ' ', ' '}};
    char *boardPtr = &board[0][0];
    do {
        printf("\nSelect a Mode\n");
        printf("%-10s | %-5s\n", "Selection:", "Mode");
        printf("-----------------------------------\n");
        printf("%-10s | %-10s\n", "1:", "2 Player");
        printf("%-10s | %-10s\n", "2:", "1 Player Easy");
        printf("%-10s | %-10s\n", "3:", "1 Player Normal");
        printf("%-10s | %-10s\n", "4:", "1 Player Hard");
        printf("%-10s | %-10s\n", "5:", "View Scoreboard");
        printf("Selection: ");
        scanf("%d", &mode);
        getchar(); // Clearing of input buffer
        /* code */
    } while ((mode < 1) || (mode > 5));

    switch (mode) {
    case (1): // Run 2 player mode
        if (play == '0') 
            swapRoles(player1, player2);

        else {
            readName(player1, 'O'); // Read name for player 1
            readName(player2, 'X'); // Read name for player 2
        }

        winner = Run2P(board, boardPtr, player);                        // Run 2 player game and return the winner as X or O
        updateScores(winner, player1, player2, LABEL_2P, scoreboard2P); // Update the scoreboard for the winner. O = P1, X = P2, D = DRAW
        break;

    case (2): // Run 1 player easy mode (MiniMax)
        readName(player1, 'O');
        winner = Run1P(board, 2);
        updateScores(winner, player1, "AI EASY", LABEL_EASY, scoreboardEasy);
        break;

    case (3): // Run 1 player normal mode (ML Algorithm)
        readName(player1, 'O');
        winner = Run1PKnn(board, "KNN/bestmoves.csv");
        updateScores(winner, player1, "AI NORMAL", LABEL_NORMAL, scoreboardNormal);
        break;

    case (4): // Run 1 player hard mode (MiniMax)
        readName(player1, 'O');
        winner = Run1P(board, 4);
        updateScores(winner, player1, "AI HARD", LABEL_HARD, scoreboardHard);
        break;

    case (5):
        printf("\n ----1 Player Scoreboard----");
        showScores(LABEL_EASY, scoreboardEasy);
        showScores(LABEL_NORMAL, scoreboardNormal);
        showScores(LABEL_HARD, scoreboardHard);
        showScores(LABEL_2P, scoreboard2P);
        goto startLoop;
    
    default:
        printf("\nInvalid mode. Please try again.");
    }

    //Using of dowhile loop to check user input validity.
    do {
        printf("\nDo you want to play again? (1 = Yes, 0 = No): "); //Check if user wants to play again
        getchar();                                                  //getchar to clear input buffer
        fflush(stdin);                                              //Clear buffer after reading input
        scanf("%c", &play);                                         //Store input to variable. If input = 1, player again, else exit

        //If else statement when the user input is valid. Other inputs will result in re-prompting for input
        if (play == '1')
            goto startLoop; //If input = 1, go to the start of the main dowhile loop
        else if (play == '0')
            return 0; //Return 0 ends the function main() and terminates the program
        else
            printf("\nInvalid input. Please try again."); //Prompt user of invalid input

    } while (play != '1' || play != '0');
}