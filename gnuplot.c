#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "gnuplot.h"

// used to represent unoccupied cells
#define EMPTY ' '

// 3x3 tic tac toe grid
char board[3][3];
char winner = EMPTY; 

// ensure that the gnuplot window stays open to be reused
static FILE *gp = NULL;

// function to open gnuplot pipe
int init_gnuplot(void)
{
    if (gp != NULL) 
        return 1;
    // opens a gnuplot pipe to write data out
    gp = _popen("gnuplot", "w");
        return gp != NULL;
}

// close gnuplot pipe
void close_gnuplot(void)
{
    if (gp) {
        fprintf(gp, "pause 3\n");
        fprintf(gp, "exit\n");
        fflush(gp);
        _pclose(gp);
        gp = NULL;
    }
}

// draw out my gnuplot
void draw(char board[3][3], char winner, int winLine[3], const char *title)
{

     // send title to gnuplot
    if (title && title[0]) {
        fprintf(gp, "set title \"%s\" font \"Arial,12\"\n", title);
    } else {
        fprintf(gp, "set title \"\"\n");
    }

    if (!gp) return;
    fprintf(gp, "unset object\n");
    fprintf(gp, "unset arrow\n");
    fprintf(gp, "unset label\n");
    // set up the grid
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

    // to label each cell and draw my X and O
    for (int i = 0; i < 9; i++) {
        int r = i/3;
        int c = i%3;
        double x = c + 0.5;
        double y = r + 0.5;
        int cellNumber = i + 1;

        // label each box in its top-left corner
        fprintf(gp,
            "set label \"%d\" at %f,%f front font \"Arial,10\" tc rgb 0x666666\n",
            cellNumber, x - 0.35, y - 0.35);

        // draw my X in the cell
        if (board[r][c] == 'X') {
            fprintf(gp, "set arrow from %f-0.3,%f-0.3 to %f+0.3,%f+0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
            fprintf(gp, "set arrow from %f-0.3,%f+0.3 to %f+0.3,%f-0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
        }
        // draw my O in the cell
        else if (board[r][c] == 'O') {
            fprintf(gp,"set object circle at %f,%f size 0.30 front fs empty " "border lc rgb 0x0000FF lw 6\n", x,y);
        }
    }

    // drawing the winning line if there is a winner
    if (winner == 'X' || winner == 'O') {
        // assign A and B as the 2 points for the line. Then draw the line from A to B
        int a = winLine[0], b = winLine[2]; 
        int ar=a/3, ac=a%3;
        int br=b/3, bc=b%3;

        // get the coordinates for the center of each cell
        double x1 = ac + 0.5, y1 = ar + 0.5;
        double x2 = bc + 0.5, y2 = br + 0.5;

        // calculate direction vector and normalize it
        double dx = x2 - x1, dy = y2 - y1;
        double len = sqrt(dx*dx + dy*dy);
        dx /= len; dy /= len;
        // extend the line a bit beyond the centers
        x1 -= dx * 0.35;
        x2 += dx * 0.35;
        y1 -= dy * 0.35;
        y2 += dy * 0.35;
        // draw the winning line in green
        fprintf(gp,
            "set arrow from %f,%f to %f,%f lw 8 lc rgb 0x00FF00 nohead front\n",
            x1,y1,x2,y2);
    }

    fprintf(gp, "plot NaN notitle\n");
    // flush the gnuplot pipe to ensure all commands are sent
    fflush(gp);
}