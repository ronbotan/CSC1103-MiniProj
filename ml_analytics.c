#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "ML.h"
// #include <1P.h>

#define K 5
#define MAX_LINE_LEN 1024
#define TRAIN_SPLIT 0.8


typedef struct {
    double distance;
    int best_move;
} Neighbor;

// --- Forward Declarations ---
// DataPoint* knn_load_dataset(const char* filename, int* num_records);
void shuffle_dataset(DataPoint* dataset, int n);
int predict_knn(DataPoint* train_set, int train_size, int new_board[9]);
static void print_results(const char *label, int correct, int total, int time);

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
    double time_knn = 0, time_perfect = 0, time_imperfect = 0;

    for (int i = 0; i < test_size; i++) {
        int* current_test_board = test_set[i].board;
        int true_best_move = test_set[i].best_move;

        clock_t start = clock();
        int knn_move = predict_knn(train_set, train_size, current_test_board);
        time_knn += clock() - start;
        if (knn_move == true_best_move) correct_prediction_knn++;

        //Jane add your minimax logic, 80 train, 20 test
        // start = clock();
        // -------- add here ----------
        // time_perfect += clock() - start;
        // if (perfect_move == true_best_move) correct_prediction_perfect++;

        //Jane add your imperfect minimax logic, 80 train, 20 test
        // start = clock();
        // //variable = method
        // time_imperfect += clock() - start;
        // if (variableName == true_best_move) correct_prediction_imperfect++;

        // MODIFIED: Added spaces at the end to ensure the line is fully overwritten
        printf("Testing item %d / %d...  ", i + 1, test_size);
        printf("\r"); // Move cursor to the start of the line
        fflush(stdout);
    }
    printf("\nEvaluation complete.\n");
    // 5. Calculate and display accuracy
    print_results("KNN",correct_prediction_knn,test_size,time_knn);
    // print_results("Perfect",   correct_prediction_perfect,   test_size);
    // print_results("Imperfect", correct_prediction_imperfect, test_size);
    
    

    // 6. Clean up
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


int predict_knn(DataPoint* train_set, int train_size, int new_board[9]) {
    Neighbor* neighbors = (Neighbor*)malloc(train_size * sizeof(Neighbor));
    if (!neighbors) return -1;

    for (int i = 0; i < train_size; i++) {
        neighbors[i].distance = dist9(new_board, train_set[i].board);
        neighbors[i].best_move = train_set[i].best_move;
    }

    qsort(neighbors, train_size, sizeof(Neighbor), cmp_neighbors);

    int votes[10] = {0};
    for (int i = 0; i < K; i++) {
        if (i < train_size) {
            votes[neighbors[i].best_move]++;
        }
    }

    int max_votes = -1;
    int predicted_move = -1;
    for (int i = 1; i <= 9; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            predicted_move = i;
        }
    }

    free(neighbors);
    return predicted_move;
}


static void print_results(const char *label, int correct, int total,int time) {
    double accuracy = total ? (double)correct / total * 100.0 : 0.0;
    double avg_ms = total ? (time / CLOCKS_PER_SEC) * 1000.0 / total : 0.0;
    printf("\n--- %s Evaluation ---\n", label);
    printf("Total test items:    %d\n", total);
    printf("Correct predictions: %d\n", correct);
    printf("Model Accuracy:      %.2f%%\n", accuracy);
    printf("Model Time Performance:      %.2f%%\n", time);
    printf("--------------------------\n");
}
