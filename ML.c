#include "ML.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


double dist9(const int a[9], const int b[9]) {
    double sum = 0.0;
    for (int i = 0; i < 9; ++i) {
        double d = (double)a[i] - (double)b[i];
        sum += d * d;
    }
    return sqrt(sum);
}

int cmp_neighbors(const void* lhs, const void* rhs) {
    double da = ((const Neighbor*)lhs)->distance;
    double db = ((const Neighbor*)rhs)->distance;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

DataPoint* knn_load_dataset(const char* filename, int* out_count) {
    *out_count = 0;
    FILE* filepointer = fopen(filename, "r");
    if (!filepointer) {
        perror("Error: Could not open dataset file");
        return NULL;
    }

    char line[KNN_MAX_LINE];
    fgets(line, sizeof line, filepointer);           // header
    while (fgets(line, sizeof line, filepointer))
        (*out_count)++;

    if (*out_count == 0) {
        fclose(filepointer);
        return NULL;
    }

    DataPoint* data = calloc(*out_count,sizeof *data);
    if (!data) {
        fclose(filepointer);
        return NULL;
    }

    rewind(filepointer);
    fgets(line, sizeof line, filepointer);
    for (int i = 0; i < *out_count && fgets(line, sizeof line, filepointer); ++i) {
        sscanf(line,
               "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
               &data[i].board[0], &data[i].board[1], &data[i].board[2],
               &data[i].board[3], &data[i].board[4], &data[i].board[5],
               &data[i].board[6], &data[i].board[7], &data[i].board[8],
               &data[i].best_move);
    }
    fclose(filepointer);
    return data;
}

void knn_free_dataset(DataPoint* data) {
    free(data);
}

int knn_predict_move(const DataPoint* data, int count, const int board[9]) {
    if (!data || count == 0) return -1;

    Neighbor* buf = calloc(count,sizeof *buf);
    if (!buf) return -1;

    for (int i = 0; i < count; ++i) {
        buf[i].distance = dist9(board, data[i].board);
        buf[i].best_move = data[i].best_move;
    }

    qsort(buf, count, sizeof *buf, cmp_neighbors);

    int votes[10] = {0};
    for (int i = 0; i < KNN_K && i < count; ++i) {
        int mv = buf[i].best_move;
        if (mv >= 1 && mv <= 9) votes[mv]++;
    }

    int best_votes = -1;
    int best_move  = -1;
    for (int mv = 1; mv <= 9; ++mv) {
        if (votes[mv] > best_votes) {
            best_votes = votes[mv];
            best_move  = mv;
        }
    }

    free(buf);
    if (best_move == -1) {
        for (int i = 0; i < 9; ++i)
            if (board[i] == 0) return i + 1;
    }
    return best_move;
}
