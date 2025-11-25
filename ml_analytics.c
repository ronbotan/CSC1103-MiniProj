#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "ML.h"
#include "1P.h"
#include "2P.h"

#define K 5
#define MAX_LINE_LEN 1024
#define TRAIN_SPLIT 0.8

typedef enum {
    PLAYER_MINIMAX_PERFECT,
    PLAYER_MINIMAX_IMPERFECT,
    PLAYER_KNN
} PlayerType;

// --- Forward Declarations ---
// DataPoint* knn_load_dataset(const char* filename, int* num_records);
void shuffle_dataset(DataPoint* dataset, int n);
int minimax_prediction(const int board9[9], int mode);
static void print_results(const char *label, int correct, int total);


int minimax_prediction_player(const int board9[9], int mode, int player);
int check_win_intboard(const int board9[9], int player);
int board_full_int(const int board9[9]);
int choose_move(PlayerType type, int current_player,
                const int board9[9], DataPoint* train_set, int train_size);
char simulate_game(PlayerType playerX, PlayerType playerO,
                   DataPoint* train_set, int train_size);
void simulate_matchups(DataPoint* train_set, int train_size, int num_games);

// --- Main Program ---
int main(void) {
    srand(time(NULL));

    // 1. Load the dataset
    int num_records = 0;
    printf("Loading dataset from 'bestmoves.csv'...\n");
    DataPoint* all_data = knn_load_dataset("KNN/bestmoves.csv", &num_records);
    if (all_data == NULL) return 1;
    printf("Dataset loaded with %d records.\n", num_records);

    // 2. Shuffle the dataset
    printf("Shuffling dataset...\n");
    shuffle_dataset(all_data, num_records);

    // 3. Split the data
    int train_size = (int)(num_records * TRAIN_SPLIT);
    int test_size = num_records - train_size;
    DataPoint* train_set = all_data;
    DataPoint* test_set = all_data + train_size;
    printf("Splitting data: %d for training, %d for testing.\n", train_size, test_size);

    // 4. Evaluate the model
    printf("\n--- Starting Model Evaluation ---\n");
    fflush(stdout); // <<-- ADDED: Flush buffer before starting the loop

    int correct_prediction_knn = 0, correct_prediction_perfect = 0, correct_prediction_imperfect = 0;

    for (int i = 0; i < test_size; i++) {
        int* current_test_board = test_set[i].board;
        int true_best_move = test_set[i].best_move;

        int knn_move = knn_predict_move(train_set, train_size, current_test_board);
        if (knn_move == true_best_move) 
        correct_prediction_knn++;

        int perfect_move  = minimax_prediction(current_test_board, 4);
        if (perfect_move == true_best_move) 
        correct_prediction_perfect++;

        
        int imperfect_move   = minimax_prediction(current_test_board, 2);
        if (imperfect_move == true_best_move) 
        correct_prediction_imperfect++;

        // MODIFIED: Added spaces at the end to ensure the line is fully overwritten
        printf("Testing item %d / %d...  ", i + 1, test_size);
        printf("\r"); // Move cursor to the start of the line
        fflush(stdout);
    }
    printf("\nEvaluation complete.\n");
    // 5. Calculate and display accuracy
    print_results("KNN",correct_prediction_knn,test_size);
    print_results("Perfect",  correct_prediction_perfect,  test_size);
    print_results("Imperfect", correct_prediction_imperfect, test_size);
    
    // 6. Run AI vs AI simulations (e.g. 200 games for each matchup)
    simulate_matchups(train_set, train_size, 1000);

    // 7. Clean up
    free(all_data);
    printf("\nCleaned up memory. Program finished.\n");

    return 0;
}


// --- Core KNN and Helper Functions (Unchanged) ---
void shuffle_dataset(DataPoint* dataset, int n) {
    if (n > 1) {
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            DataPoint temp = dataset[i];
            dataset[i] = dataset[j];
            dataset[j] = temp;
        }
    }
}



