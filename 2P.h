#ifndef TWO_P_H
#define TWO_P_H

#define ROW 3
#define COLUMN 3

int  checkWin(char board[ROW][COLUMN], int winline[3]);
int  checkFull(char *boardPtr);
char Run2P(char board[ROW][COLUMN], char *boardPtr, char player);

#endif
