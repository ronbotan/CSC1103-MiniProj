#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // For mkdir

/*
Encoding:
  X  ->  1
  O  -> -1
  empty -> 0

Board index mapping:
  d1 d2 d3
  d4 d5 d6
  d7 d8 d9

bestmove is 1–9 (0 if no move available).
*/

// Function Prototypes
int check_winner(int board[9]);
int is_moves_left(int board[9]);
int minimax(int board[9], int depth, int isMaximizing);
int current_player(int board[9]);
int find_best_move(int board[9]);
long board_to_int(int board[9]);
void generate_and_write_data(int board[9], FILE *fp, char seen_states[]);

// Check if someone has won.
// Returns  1 if X wins, -1 if O wins, 0 otherwise.
int check_winner(int board[9]) {
    int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8}, // rows
        {0,3,6}, {1,4,7}, {2,5,8}, // cols
        {0,4,8}, {2,4,6}           // diagonals
    };

    for (int i = 0; i < 8; i++) {
        int a = wins[i][0];
        int b = wins[i][1];
        int c = wins[i][2];
        int sum = board[a] + board[b] + board[c];
        if (sum == 3)  return  1;   // X wins
        if (sum == -3) return -1;   // O wins
    }
    return 0;
}

// Returns 1 if there is any empty cell; 0 otherwise.
int is_moves_left(int board[9]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) return 1;
    }
    return 0;
}

// Minimax: returns score from X's perspective
int minimax(int board[9], int depth, int isMaximizing) {
    int winner = check_winner(board);
    if (winner != 0) return (winner == 1) ? 10 - depth : -10 + depth;
    if (!is_moves_left(board)) return 0;

    int best = isMaximizing ? -1000 : 1000;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = isMaximizing ? 1 : -1;
            int score = minimax(board, depth + 1, !isMaximizing);
            board[i] = 0; // undo move
            if (isMaximizing) {
                if (score > best) best = score;
            } else {
                if (score < best) best = score;
            }
        }
    }
    return best;
}

// Determine whose turn it is based on board state.
int current_player(int board[9]) {
    int countX = 0, countO = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 1)  countX++;
        if (board[i] == -1) countO++;
    }
    if (countX == countO)      return  1; // X's turn
    else if (countX == countO + 1) return -1; // O's turn
    else return 0; // invalid position
}

// Find best move for whoever's turn it is now.
int find_best_move(int board[9]) {
    int player = current_player(board);
    if (player == 0 || !is_moves_left(board) || check_winner(board) != 0) {
        return -1; // invalid or game already over
    }

    int bestMove = -1;
    int bestVal = (player == 1) ? -1000 : 1000;

    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = player;
            int moveVal = minimax(board, 0, (player == -1));
            board[i] = 0; // undo

            if (player == 1) { // X is maximizing
                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    bestMove = i;
                }
            } else { // O is minimizing
                if (moveVal < bestVal) {
                    bestVal = moveVal;
                    bestMove = i;
                }
            }
        }
    }
    return bestMove;
}

// NEW FUNCTION: Convert board state to a unique integer ID.
// We map (X:1, O:-1, empty:0) to the digits (2, 0, 1) for a base-3 number.
long board_to_int(int board[9]) {
    long unique_id = 0;
    long power_of_3 = 1;
    for (int i = 0; i < 9; i++) {
        int digit = board[i] + 1; // Map {-1, 0, 1} to {0, 1, 2}
        unique_id += digit * power_of_3;
        power_of_3 *= 3;
    }
    return unique_id;
}


// MODIFIED FUNCTION: Now prevents duplicate processing.
void generate_and_write_data(int board[9], FILE *fp, char seen_states[]) {
    // 1. Get unique ID for the current board
    long board_id = board_to_int(board);

    // 2. If we've already processed this state, stop this path.
    if (seen_states[board_id] == 1) {
        return;
    }

    // 3. Mark this state as seen so we don't process it again.
    seen_states[board_id] = 1;

    // 4. Find and write the best move for the current board state
    int bestIdx = find_best_move(board);
    if (bestIdx != -1) {
        for (int i = 0; i < 9; i++) {
            fprintf(fp, "%d,", board[i]);
        }
        fprintf(fp, "%d\n", bestIdx + 1);
    }

    // 5. If game is over, stop recursing down this path
    if (check_winner(board) != 0 || !is_moves_left(board)) {
        return;
    }

    // 6. Recurse for next possible moves
    int player = current_player(board);
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            board[i] = player; // Make a move
            generate_and_write_data(board, fp, seen_states); // Recurse
            board[i] = 0; // Undo the move (backtrack)
        }
    }
}


int main(void) {
    // Create the directory if it doesn't exist (for Linux/macOS)
    // For Windows, you might need to use _mkdir or create it manually
    
    const char *filename = "KNN/bestmoves.csv";
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        perror("Error opening file. Make sure the 'KNN' directory exists");
        return 1;
    }
    
    // The max ID is 3^9 - 1 = 19682. We need an array of size 19683.
    // Use calloc to initialize all elements to 0 ('not seen').
    char *seen_states = (char*)calloc(19683, sizeof(char));
    if (seen_states == NULL) {
        printf("Failed to allocate memory for seen_states.\n");
        fclose(fp);
        return 1;
    }

    // Write the header
    fprintf(fp, "d1,d2,d3,d4,d5,d6,d7,d8,d9,bestmove\n");

    // Initial empty board
    int initial_board[9] = {0,0,0,0,0,0,0,0,0};

    // Start the recursive generation process
    printf("Generating unique dataset... This may take a moment.\n");
    generate_and_write_data(initial_board, fp, seen_states);

    fclose(fp); // Close the file.
    free(seen_states); // IMPORTANT: Free the allocated memory

    printf("Duplicate-free dataset has been successfully generated in %s\n", filename);

    return 0;
}