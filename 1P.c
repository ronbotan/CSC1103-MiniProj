#include <stdio.h>
#include "gnuplot.h"
#include "1P.h"
#include "2P.h"
#include <time.h>

// 1 Player Mode 
// Difficulty = 2 --> Easy (HUman must win) 
// Difficulty = 4 --> Hard (HUman must lose)
char Run1P(char board[3][3], char* boardPtr, int difficulty)
{
    // Store game outcome
    int result = 0;

    // Used for GUI to draw the winning line
    char winner = ' ';          
    int winLine[3] = {-1,-1,-1};

    // Reset board to blank using spaces for gnuplot
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            board[r][c] = ' ';
        }
    }

    // If gnuplot failed to open, return draw as safe fallback
    if (!init_gnuplot()) {
        printf("Error: gnuplot could not start.\n");
        return 'D';
    }

    // Display the tile on board
    const char *mode_str = (difficulty == 4) ? "Hard" : (difficulty == 2) ? "Easy" : "Custom";
    char title[120];
    snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - Human's turn", mode_str);
    draw(board, winner, winLine, title);     // initial board

    while (1) {
        clock_t start = clock();
        // Human Move
        humanTurn(board);
        clock_t end = clock();
        double time_spent = 1000.0*(end - start) / CLOCKS_PER_SEC;
        printf("Human turn time: %.2f ms\n", time_spent);
        result = checkWin(board, winLine);           // Check if human moved 3 in a row

        if (result == HUMAN) {                       
            winner = HUMAN;                          
            snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - Human wins!", mode_str);   // Display win message
            draw(board, winner, winLine, title);     // Draw the winning line
            printf("\nYou Win!\n");
            break;
        }

        if (checkFull(boardPtr)) {               // No empty cells means draw
            winner = 'D';
            snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - Draw", mode_str);
            draw(board, winner, winLine, title);
            printf("\n It's a Draw!\n");
            break;
        }


        // AI Move
        clock_t ai_start = clock();
        aiTurn(board, difficulty);
        clock_t ai_end = clock();
        double ai_time_spent = 1000.0*(ai_end - ai_start) / CLOCKS_PER_SEC;
        printf("AI turn time: %.2f ms\n", ai_time_spent);            

        result = checkWin(board, winLine);    // Check if AI move 3 in a row

        if (result == AI) {                    
            winner = AI;                      
            snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - AI wins!", mode_str);   // Display win message
            draw(board, winner, winLine, title);     // Draw winning line
            printf("\nAI Wins!\n");          
            break;
        }

        if (checkFull(boardPtr)) {
            winner = 'D';
            snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - Draw", mode_str);
            draw(board, winner, winLine, title);
            printf("\n It's a Draw!\n");
            break;
        }

        snprintf(title, sizeof title, "Tic-Tac-Toe - 1P (%s) - Human's turn", mode_str);
        draw(board, winner, winLine, title);     // Update board after both moves
    }

    // Close gnuplot window
    close_gnuplot();

    // Human wins
    if (result == HUMAN) {
        return HUMAN;
    }   

    // AI wins
    else if (result == AI) {
        return AI;
    }  
    
    // Draw
    else {
        return 'D';           
    }     
      
}

