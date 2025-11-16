#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HUMAN 'O'
#define AI 'X'

int hasEmpty(char b[3][3]);
int whoWins(char b[3][3]);
int minimaxLogic(char b[3][3], int isAITurn);
void aiTurn(char b[3][3], int mode);
void humanTurn(char b[3][3]);
void getWinLine(char b[3][3], int winLine[3]);

/* 1 Player Mode */
/* Difficulty = 2 --> Easy (HUman must win) */
/* Difficulty = 4 --> Hard (HUman must lose) */
char Run1P(char board[3][3], int difficulty)
{
    int result = 0;

    char winner = ' ';          
    int winLine[3] = {-1,-1,-1};

    // Reset board to blank using spaces for gnuplot
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            board[r][c] = ' ';
        }
    }

    // Seed the random number generator (used inside minimax if needed)
    srand((unsigned)time(NULL));

    // If gnuplot failed to open, return draw as safe fallback
    if (!init_gnuplot()) {
        printf("Error: gnuplot could not start.\n");
        return 'D';
    }

    draw(board, winner, winLine);     // initial board

    while (1) {

        // Human Move
        humanTurn(board);

        result = whoWins(board);

        if (result == 1) {
            winner = HUMAN;
            getWinLine(board, winLine);
            draw(board, winner, winLine);
            printf("\nYou Win!\n");
            break;
        }

        if (!hasEmpty(board)) {
            winner = 'D';
            draw(board, winner, winLine);
            printf("\n It's a Draw!\n");
            break;
        }

        // AI Move
        aiTurn(board, difficulty);

        result = whoWins(board);

        if (result == 2) {
            winner = AI;
            getWinLine(board, winLine);
            draw(board, winner, winLine);
            printf("\nAI Wins!\n");
            break;
        }

        if (!hasEmpty(board)) {
            winner = 'D';
            draw(board, winner, winLine);
            printf("\n It's a Draw!\n");
            break;
        }

        draw(board, winner, winLine);
    }

    // Close gnuplot window
    close_gnuplot();

    // Human wins
    if (result == 1) {
        return HUMAN;
    }   

    // AI wins
    else if (result == 2) {
        return AI;
    }  
    
    // Draw
    else {
        return 'D';           
    }          
}

// Check if any empty spot exists
int hasEmpty(char b[3][3]) {
    for (int r=0 ; r<3 ; r++)          // Loop through every row
        for (int c=0 ; c<3 ; c++)      // Loop through every column
            if (b[r][c] == ' ') {      // If the spot is blank
                return 1;              // At least one empty spot exists
            }
    return 0;                          // No empty spot
}

// Check who currently win
/*
   Returns:
   1 = Player win
   2 = AI win
   0 = Draw (no winner)
*/
int whoWins(char b[3][3]) {

    for (int i=0;i<3;i++) {

        // Check each row
        if (b[i][0]!=' ' && b[i][0]==b[i][1] && b[i][1]==b[i][2]) {
            return (b[i][0]==HUMAN) ? 1 : 2;
        }

        // Check each column
        if (b[0][i]!=' ' && b[0][i]==b[1][i] && b[1][i]==b[2][i]) {
            return (b[0][i]==HUMAN) ? 1 : 2;
        }
    }

    // Check diagonal (top left to bottom right)
    if (b[0][0] != ' ' && b[0][0]==b[1][1] && b[1][1]==b[2][2]) {
        return (b[0][0]==HUMAN) ? 1 : 2;
    }

    // Check diagonal (top right to bottom left)
    if (b[0][2] != ' ' && b[0][2]==b[1][1] && b[1][1]==b[2][0]) {
        return (b[0][2]==HUMAN) ? 1 : 2;
    }

    // No winner yet
    return 0;   
}

