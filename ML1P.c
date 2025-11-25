#include <stdio.h>
#include "ML.h"
#include "gnuplot.h"        // reuse existing draw/init/check helpers
#include "2P.h"             // for checkWin/checkFull if not in a header
#include "1P.h"
#include <time.h>   // add with the other includes
static void flatten_board(const char board[3][3], int out[9]) {
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) {
            char cell = board[r][c];
            out[r * 3 + c] = (cell == 'X') ? 1 :
                             (cell == 'O') ? -1 : 0;
        }
}

static int pick_knn_move(const char board[3][3],
                         const DataPoint* data, int count) {
    int flat[9];
    flatten_board(board, flat);
    return knn_predict_move(data, count, flat);
}

char Run1PKnn(char board[3][3], const char* dataset_path) {
    int record_count = 0;
    DataPoint* data = knn_load_dataset(dataset_path, &record_count);
    if (!data) {
        printf("Failed to load dataset.\n");
        return 'D';
    }

    static int winLine[3] = {-1, -1, -1};
    winner = ' ';
    if (!init_gnuplot()) {
        printf("Could not open gnuplot.\n");
        knn_free_dataset(data);
        return 'D';
    }

    char title[120];
    char player = 'O'; // human first
    snprintf(title, sizeof title, "Tic-Tac-Toe - 1P - Human's turn");
    while (1) {
        draw(board, winner, winLine, title);
        clock_t start = clock();
        if (player == 'O') {
            humanTurn(board);
        } else {
            int move = pick_knn_move(board, data, record_count);
            int r = (move - 1) / 3;
            int c = (move - 1) % 3;
            if (board[r][c] != ' ') {
                int fallback = -1;
                for (int idx = 0; idx < 9; ++idx) {
                    int rr = idx / 3, cc = idx % 3;
                    if (board[rr][cc] == ' ') {
                        r = rr;
                        c = cc;
                        fallback = idx + 1;
                        break;
                    }
                }
                if (fallback != -1) move = fallback; // update the index we’ll report
            }
            board[r][c] = 'X';
            printf("AI plays %d\n", move);
        }
        clock_t end = clock();
        double time_spent = 1000.0*(end - start) / CLOCKS_PER_SEC;
        if (player == 'O')
            printf("Human turn time: %.2f ms\n", time_spent);
        else
            printf("AI turn time: %.2f ms\n", time_spent);
        if (checkWin(board, winLine)) {
            winner = player;
            if (winner == 'O') {
                snprintf(title, sizeof title, "Tic-Tac-Toe - 1P - Human wins!");
                printf("\nHuman Wins!\n");  
            }else {
                snprintf(title, sizeof title, "Tic-Tac-Toe - 1P - AI wins!");
                printf("\nAI Wins!\n");  
            }
            draw(board, winner, winLine, title);
            break;
        }
        if (checkFull(&board[0][0])) {
            winner = 'D';
            snprintf(title, sizeof title, "Tic-Tac-Toe - 1P - Draw");
            printf("\n It's a Draw!\n");
            draw(board, winner, winLine, title);
            break;
        }
        player = (player == 'O') ? 'X' : 'O';
    }

    close_gnuplot();
    knn_free_dataset(data);
    return winner;
}
