#include <stdio.h>
#include <string.h>

#define MAXPLAYERS 10

// Create a structure to link the score to player names
// Typedef struct Players stored to the Players to make it easier to access the structure variables in functions 
typedef struct PlayerInfo{
    char name[50];
    int win;
    int draw;
    int played;
} Players;

void readName(char player[], char p);
void trim(char *str);
void showScores(char label[], char scoreboard[]);
void updateDraw(char player1[], char player2[], char scoreboard[]);
void updateLoserScore(char loserName[], char scoreboard[]);
void updateWinnerScore(char winnerName[], char scoreboard[]);
void saveScores(Players players[], int count, char scoreboard[]);
void updateScores(char winner, char player1[], char player2[], char label[], char scoreboard[]);
void swapRoles(char player1[], char player2[]);
int readScores(Players players[], int maxPlayers, char scoreboard[]);

void readName(char player[], char p) {
    printf("\nEnter name for Player %c: ", p);
    fgets(player, 31, stdin); //Name of user can vary. We set the buffer space to 30 characters + 1 terminating null for flexibility
    trim(player); //Calling of trim function to remove empty spaces and newline character (\n) 
}

void trim(char *str) {
    //Removing of newline character picked up by fgets
    str[strcspn(str, "\n")] = '\0';

    //Removing of empty spaces at the end
    for (int i = strlen(str) - 1; i >= 0 && str[i] == ' '; i--)
        str[i] = '\0';
}

void swapRoles(char player1[], char player2[]) {
    char temp[50]; //Temporary array to hold player 1 (support longer names)
    strcpy(temp, player1); //Store player 1 into a tempArray
    strcpy(player1, player2); //Move player 2 to player 1 position
    strcpy(player2, temp); //Move tempArray (player 1) to player 2
}

void sortWins(Players players[], int count) {
    for (int i = 0; i < count - 1; i++) {
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

void updateScores(char winner, char player1[], char player2[], char label[], char scoreboard[]) {
    if (winner == 'O') { //Player 1 win
        updateWinnerScore(player1, scoreboard);
        updateLoserScore(player2, scoreboard);
    }
    else if (winner == 'X') {//Player 2 win
        updateWinnerScore(player2, scoreboard);
        updateLoserScore(player1, scoreboard);
    }
    else if (winner == 'D') { //DRAW
        updateDraw(player1, player2, scoreboard);
    }
    showScores(label,scoreboard); //Show scores after updating
}


//Load scores into an array of Player structs
int readScores(Players players[], int maxPlayers, char scoreboard[]) {
    FILE *fp = fopen(scoreboard, "r");
    if (fp == NULL) //No file of this name, break out
        return 0;

    int count = 0;
    char line[256];
    while (count < maxPlayers && fgets(line, sizeof(line), fp)) {
        //Expect file lines as tab-separated: name\twin\tdraw\tplayed
        //Name may contain spaces; we read up to 30 chars stopping at a tab.
        int w, d, p;
        if (sscanf(line, "%30[^\t]\t%d\t%d\t%d", players[count].name, &w, &d, &p) == 4) {
            players[count].win = w;
            players[count].draw = d;
            players[count].played = p;
            trim(players[count].name);
            count++;
        }
    }
    fclose(fp);
    return count;
}

//Write all players back to file
void saveScores(Players players[], int count, char scoreboard[]) {
    sortWins(players, count);

    FILE *fp = fopen(scoreboard, "w");
    if (fp == NULL) {
        printf("Error writing scoreboard.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        //Write as tab-separated texts so names can have spaces
        fprintf(fp, "%s\t%d\t%d\t%d\n",
            players[i].name,
            players[i].win,
            players[i].draw,
            players[i].played);
    }
    fclose(fp);
}

//Update or add a player’s score
void updateWinnerScore(char winnerName[], char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, MAXPLAYERS, scoreboard);

    for (int i = 0; i < count; i++) {
        if (strcmp(players[i].name, winnerName) == 0) {
            players[i].win++;
            players[i].played++;
            saveScores(players, count, scoreboard);
            return;
        }
    }

    //Add new player
    strcpy(players[count].name, winnerName);
    players[count].win = 1;
    players[count].draw = 0;
    players[count].played = 1;
    count++;
    saveScores(players, count, scoreboard);
}

void updateLoserScore(char loserName[], char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, MAXPLAYERS, scoreboard);

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

void updateDraw(char player1[], char player2[], char scoreboard[]) {
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

void showScores(char label[], char scoreboard[]) {
    Players players[MAXPLAYERS];
    int count = readScores(players, MAXPLAYERS, scoreboard);

    printf("\n  %s Current Scoreboard   \n", label);
    printf("%-30s | %-5s | %-5s | %-6s\n", "Name", "Wins", "Draws", "Games");
    printf("-----------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-30s | %-5d | %-5d | %-6d\n",
            players[i].name,
            players[i].win,
            players[i].draw,
            players[i].played);
    }
    printf("\n");
}

