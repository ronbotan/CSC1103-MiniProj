#include <stdio.h>
#include "knn.h"
#include "gnuplot.h"        // reuse existing draw/init/check helpers
#include "2P.h"             // for checkWin/checkFull if not in a header

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

    char player = 'O'; // human first
    while (1) {
        draw(board, winner, winLine);

        if (player == 'O') {
            int input;
            printf("Player O, choose 1-9: ");
            if (scanf("%d", &input) != 1 || input < 1 || input > 9) {
                printf("Invalid input.\n");
                while (getchar() != '\n');
                continue;
            }
            int r = (input - 1) / 3;
            int c = (input - 1) % 3;
            if (board[r][c] != ' ') {
                printf("Cell already taken.\n");
                continue;
            }
            board[r][c] = 'O';
        } else {
            int move = pick_knn_move(board, data, record_count);
            if (move == -1) break;
            int r = (move - 1) / 3;
            int c = (move - 1) % 3;
            if (board[r][c] != ' ') {
                int found = 0;
                for (int idx = 0; idx < 9; ++idx) {
                    int rr = idx / 3, cc = idx % 3;
                    if (board[rr][cc] == ' ') {
                    r = rr;
                    c = cc;
                    found = 1;
                    break;
                    }
                }
                if (!found) break;
            }
            board[r][c] = 'X';
            printf("AI plays %d\n", move);
        }

        if (checkWin(board, winLine)) {
            winner = player;
            draw(board, winner, winLine);
            break;
        }
        if (checkFull(&board[0][0])) {
            winner = 'D';
            draw(board, winner, winLine);
            break;
        }
        player = (player == 'O') ? 'X' : 'O';
    }

    close_gnuplot();
    knn_free_dataset(data);
    return winner;
}
