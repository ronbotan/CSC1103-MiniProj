#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HUMAN 'X'
#define AI 'O'

void showBoard(char b[3][3]);
int hasEmpty(char b[3][3]);
int whoWins(char b[3][3]);
int minimaxLogic(char b[3][3], int isAITurn);
void aiTurn(char b[3][3], int mode);
void humanTurn(char b[3][3]);
int checkState(char b[3][3]);

int main() {
    char board[3][3] = { {'1','2','3'}, {'4','5','6'}, {'7','8','9'} };
    int result = 0, mode;

    srand(time(NULL));

    printf("==== Tic Tac Toe ====\n");
    printf("Select difficulty:\n");
    printf("1 - Easy\n");
    printf("2 - Hard\n");
    printf("Enter your choice: ");
    scanf("%d", &mode);

    while (1) {
        showBoard(board);             // show current board
        humanTurn(board);             // human always move first
        result = checkState(board);   // check if game ends
        if (result != 0) { 
            break;                    // Loop stops, found winner or draw
        }

        aiTurn(board, mode);          // AI move
        result = checkState(board);   // check if game ends
        if (result != 0) {
            break;                    // Loop stops, found winner or draw
        }
    }

    showBoard(board);                 // Once loop ends, display board and output message
    if (result == 1) {
        printf("You win!\n");
    }
    else if (result == 2) {
        printf("AI wins!\n");
    }
    else {
        printf("It's a draw!\n");
    }
}

// GUI - Display the board
void showBoard(char b[3][3]) {
    printf("\n");
    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            printf(" %c ", b[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Check if any empty spot exists
int hasEmpty(char b[3][3]) {
    for (int r=0;r<3;r++)                              // Loop through every row
        for (int c=0;c<3;c++)                          // Loop through every column
            if (b[r][c] != HUMAN && b[r][c] != AI) {   // Spot is not put by either AI or human
                return 1;                              // At least one empty spot exists
            }
    return 0;
}

// Check who currently win
/*
   Returns:
   1 = Player win
   2 = AI win
   3 = Draw (no winner)
*/
int whoWins(char b[3][3]) {

    for (int i=0;i<3;i++) {

        // Check each row
        if (b[i][0]==b[i][1] && b[i][1]==b[i][2]) {
            if (b[i][0]==HUMAN) {
                return 1;
            }
            if (b[i][0]==AI) {
                return 2;
            }
        }

        // Check each column
        if (b[0][i]==b[1][i] && b[1][i]==b[2][i]) {
            if (b[0][i]==HUMAN) {
                return 1;
            }
            if (b[0][i]==AI) {
                return 2;
            }
        }
    }

    // Check diagonal (top left to bottom right)
    if (b[0][0]==b[1][1] && b[1][1]==b[2][2]) {
        if (b[0][0]==HUMAN) {
            return 1;
        }
        if (b[0][0]==AI) {
            return 2;
        }
    }

    // Check diagonal (top right to bottom left)
    if (b[0][2]==b[1][1] && b[1][1]==b[2][0]) {
        if (b[0][2]==HUMAN) {
            return 1;
        }
        if (b[0][2]==AI) {
            return 2;
        }
    }

    // No winner yet
    return 0;   
}

// Assume every possible move to find which moves will win or lose
int minimaxLogic(char b[3][3], int isAITurn) {
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
        int foundDraw = 0;
        for (int r=0;r<3;r++) {
            for (int c=0;c<3;c++) {
                if (b[r][c]!=HUMAN && b[r][c]!=AI) {      // search for empty spot
                    char temp = b[r][c];
                    b[r][c] = AI;                         // temporarily put AI on the empty spot
                    int result = minimaxLogic(b, 0);      // see what happens if AI move to empty spot, then player move next, then AI again
                    b[r][c] = temp;                       // put back the value
                        
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

        // when a draw is found
        if (foundDraw) {
            return 3;   
        } 

        // when not a draw (someone wins but not AI - Player win)
        else {
            return 1;   
        }
    } 
    
    // Human turn
    else { 
        int foundDraw = 0;
        for (int r=0;r<3;r++) {
            for (int c=0;c<3;c++) {
                if (b[r][c]!=HUMAN && b[r][c]!=AI) {
                    char temp = b[r][c];                   // same as above
                    b[r][c] = HUMAN;
                    int result = minimaxLogic(b, 1);
                    b[r][c] = temp;

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
        // when a draw is found
        if (foundDraw ==  1) {
            return 3;   
        } 

        // when not a draw (someone wins)
        else {
            return 2;   
        }
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
        if (b[r][c]==HUMAN || b[r][c]==AI) {
            printf("That cell is already used!\n");
        }
        else {
            b[r][c] = HUMAN;
            break;
        }
    }
}

/* ---------- AI move (Easy/Hard) ---------- */
void aiTurn(char b[3][3], int mode) {
    int bestRow = -1, bestCol = -1;
    int foundDraw = 0;

    // Hard: Human must lose
    if (mode == 2) {
        for (int r=0;r<3;r++) {
            for (int c=0;c<3;c++) {
                if (b[r][c]!=HUMAN && b[r][c]!=AI) {
                    char temp = b[r][c];
                    b[r][c] = AI;
                    int outcome = minimaxLogic(b, 0);      // same logic as above
                    b[r][c] = temp;

                    // AI can win
                    if (outcome == 2) {  
                        bestRow = r; bestCol = c;
                        break;
                    } else if (outcome == 3 && bestRow == -1) {
                        // Save a draw move if no win found yet
                        bestRow = r; bestCol = c;
                    }
                }
            }

            // break outer loop once move found
            if (bestRow != -1) {
                break;
            } 
        }
    } 

    // Easy - Human must win
    else {
        for (int r=0;r<3;r++) {
            for (int c=0;c<3;c++) {
                if (b[r][c]!=HUMAN && b[r][c]!=AI) {
                    char temp = b[r][c];
                    b[r][c] = AI;
                    int outcome = minimaxLogic(b, 0);
                    b[r][c] = temp;

                    // Player will win → pick it
                    if (outcome == 1) {  
                        bestRow = r; bestCol = c;
                        break;
                    } 
                    
                    else if (outcome == 3 && bestRow == -1) {
                        // Save draw move only if no losing one found yet
                        bestRow = r; bestCol = c;
                    }
                }
            }

            // break outer loop once chosen
            if (bestRow != -1) {
                break;
            }
        }
    }

    /* If still not found (shouldnt happen), pick first empty spot */
    if (bestRow == -1)
        for (int r=0;r<3;r++)
            for (int c=0;c<3;c++)
                if (b[r][c]!=HUMAN && b[r][c]!=AI) {
                    bestRow = r;
                    bestCol = c;

                    // exit both loops
                    r = c = 3; 
                }

    b[bestRow][bestCol] = AI;
}

// Check current game status
int checkState(char b[3][3]) {
    int w = whoWins(b);

    // Player win
    if (w == 1) {
        return 1;
    }

    // AI win
    if (w == 2) {
        return 2;
    }

    // Draw
    if (!hasEmpty(b)) {
        return 3;
    }

    return 0;
}
