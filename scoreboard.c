#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define scoreboard "scoreboard.txt"
#define MAXPLAYERS 10

// Create a structure to link the score to player names
// Typedef struct Players stored to the Players to make it easier to access the structure variables in functions 
typedef struct Players{
    char name[50];
    int win;
    int draw;
    int played;
} Players;

void readName(char player[], char p);
void trim(char *str);
void showScores();
void updateDraw(char *player1, char *player2);
void updateLoserScore(char *loserName);
void updatePlayerScore(char *winnerName);
void saveScores(Players players[], int count);
int readScores(Players players[], int maxPlayers);
void updateScores(char winner, char player1[], char player2[]);

void updateScores(char winner, char player1[], char player2[]) {
    if (winner == 'O') { //Player 1 win
        updatePlayerScore(player1);
        updateLoserScore(player2);
    }
    else if (winner == 'X') {//Player 2 win
        updatePlayerScore(player2);
        updateLoserScore(player1);
    }
    else if (winner == 'D') { //DRAW
        updateDraw(player1, player2);
    }
    showScores(); //Show scores after updating
}


// Load scores into an array of Player structs
int readScores(Players players[], int maxPlayers) {
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
void saveScores(Players players[], int count) {
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
void updatePlayerScore(char *winnerName) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100);

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, winnerName) == 0) {
            players[i].win++;
            players[i].played++;
            saveScores(players, count);
            return;
        }
    }

    // Add new player
    strcpy(players[count].name, winnerName);
    players[count].win = 1;
    players[count].draw = 0;
    players[count].played = 1;
    count++;
    saveScores(players, count);
}

void updateLoserScore(char *loserName) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100);

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, loserName) == 0) {
            players[i].played++;
            saveScores(players, count);
            return;
        }
    }

    //Add new player with 0 win and 1 played.
    strcpy(players[count].name, loserName);
    players[count].win = 0;
    players[count].draw = 0;
    players[count].played = 1;
    count++;
    saveScores(players, count);
}

void updateDraw(char *player1, char *player2) {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100);
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

    saveScores(players, count);
}

void showScores() {
    Players players[MAXPLAYERS];
    int count = readScores(players, 100);

    printf("\n🏆 Current Scoreboard 🏆\n");
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

/*
void readNames() {
    static char player1[50], player2[50];
    printf("Enter name for Player X: ");
    fgets(player1, 50, stdin);
    printf("Enter name for Player O: ");
    fgets(player2, 50, stdin);
}
*/