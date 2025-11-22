#ifndef ONE_P_H
#define ONE_P_H


#define HUMAN 'O'
#define AI    'X'

char Run1P(char board[3][3], char* boardPtr, int difficulty);
int minimaxLogic(char b[3][3], int isAITurn);
int aiTurn(char b[3][3], int mode);
void humanTurn(char b[3][3]);
/*int hasEmpty(char b[3][3]);
int whoWins(char b[3][3]);
void getWinLine(char b[3][3], int winLine[3]);*/

#endif
