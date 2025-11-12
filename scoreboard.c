#include <stdio.h>
#include <string.h>

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
    printf("%-10s | %-5s | %-5s | %-6s\n", "Name", "Wins", "Draws", "Games");
    printf("-----------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10s | %-5d | %-5d | %-6d\n",
            players[i].name,
            players[i].win,
            players[i].draw,
            players[i].played);
    }
    printf("\n");
}

void readNames() {
    static char player1[50], player2[50];
    printf("Enter name for Player X: ");
    fgets(player1, 50, stdin);
    printf("Enter name for Player O: ");
    fgets(player2, 50, stdin);
}

void trim(char *str) {
    // Remove trailing newline
    str[strcspn(str, "\r\n")] = '\0';

    // Remove trailing spaces
    for (int i = strlen(str) - 1; i >= 0 && str[i] == ' '; i--)
        str[i] = '\0';
}