#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

/* ---- Board symbols ---- */
#define EMPTY ' '
#define X     'X'
#define O     'O'

typedef enum { MODE_1P=1, MODE_2P=2 } game_mode_t;
typedef enum { DIFF_EASY=1, DIFF_MED=2, DIFF_HARD=3 } difficulty_t;

/* 3×3 Tic Tac Toe grid */
static char board[3][3];

static char turn = X;
static char winner = EMPTY;   // 'X', 'O', ' ', or 'D' (draw)
static int winLine[3] = {-1, -1, -1};

static game_mode_t mode_choice = MODE_2P;
static difficulty_t diff_choice = DIFF_EASY;
static char human = X;
static char ai    = O;


/* ---------- WIN CHECK ---------- */
static char CheckWinner(char b[3][3], int wline[3])
{
    // Check rows
    for (int r = 0; r < 3; r++) {
        if (b[r][0] != ' ' && b[r][0] == b[r][1] && b[r][1] == b[r][2]) {
            if (wline) { wline[0] = r*3+0; wline[1] = r*3+1; wline[2] = r*3+2; }
            return b[r][0];  // 'X' or 'O'
        }
    }

    // Check columns
    for (int c = 0; c < 3; c++) {
        if (b[0][c] != ' ' && b[0][c] == b[1][c] && b[1][c] == b[2][c]) {
            if (wline) { wline[0] = 0*3+c; wline[1] = 1*3+c; wline[2] = 2*3+c; }
            return b[0][c];
        }
    }

    // Check main diagonal
    if (b[1][1] != ' ' && b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        if (wline) { wline[0] = 0; wline[1] = 4; wline[2] = 8; }
        return b[1][1];
    }

    // Check anti-diagonal
    if (b[1][1] != ' ' && b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        if (wline) { wline[0] = 2; wline[1] = 4; wline[2] = 6; }
        return b[1][1];
    }

    // Check for draw
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (b[r][c] == ' ')
                return ' ';  // game still in progress

    return 'D';  // draw
}