// 0 = empty, 1 = X (AI), -1 = O (HUMAN) based on your CSV
static char int_to_symbol(int v) {
    if (v == 0)  return ' ';
    if (v == 1)  return AI;     // 'X'
    if (v == -1) return HUMAN;  // 'O'
    return ' ';
}

static void int9_to_char33(const int board9[9], char b[3][3]) {
    for (int i = 0; i < 9; i++) {
        int r = i / 3;
        int c = i % 3;
        if (board9[i] == 0)      b[r][c] = ' ';
        else if (board9[i] == 1) b[r][c] = 'X';
        else if (board9[i] == -1)b[r][c] = 'O';
    }
}

// This function ONLY returns the predicted move (1..9)
int minimax_prediction(const int board9[9], int mode) {
    char b[3][3];
    // Convert 1D int[9] → 2D char[3][3] that aiTurn understands
    int9_to_char33(board9, b);
    // Call your existing AI of minimax based on mode 
    int move = aiTurn(b, mode);   // mode 4 = perfect, 2 = imperfect
    return move;                  // 1..9
}


// Map int cell to 'X'/'O'/' ' from the perspective of "player"
// player: 1 = X, -1 = O
static char int_to_symbol_for_player(int cell, int player) {
    if (cell == 0) return ' ';

    if (player == 1) { // X is current player
        if (cell == 1)   return AI;    // me as 'X'
        else             return HUMAN; // opponent as 'O'
    } else {            // player == -1, O is current player
        if (cell == -1)  return AI;    // me as 'X' in transformed board
        else             return HUMAN; // opponent as 'O'
    }
}

// Minimax for whichever side is to move: player = 1 (X) or -1 (O)
int minimax_prediction_player(const int board9[9], int mode, int player) {
    char b[3][3];

    for (int i = 0; i < 9; i++) {
        int r = i / 3;
        int c = i % 3;
        b[r][c] = int_to_symbol_for_player(board9[i], player);
    }

    int move = aiTurn(b, mode);  // same aiTurn as your 1P.c (returns 1..9)
    return move;
}

// Check if given player (1 or -1) has 3 in a row on int[9] board
int check_win_intboard(const int board9[9], int player) {
    char b[3][3];
    int winLine[3] = {-1, -1, -1};

    // Convert int[9] -> char[3][3] using our adapter
    int9_to_char33(board9, b);

    // Use your existing 2P logic
    char w = checkWin(b, winLine);   // returns 'X', 'O', or 0

    if (player == 1 && w == 'X') return 1;   // X (1) wins
    if (player == -1 && w == 'O') return 1;  // O (-1) wins
    return 0;
}

int board_full_int(const int board9[9]) {
    for (int i = 0; i < 9; i++) {
        if (board9[i] == 0) return 0;
    }
    return 1;
}

static void print_results(const char *label, int correct, int total) {
    double accuracy = total ? (double)correct / total * 100.0 : 0.0;
    printf("\n--- %s Evaluation ---\n", label);
    printf("Total test items:    %d\n", total);
    printf("Correct predictions: %d\n", correct);
    printf("Model Accuracy:      %.2f%%\n", accuracy);
    printf("--------------------------\n");
}

int choose_move(PlayerType type, int current_player,
                const int board9[9], DataPoint* train_set, int train_size) {
    switch (type) {
        case PLAYER_MINIMAX_PERFECT:
            return minimax_prediction_player(board9, 4, current_player);
        case PLAYER_MINIMAX_IMPERFECT:
            return minimax_prediction_player(board9, 2, current_player);
        case PLAYER_KNN: {
            int move = knn_predict_move(train_set, train_size, (int*)board9);
            int idx  = move - 1;

            // Fallback only for KNN
            if (move < 1 || move > 9 || board9[idx] != 0) {
                move = -1;
                for (int i = 0; i < 9; i++) {
                    if (board9[i] == 0) {
                        move = i + 1;
                        break;
                    }
                }
            }
            return move;
        }
        default:
            return -1;
    }
}

