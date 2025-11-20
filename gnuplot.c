#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "gnuplot.h"

/* ---- Board symbols ---- */
#define EMPTY ' '

/* 3×3 Tic Tac Toe grid */
char board[3][3];
char winner = EMPTY;   // 'X', 'O', ' ', or 'D' (draw)

/* file handle for gnuplot pipe is owned by this module */
static FILE *gp = NULL;

/* Open gnuplot pipe. Returns 1 on success, 0 on failure. */
int init_gnuplot(void)
{
    if (gp != NULL) return 1; /* already open */
    gp = _popen("gnuplot -persist", "w");
    return gp != NULL;
}

/* Close gnuplot pipe if open. */
void close_gnuplot(void)
{
    if (gp) {
        _pclose(gp);
        gp = NULL;
    }
}

/* ---------- DRAW USING GNUPLOT ---------- */
void draw(char board[3][3], char winner, int winLine[3], const char *title)
{

     // send title to gnuplot
    if (title && title[0]) {
        fprintf(gp, "set title \"%s\" font \"Arial,12\"\n", title);
    } else {
        fprintf(gp, "set title \"\"\n");
    }

    if (!gp) return; /* nothing to draw to */
    fprintf(gp, "unset object\n");
    fprintf(gp, "unset arrow\n");
    fprintf(gp, "unset label\n");

    fprintf(gp,
        "unset key;"
        "set size square;"
        "set xrange [0:3];"
        "set yrange [3:0];"
        "unset xtics;"
        "unset ytics;"
        "unset border;"
        "set style line 1 lc rgb 0x000000 lw 3;\n"
        "set arrow from 1,0 to 1,3 nohead ls 1;\n"
        "set arrow from 2,0 to 2,3 nohead ls 1;\n"
        "set arrow from 0,1 to 3,1 nohead ls 1;\n"
        "set arrow from 0,2 to 3,2 nohead ls 1;\n"
    );

    /* draw X and O */
    for (int i = 0; i < 9; i++) {
        int r = i/3;
        int c = i%3;
        double x = c + 0.5;
        double y = r + 0.5;
        int cellNumber = i + 1;

        // Label each box in its top-left corner
        fprintf(gp,
            "set label \"%d\" at %f,%f front font \"Arial,10\" tc rgb 0x666666\n",
            cellNumber, x - 0.35, y - 0.35);

        if (board[r][c] == 'X') {
            fprintf(gp, "set arrow from %f-0.3,%f-0.3 to %f+0.3,%f+0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
            fprintf(gp, "set arrow from %f-0.3,%f+0.3 to %f+0.3,%f-0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
        }
        else if (board[r][c] == 'O') {
            fprintf(gp,"set object circle at %f,%f size 0.30 front fs empty " "border lc rgb 0x0000FF lw 6\n", x,y);
        }
    }

    /* Winning line */
    if (winner == 'X' || winner == 'O') {
        int a = winLine[0], b = winLine[2]; //Assign A and B as the 2 points for the line. Then draw the line from A to B
        int ar=a/3, ac=a%3;
        int br=b/3, bc=b%3;

        double x1 = ac + 0.5, y1 = ar + 0.5;
        double x2 = bc + 0.5, y2 = br + 0.5;

        double dx = x2 - x1, dy = y2 - y1;
        double len = sqrt(dx*dx + dy*dy);
        dx /= len; dy /= len;

        x1 -= dx * 0.35;
        x2 += dx * 0.35;
        y1 -= dy * 0.35;
        y2 += dy * 0.35;

        fprintf(gp,
            "set arrow from %f,%f to %f,%f lw 8 lc rgb 0x00FF00 nohead front\n",
            x1,y1,x2,y2);
    }

    fprintf(gp, "plot NaN notitle\n");
    fflush(gp);
}

/*
void reset_board(void)
{
    for (int r=0; r<3; r++)
        for (int c=0; c<3; c++)
            board[r][c] = EMPTY;

    winner = EMPTY;
    winLine[0]=winLine[1]=winLine[2] = -1;
}*/