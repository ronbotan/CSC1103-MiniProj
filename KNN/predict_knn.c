#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define K 5
#define MAX_LINE_LEN 1024
#define TRAIN_SPLIT 0.8

// --- Data Structures ---
typedef struct {
    int board[9];
    int best_move;
} DataPoint;

typedef struct {
    double distance;
    int best_move;
} Neighbor;

// --- Forward Declarations ---
DataPoint* load_dataset(const char* filename, int* num_records);
void shuffle_dataset(DataPoint* dataset, int n);
int predict_knn(DataPoint* train_set, int train_size, int new_board[9]);

// --- Main Program ---
int main(void) {
    srand(time(NULL));

    // 1. Load the dataset
    int num_records = 0;
    printf("Loading dataset from 'bestmoves.csv'...\n");
    DataPoint* all_data = load_dataset("bestmoves.csv", &num_records);
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

    int correct_predictions = 0;
    for (int i = 0; i < test_size; i++) {
        int* current_test_board = test_set[i].board;
        int true_best_move = test_set[i].best_move;

        int predicted_move = predict_knn(train_set, train_size, current_test_board);
        
        if (predicted_move == true_best_move) {
            correct_predictions++;
        }

        // MODIFIED: Added spaces at the end to ensure the line is fully overwritten
        printf("Testing item %d / %d...  ", i + 1, test_size);
        printf("\r"); // Move cursor to the start of the line
        fflush(stdout);
    }
    printf("\nEvaluation complete.\n");

    // 5. Calculate and display accuracy
    double accuracy = (double)correct_predictions / test_size * 100.0;
    printf("\n--- Evaluation Results ---\n");
    printf("Total test items:    %d\n", test_size);
    printf("Correct predictions: %d\n", correct_predictions);
    printf("Model Accuracy:      %.2f%%\n", accuracy);
    printf("--------------------------\n");

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

double euclidean_distance(int board1[9], int board2[9]) {
    double sum_sq_diff = 0.0;
    for (int i = 0; i < 9; i++) {
        sum_sq_diff += pow(board1[i] - board2[i], 2);
    }
    return sqrt(sum_sq_diff);
}

int compare_neighbors(const void* a, const void* b) {
    if (((Neighbor*)a)->distance < ((Neighbor*)b)->distance) return -1;
    if (((Neighbor*)a)->distance > ((Neighbor*)b)->distance) return 1;
    return 0;
}

int predict_knn(DataPoint* train_set, int train_size, int new_board[9]) {
    Neighbor* neighbors = (Neighbor*)malloc(train_size * sizeof(Neighbor));
    if (!neighbors) return -1;

    for (int i = 0; i < train_size; i++) {
        neighbors[i].distance = euclidean_distance(new_board, train_set[i].board);
        neighbors[i].best_move = train_set[i].best_move;
    }

    qsort(neighbors, train_size, sizeof(Neighbor), compare_neighbors);

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

DataPoint* load_dataset(const char* filename, int* num_records) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error: Could not open dataset file");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int count = 0;
    fgets(line, MAX_LINE_LEN, file);
    while (fgets(line, MAX_LINE_LEN, file)) count++;
    *num_records = count;

    DataPoint* dataset = (DataPoint*)malloc(count * sizeof(DataPoint));
    if (!dataset) {
        perror("Failed to allocate memory for dataset");
        fclose(file);
        return NULL;
    }

    rewind(file);
    fgets(line, MAX_LINE_LEN, file);
    int i = 0;
    while (fgets(line, MAX_LINE_LEN, file)) {
        sscanf(line, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            &dataset[i].board[0], &dataset[i].board[1], &dataset[i].board[2],
            &dataset[i].board[3], &dataset[i].board[4], &dataset[i].board[5],
            &dataset[i].board[6], &dataset[i].board[7], &dataset[i].board[8],
            &dataset[i].best_move);
        i++;
    }

    fclose(file);
    return dataset;
}