/* ---------- DRAW USING GNUPLOT ---------- */
void draw(FILE *gp, char board[3][3])
{
    fprintf(gp, "unset object\n");
    fprintf(gp, "unset arrow\n");

    fprintf(gp,
        "unset key;"
        "set size square;"
        "set xrange [0:3];"
        "set yrange [0:3];"
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

        if (board[r][c] == X) {
            fprintf(gp, "set arrow from %f-0.3,%f-0.3 to %f+0.3,%f+0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
            fprintf(gp, "set arrow from %f-0.3,%f+0.3 to %f+0.3,%f-0.3 nohead lw 6 lc rgb 0xFF0000\n", x,y,x,y);
        }
        else if (board[r][c] == O) {
            fprintf(gp,
                "set object circle at %f,%f size 0.30 front fs empty "
                "border lc rgb 0x0000FF lw 6\n", x,y);
        }
    }

    /* Winning line */
    if (winner == X || winner == O) {
        int a = winLine[0], b = winLine[2];
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


/* ---------- Utility ---------- */
static void reset_board(void)
{
    for (int r=0; r<3; r++)
        for (int c=0; c<3; c++)
            board[r][c] = EMPTY;

    winner = EMPTY;
    winLine[0]=winLine[1]=winLine[2] = -1;
    turn = X;
}

static char next_turn(char t) { return (t==X) ? O : X; }


/* ---------- AI Helpers ---------- */
static int find_winning_move(char who)
{
    for (int i=0; i<9; i++) {
        int r=i/3, c=i%3;
        if (board[r][c] != EMPTY) continue;
        board[r][c] = who;
        char w = CheckWinner(board, NULL);
        board[r][c] = EMPTY;
        if (w == who) return i;
    }
    return -1;
}

static int random_move(void)
{
    int list[9], n=0;
    for (int i=0;i<9;i++)
        if (board[i/3][i%3] == EMPTY)
            list[n++] = i;
    if (n==0) return -1;
    return list[rand()%n];
}


/* ---------- Minimax ---------- */
static int minimax_score(char player)
{
    char w = CheckWinner(board, NULL);
    if (w == ai) return +10;
    if (w == human) return -10;
    if (w == 'D') return 0;

    int best = (player == ai ? -1000 : 1000);

    for (int i=0; i<9; i++) {
        int r=i/3, c=i%3;
        if (board[r][c] != EMPTY) continue;

        board[r][c] = player;
        int sc = minimax_score(next_turn(player));
        board[r][c] = EMPTY;

        if (player == ai) {
            if (sc > best) best = sc;
        } else {
            if (sc < best) best = sc;
        }
    }
    return best;
}

static int best_minimax_move(void)
{
    int bestScore = -1000;
    int move = -1;

    for (int i=0; i<9; i++) {
        int r=i/3, c=i%3;
        if (board[r][c] != EMPTY) continue;

        board[r][c] = ai;
        int sc = minimax_score(human);
        board[r][c] = EMPTY;

        if (sc > bestScore) {
            bestScore = sc;
            move = i;
        }
    }
    return move;
}

static int ai_pick_move(void)
{
    if (diff_choice == DIFF_EASY)
        return random_move();

    if (diff_choice == DIFF_MED) {
        int m = find_winning_move(ai);
        if (m != -1) return m;
        m = find_winning_move(human);
        if (m != -1) return m;
        return random_move();
    }

    /* Hard */
    int m = find_winning_move(ai);
    if (m!=-1) return m;
    m = find_winning_move(human);
    if (m!=-1) return m;
    return best_minimax_move();
}


/* ---------- Menu ---------- */
static long read_long_choice(const char *p, long lo, long hi)
{
    char buf[64];
    while (1) {
        if (p) fputs(p, stdout);
        if (!fgets(buf, 64, stdin)) return lo;
        long v = strtol(buf, NULL, 10);
        if (v >= lo && v <= hi) return v;
        printf("Please enter %ld–%ld.\n", lo, hi);
    }
}

static void show_menu(void)
{
    printf("=== Tic Tac Toe ===\n");
    printf("1) 1 Player\n");
    printf("2) 2 Players\n");

    mode_choice = read_long_choice("Choose: ",1,2);

    if (mode_choice == MODE_1P) {
        printf("\nDifficulty:\n");
        printf("1) Easy\n");
        printf("2) Medium\n");
        printf("3) Hard\n");
        diff_choice = read_long_choice("Choose: ",1,3);
        human = X;
        ai = O;
    }

    printf("\nX goes first. Enter 1 to 9.\n\n");
}


/* ---------- MAIN ---------- */
int poop(void)
{
    srand((unsigned)time(NULL));

    FILE *gp = _popen("gnuplot -persist", "w");
    if (!gp) {
        printf("Failed to open gnuplot\n");
        return 1;
    }

    show_menu();
    reset_board();
    draw(gp, board);

    char buf[64];

    while (fgets(buf, 64, stdin)) {

        char *p = buf;
        while (*p==' '||*p=='\t') p++;
        if (*p=='q' || *p=='Q') break;

        /* AI MOVE */
        if (mode_choice==MODE_1P && turn==ai) {
            int mv = ai_pick_move();
            if (mv >= 0) {
                int r=mv/3, c=mv%3;
                board[r][c] = ai;
            }
            winner = CheckWinner(board, winLine);
            draw(gp, board);
            if (winner!=' ') {
                if (winner=='D') printf("Draw!\n");
                else printf("%c wins!\n", winner);
                break;
            }
            turn = next_turn(turn);
            printf("Enter move (1–9): ");
            continue;
        }

        /* HUMAN MOVE */
        long v = strtol(p,NULL,10);
        if (v < 1 || v > 9) {
            printf("Enter 1–9 or q.\n");
            continue;
        }

        int idx = (int)v - 1;
        int r = idx/3, c = idx%3;

        if (board[r][c] != EMPTY) {
            printf("Cell filled. Try again.\n");
            continue;
        }

        board[r][c] = turn;

        winner = CheckWinner(board, winLine);
        draw(gp, board);
        if (winner!=' ') {
            if (winner=='D') printf("Draw!\n");
            else printf("%c wins!\n", winner);
            break;
        }

        turn = next_turn(turn);

        if (mode_choice==MODE_2P ||
            (mode_choice==MODE_1P && turn==human))
        {
            printf("Enter move (1 to 9): ");
        }
    }

    _pclose(gp);
    return 0;
}