// Recursively tries all possible moves for AI and human
// Determine who has the greater chance of wining
int minimaxLogic(char b[3][3], int isAITurn) {

    // Check if currently have winner
    int winLine[3] = {-1,-1,-1};
    int winner = checkWin(b, winLine);

    // If a winner is found, direct return winner
    if (winner == HUMAN) {
        return 1;
    }
    else if (winner == AI) {
        return 2;
    }

    // No more empty spot and no winner means draw
    char *ptr = (char*)b;

    if (checkFull(ptr)) {
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
                    b[r][c] = ' ';                        // Set the cell back to empty
                        
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
        int foundAIWin = 0;

        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {                       // Only Test empty spot
                    char temp = b[r][c];                  // Save original spot
                    b[r][c] = HUMAN;                      // Temporarily put Human on the empty spot 
                    int result = minimaxLogic(b, 1);      // Simulate AI move after placed Human to empty spot
                    b[r][c] = ' ';                        // Set the cell back to empty

                    // Player can win
                    if (result == 1) {
                        return 1;
                    }  
                    
                    // Draw possible
                    if (result == 3) {
                        foundDraw = 1;
                    }

                    // Track AI win possibility
                    if (result == 2) {
                        foundAIWin = 1;
                    }
                }
            }
        }

        // If a draw move is found, return 3; else AI win, return 2
        if (foundDraw) {
            return 3;
        }
        else if (foundAIWin) {
            return 2;
        }
        else {
            return 1;
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
int aiTurn(char b[3][3], int mode) {
    int bestRow = -1, bestCol = -1;

    // Hard: Human must lose
    if (mode == 4) {

        int winMoves[9][2];            // To store all moves AI can win
        int winCount = 0;              // Number of winning move found                      

        int drawMoves[9][2];           // To store all the move that lead to draw 
        int drawCount = 0;             // Number of draw move found
 
        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c] == ' ') {
                    char temp = b[r][c];                 
                    b[r][c] = AI;                        // Temporary put AI at empty spot
                    int result = minimaxLogic(b, 0);     // Evaluate move outcome using minimax
                    b[r][c] = ' ';                       // Set cell back to empty

                    // AI can win
                    if (result == 2) {                  // If AI can win
                        winMoves[winCount][0] = r;       // Record possible winning moves
                        winMoves[winCount][1] = c;      
                        winCount ++;                     // Add winnng count
                    } 
                    else if (result == 3) {             // If the game draw
                        drawMoves[drawCount][0] = r;     // Record possile draw move
                        drawMoves[drawCount][1] = c;    
                        drawCount++;                     // Add draw count
                    }
                }
            }
        }
        
        // If any winning move exists then use it
        if (winCount > 0) {
            bestRow = winMoves[0][0];
            bestCol = winMoves[0][1];
        }

        // Otherwise pick a draw move
        else if (drawCount > 0) {
            bestRow = drawMoves[0][0];
            bestCol = drawMoves[0][1];
        }

    } 

    // Easy - Human must win
    if (mode == 2) {
        for (int r=0 ; r<3 ; r++) {
            for (int c=0 ; c<3 ; c++) {
                if (b[r][c]==' ') {
                    char temp = b[r][c];
                    b[r][c] = AI;
                    int result = minimaxLogic(b, 0);         // Evaluate move result using minimax
                    b[r][c] = ' ';
                                                           
                    if (result == 1) {  
                        bestRow = r; bestCol = c;             // Pick the one that Human will win
                        break;
                    } 
                    
                    else if (result == 3 && bestRow == -1) {
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

    // Uses for ml_analytics
    int moveIndex = bestRow * 3 + bestCol + 1;
    return moveIndex;
}

// Repeated functions as 2P.c
/*void getWinLine(char b[3][3], int winLine[3]) {

    // Rows
    for (int r=0; r<3; r++) {
        if (b[r][0] != ' ' && b[r][0] == b[r][1] && b[r][1] == b[r][2]) {
            winLine[0] = r*3 + 0;
            winLine[1] = r*3 + 1;
            winLine[2] = r*3 + 2;
            return;
        }
    }

    // Columns
    for (int c=0; c<3; c++) {
        if (b[0][c] != ' ' && b[0][c] == b[1][c] && b[1][c] == b[2][c]) {
            winLine[0] = 0*3 + c;
            winLine[1] = 1*3 + c;
            winLine[2] = 2*3 + c;
            return;
        }
    }

    // Diagonal (top left to bottom right)
    if (b[0][0] != ' ' && b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        winLine[0] = 0;
        winLine[1] = 4;
        winLine[2] = 8;
        return;
    }

    // Diagonal (top right to bottom left)
    if (b[0][2] != ' ' && b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        winLine[0] = 2;
        winLine[1] = 4;
        winLine[2] = 6;
        return;
    }

    // No win
    winLine[0] = winLine[1] = winLine[2] = -1;
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
    
int whoWins(char b[3][3]) {

    for (int i=0 ; i<3 ; i++) {

        // Check each row
        if (b[i][0] != ' ' && b[i][0] == b[i][1] && b[i][1] == b[i][2]) {
            return (b[i][0]==HUMAN) ? 1 : 2;
        }

        // Check each column
        if (b[0][i] != ' ' && b[0][i] == b[1][i] && b[1][i] == b[2][i]) {
            return (b[0][i]==HUMAN) ? 1 : 2;
        }
    }

    // Check diagonal (top left to bottom right)
    if (b[0][0] != ' ' && b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        return (b[0][0]==HUMAN) ? 1 : 2;
    }

    // Check diagonal (top right to bottom left)
    if (b[0][2] != ' ' && b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        return (b[0][2]==HUMAN) ? 1 : 2;
    }

    // No winner yet
    return 0;   
}*/