char simulate_game(PlayerType playerX, PlayerType playerO,
                   DataPoint* train_set, int train_size) {
    int board9[9] = {0};   // 0 = empty, 1 = X, -1 = O
    int current = 1;       // start with X
    int moves = 0;

    while (1) {
        PlayerType type = (current == 1) ? playerX : playerO;

        int move = choose_move(type, current, board9, train_set, train_size);
        if (move < 1 || move > 9) {
            return 'E';
        }

        int idx = move - 1;
        if (board9[idx] != 0) {
            // tried to play on occupied cell → treat as error
            return 'E';
        }

        board9[idx] = current;
        moves++;

        if (check_win_intboard(board9, current)) {
            return (current == 1) ? 'X' : 'O';
        }
        if (board_full_int(board9)) {
            return 'D';
        }

        current = -current;  // switch player
    }
}

static void simulate_pair(const char *label,
                          PlayerType A, PlayerType B,
                          DataPoint *train_set, int train_size,
                          int num_games)
{
    int half = num_games / 2;  // first half: A starts, second half: B starts
    int a_wins = 0, b_wins = 0, draws = 0;
    char result;

    // --- First half: A = X, B = O ---
    for (int g = 0; g < half; g++) {
        result = simulate_game(A, B, train_set, train_size);
        if (result == 'X')      a_wins++;  // X is A
        else if (result == 'O') b_wins++;  // O is B
        else if (result == 'D') draws++;
    }

    // --- Second half: B = X, A = O ---
    for (int g = half; g < num_games; g++) {
        result = simulate_game(B, A, train_set, train_size);
        if (result == 'X')      b_wins++;  // X is B now
        else if (result == 'O') a_wins++;  // O is A now
        else if (result == 'D') draws++;
    }

    printf("\n=== %s ===\n", label);
    printf("Total games: %d\n", num_games);
    printf("A wins:      %d\n", a_wins);
    printf("B wins:      %d\n", b_wins);
    printf("Draws:       %d\n", draws);
    printf("A win rate:  %.2f%%\n", num_games ? 100.0 * a_wins / num_games : 0.0);
    printf("B win rate:  %.2f%%\n", num_games ? 100.0 * b_wins / num_games : 0.0);
}


void simulate_matchups(DataPoint* train_set, int train_size, int num_games) {
    // Minimax (Perfect) vs Minimax (Perfect)
    simulate_pair("Minimax (Perfect) vs Minimax (Perfect)  [A = Perfect, B = Perfect]",
                  PLAYER_MINIMAX_PERFECT,
                  PLAYER_MINIMAX_PERFECT,
                  train_set, train_size, num_games);

    // Minimax (Perfect) vs Minimax (Perfect)
    simulate_pair("Imperfect vs Imperfect  [A = Imperfect, B = Imperfect]",
                  PLAYER_MINIMAX_IMPERFECT,
                  PLAYER_MINIMAX_IMPERFECT,
                  train_set, train_size, num_games);
    
    // Minimax (Perfect) vs Minimax (Perfect)
    simulate_pair("KNN vs KNN  [A = KNN, B = KNN]",
                  PLAYER_KNN,
                  PLAYER_KNN,
                  train_set, train_size, num_games);
            
    // Minimax (Perfect) vs Imperfect
    simulate_pair("Minimax (Perfect) vs Imperfect  [A = Perfect, B = Imperfect]",
                  PLAYER_MINIMAX_PERFECT,
                  PLAYER_MINIMAX_IMPERFECT,
                  train_set, train_size, num_games);

    // KNN vs Imperfect
    simulate_pair("KNN vs Imperfect  [A = KNN, B = Imperfect]",
                  PLAYER_KNN,
                  PLAYER_MINIMAX_IMPERFECT,
                  train_set, train_size, num_games);

    // Minimax (Perfect) vs KNN
    simulate_pair("Minimax (Perfect) vs KNN  [A = Perfect, B = KNN]",
                  PLAYER_MINIMAX_PERFECT,
                  PLAYER_KNN,
                  train_set, train_size, num_games);
}


