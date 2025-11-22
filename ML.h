#ifndef ML1P_H
#define ML1P_H

#define KNN_K 5
#define KNN_MAX_LINE 256

typedef struct {
    int board[9];      // -1=O, 0=empty, 1=X
    int best_move;     // 1..9
} DataPoint;

DataPoint* knn_load_dataset(const char* filename, int* out_count);
void       knn_free_dataset(DataPoint* data);
int        knn_predict_move(const DataPoint* data, int count,
                            const int board[9]);
double dist9(const int a[9], const int b[9]); 
int cmp_neighbors(const void* lhs, const void* rhs);
#endif
