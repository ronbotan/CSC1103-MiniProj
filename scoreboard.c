#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define MAXPLAYERS 10

// Create a structure to link the score to player names
// Typedef struct Players stored to the Players to make it easier to access the structure variables in functions 
typedef struct Players{
    char name[50];
    int win;
    int draw;
    int played;
} Players;

static void sort_by_wins_desc(Players players[], int count) {
    for (int i = 0; i < count - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < count; ++j) {
            if (players[j].win > players[best].win) {
                best = j;
            }
        }
        if (best != i) {
            Players tmp = players[i];
            players[i] = players[best];
            players[best] = tmp;
        }
    }
}

void readName(char player[], char p);
void trim(char *str);
void showScores(const char *label, char scoreboard[]);
void updateDraw(char *player1, char *player2, char scoreboard[]);
void updateLoserScore(char *loserName, char scoreboard[]);
void updatePlayerScore(char *winnerName, char scoreboard[]);
void saveScores(Players players[], int count, char scoreboard[]);
int readScores(Players players[], int maxPlayers, char scoreboard[]);
void updateScores(char winner, char player1[], char player2[],const char *label, char scoreboard[]);
void swapRoles(char player1[], char player2[]);

void updateScores(char winner, char player1[], char player2[],const char *label, char scoreboard[]) {
    if (winner == 'O') { //Player 1 win
        updatePlayerScore(player1, scoreboard);
        updateLoserScore(player2, scoreboard);
    }
    else if (winner == 'X') {//Player 2 win
        updatePlayerScore(player2, scoreboard);
        updateLoserScore(player1, scoreboard);
    }
    else if (winner == 'D') { //DRAW
        updateDraw(player1, player2, scoreboard);
    }
    showScores(label,scoreboard); //Show scores after updating
}


// Load scores into an array of Player structs
int readScores(Players players[], int maxPlayers, char scoreboard[]) {
    FILE *fp = fopen(scoreboard, "r");
    if (fp == NULL) // No file of this name, break out
        return 0; 

    int count = 0;
    while (count < maxPlayers && fscanf(fp, "%s %d %d %d",
           players[count].name,
           &players[count].win,
           &players[count].draw,
           &players[count].played) == 4) {count++;}
    fclose(fp);
    return count;
}

// Write all players back to file
void saveScores(Players players[], int count, char scoreboard[]) {
    sort_by_wins_desc(players, count);
    
    FILE *fp = fopen(scoreboard, "w");
    if (fp == NULL) {
        printf("Error writing scoreboard.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %d %d %d\n",
            players[i].name,
            players[i].win,
            players[i].draw,
            players[i].played);
    }
    fclose(fp);
}

// Update or add a player’s score
void updatePlayerScore(char *winnerName, char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100, scoreboard);

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, winnerName) == 0) {
            players[i].win++;
            players[i].played++;
            saveScores(players, count, scoreboard);
            return;
        }
    }

    // Add new player
    strcpy(players[count].name, winnerName);
    players[count].win = 1;
    players[count].draw = 0;
    players[count].played = 1;
    count++;
    saveScores(players, count, scoreboard);
}

void updateLoserScore(char *loserName, char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100, scoreboard);

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, loserName) == 0) {
            players[i].played++;
            saveScores(players, count, scoreboard);
            return;
        }
    }

    //Add new player with 0 win and 1 played.
    strcpy(players[count].name, loserName);
    players[count].win = 0;
    players[count].draw = 0;
    players[count].played = 1;
    count++;
    saveScores(players, count, scoreboard);
}

void updateDraw(char *player1, char *player2, char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, MAXPLAYERS, scoreboard);
    int found1 = 0, found2 = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, player1) == 0) {
            players[i].draw++;
            players[i].played++;
            found1 = 1;
        } else if (strcmp(players[i].name, player2) == 0) {
            players[i].draw++;
            players[i].played++;
            found2 = 1;
        }
    }

    if (!found1) {
        strcpy(players[count].name, player1);
        players[count].win = 0;
        players[count].draw = 1;
        players[count].played = 1;
        count++;
    }

    if (!found2) {
        strcpy(players[count].name, player2);
        players[count].win = 0;
        players[count].draw = 1;
        players[count].played = 1;
        count++;
    }

    saveScores(players, count, scoreboard);
}

void showScores(const char *label,char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100, scoreboard);

    printf("\n  %s Current Scoreboard   \n", label);
    printf("%-20s | %-5s | %-5s | %-6s\n", "Name", "Wins", "Draws", "Games");
    printf("-----------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s | %-5d | %-5d | %-6d\n",
            players[i].name,
            players[i].win,
            players[i].draw,
            players[i].played);
    }
    printf("\n");
}

void trim(char *str) {
    // Remove trailing newline
    str[strcspn(str, "\r\n")] = '\0';

    // Remove trailing spaces
    for (int i = strlen(str) - 1; i >= 0 && str[i] == ' '; i--)
        str[i] = '\0';
}

void readName(char player[], char p) {
    printf("\nEnter name for Player %c: ", p);
    fgets(player, 20, stdin); //fgets reads the entire string, allowing full names with spaces (e.g. Ron Tan)
    trim(player); //Run the trim function after reading using fgets to get rid of \n character
}

void swapRoles(char player1[], char player2[]) {
    char temp[20]; //Temporary array to hold player 1
    strcpy(temp, player1); //Store player 1 into a tempArray
    strcpy(player1, player2); //Move player 2 to player 1 position
    strcpy(player2, temp); //Move tempArray (player 1) to player 2
}