// Recursively tries all possible moves for AI and human
// Determine who has the greater chance of wining
int minimaxLogic(char b[3][3], int isAITurn) {

    // Check if currently have winner
    int winner = whoWins(b);

    // If a winner is found, direct return winner
    if (winner != 0) {
        return winner;
    }

    // No more empty spot and no winner means draw
    if (!hasEmpty(b)) {
        return 3; 
    } 

    // AI turn
    if (isAITurn == 1) {

        // Use to track if draw is possible
        int foundDraw = 0;

        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c] == ' ') {                     // Only test empty spot
                    char temp = b[r][c];                  // Save original spot
                    b[r][c] = AI;                         // Temporarily put AI on the empty spot
                    int result = minimaxLogic(b, 0);      // Simulate human move after placed AI to empty spot
                    b[r][c] = temp;                       // Undo the move
                        
                    // AI can win
                    if (result == 2) {
                        return 2;
                    }      

                    // Draw possible
                    if (result == 3) {
                        foundDraw = 1;
                    } 
                }
            }
        }

        // If a draw move is found, return 3; else human win, return 1
        return foundDraw ? 3 : 1;     
    } 
    
    // Human turn
    else { 
        int foundDraw = 0;

        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {                       // Only Test empty spot
                    char temp = b[r][c];                  // Save original spot
                    b[r][c] = HUMAN;                      // Temporarily put Human on the empty spot 
                    int result = minimaxLogic(b, 1);      // Simulate AI move after placed Human to empty spot
                    b[r][c] = temp;                       // Undo the move

                    // Player can win
                    if (result == 1) {
                        return 1;
                    }  
                    
                    // Draw possible
                    if (result == 3) {
                        foundDraw = 1;
                    }
                }
            }
        }

        // If a draw move is found, return 3; else AI win, return 2
        return foundDraw ? 3 : 2;
    }
}

// Player action - choose which spot to put
void humanTurn(char b[3][3]) {
    int choice;

    while (1) {
        printf("Enter your move (1-9): ");

        // Check if input number is between 1-9
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 9) {
            printf("Invalid input!\n");
            while(getchar()!='\n');
            continue;
        }
        int r = (choice-1)/3;
        int c = (choice-1)%3;

        // If the spot is not empty
        if (b[r][c]!=' ') {
            printf("This cell is already taken!\n");
        }
        else {
            b[r][c] = HUMAN;      // Place the human move
            break;
        }
    }
}

/* ---------- AI move (Easy/Hard) ---------- */
void aiTurn(char b[3][3], int mode) {
    int bestRow = -1, bestCol = -1;

    // Hard: Human must lose
    if (mode == 4) {
        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {
                    char temp = b[r][c];
                    b[r][c] = AI;
                    int outcome = minimaxLogic(b, 0);          // Same action as minimaxLogic()
                    b[r][c] = temp;

                    // AI can win
                    if (outcome==2) {  
                        bestRow = r; bestCol = c;              // If this move leads AI to win, use it
                        break;
                    } else if (outcome==3 && bestRow==-1) {
                        bestRow = r; bestCol = c;              // Save a draw move if no win found yet    
                    }
                }
            }

            // Stop searching if move chosen
            if (bestRow!=-1) {
                break;
            } 
        }
    } 

    // Easy - Human must win
    if (mode == 2) {
        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {
                    char temp = b[r][c];
                    b[r][c] = AI;
                    int outcome = minimaxLogic(b, 0);
                    b[r][c] = temp;
                                                           
                    if (outcome==1) {  
                        bestRow = r; bestCol = c;             // Pick the one that Human will win
                        break;
                    } 
                    
                    else if (outcome==3 && bestRow==-1) {
                        bestRow = r; bestCol = c;             // Save draw move only if no losing one found yet
                    }
                }
            }

            // Stop searching if move chosen
            if (bestRow != -1) {
                break;
            }
        }
    }

    /* If still not found (shouldnt happen), pick first empty spot */
    if (bestRow == -1) {
        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {
                    bestRow = r;
                    bestCol = c;

                    // exit both loops
                    r = c = 3; 
                }
            }
        }
    }

    // Place AI symbol in the chosen spot
    b[bestRow][bestCol] = AI;
}

// Compute winning line for 1-player mode
void getWinLine(char b[3][3], int winLine[3]) {

    // Rows
    for (int r=0; r<3; r++) {
        if (b[r][0]!=' ' && b[r][0]==b[r][1] && b[r][1]==b[r][2]) {
            winLine[0] = r*3 + 0;
            winLine[1] = r*3 + 1;
            winLine[2] = r*3 + 2;
            return;
        }
    }

    // Columns
    for (int c=0; c<3; c++) {
        if (b[0][c]!=' ' && b[0][c]==b[1][c] && b[1][c]==b[2][c]) {
            winLine[0] = 0*3 + c;
            winLine[1] = 1*3 + c;
            winLine[2] = 2*3 + c;
            return;
        }
    }

    // Diagonal (top left to bottom right)
    if (b[0][0]!=' ' && b[0][0]==b[1][1] && b[1][1]==b[2][2]) {
        winLine[0] = 0;
        winLine[1] = 4;
        winLine[2] = 8;
        return;
    }

    // Diagonal (top right to bottom left)
    if (b[0][2]!=' ' && b[0][2]==b[1][1] && b[1][1]==b[2][0]) {
        winLine[0] = 2;
        winLine[1] = 4;
        winLine[2] = 6;
        return;
    }

    // No win
    winLine[0] = winLine[1] = winLine[2] = -1;
}