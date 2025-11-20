// gnuplot.h
#ifndef GNUPLOT_H
#define GNUPLOT_H

#define ROW 3
#define COLUMN 3

extern char winner;
extern int winLine[3];

int  init_gnuplot(void);
void close_gnuplot(void);
void draw(char board[ROW][COLUMN], char winner, int winLine[3], const char *title);

#endif
