#include "lvgl.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern const lv_font_t lv_font_montserrat_30;

#define N_CELLS 9

/* --- Game mode and difficulty enums --- */
typedef enum {
    MODE_NONE = 0,
    MODE_1P,
    MODE_2P
} mode_t;

typedef enum {
    DIFF_NONE = 0,
    DIFF_EASY,
    DIFF_HARD
} difficulty_t;

/* --- App structure --- */
typedef struct app_t {
    lv_obj_t *cell_btns[N_CELLS];
    lv_obj_t *cell_labels[N_CELLS];
    lv_obj_t *status_label;
    char board[N_CELLS];
    char current;
    bool game_over;
    mode_t mode;
    difficulty_t difficulty;

    char player1_name[32];
    char player2_name[32];
    lv_obj_t *input_p1;
    lv_obj_t *input_p2;

    lv_style_t style_cell;
    lv_style_t style_win;
    lv_style_t style_x;
    lv_style_t style_o;
} app_t;

/* --- Winning lines --- */
static const int WIN_LINES[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
};

/* --- Forward declarations --- */
static void create_login_screen(app_t *app, lv_obj_t *parent);
static void create_homepage(app_t *app, lv_obj_t *parent);
static void create_difficulty_screen(app_t *app, lv_obj_t *parent);
static void create_game(app_t *app, lv_obj_t *parent);

/* --- Utility functions --- */
static void update_status(app_t *app) {
    char buf[64];
    if(app->game_over) return;
    snprintf(buf, sizeof(buf), "Turn: %s (%c)",
             (app->current=='X') ? app->player1_name : app->player2_name, app->current);
    lv_label_set_text(app->status_label, buf);
}

static char check_winner(const char b[9], int *win_line_idx) {
    for (int i = 0; i < 8; ++i) {
        int a = WIN_LINES[i][0], c = WIN_LINES[i][1], d = WIN_LINES[i][2];
        if (b[a] && b[a] == b[c] && b[c] == b[d]) {
            if (win_line_idx) *win_line_idx = i;
            return b[a];
        }
    }
    return '\0';
}

static bool board_full(const char b[9]) {
    for (int i = 0; i < N_CELLS; ++i)
        if (!b[i]) return false;
    return true;
}

static void set_cells_enabled(app_t *app, bool en) {
    for (int i = 0; i < N_CELLS; ++i) {
        if (en) lv_obj_clear_state(app->cell_btns[i], LV_STATE_DISABLED);
        else    lv_obj_add_state(app->cell_btns[i], LV_STATE_DISABLED);
    }
}

static void clear_win_highlight(app_t *app) {
    for (int i = 0; i < N_CELLS; ++i)
        lv_obj_remove_style(app->cell_btns[i], &app->style_win, 0);
}

/* --- Game Over → Home --- */
static void on_gameover_ok(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);

    lv_obj_t *ok_btn  = lv_event_get_target(e);
    lv_obj_t *footer  = lv_obj_get_parent(ok_btn);
    lv_obj_t *mbox    = lv_obj_get_parent(footer);
    lv_msgbox_close(mbox);

    lv_obj_clean(lv_scr_act());
    create_homepage(app, lv_scr_act());
}

static void show_gameover_and_home(app_t *app, const char *msg) {
    lv_obj_t *mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(mbox, "Game Over");
    lv_msgbox_add_text(mbox, msg);

    lv_obj_t *ok = lv_msgbox_add_footer_button(mbox, "Home");
    lv_obj_add_event_cb(ok, on_gameover_ok, LV_EVENT_CLICKED, app);
    lv_obj_center(mbox);
}

/* --- Highlight win --- */
static void highlight_line(app_t *app, int line_idx) {
    for (int k = 0; k < 3; ++k) {
        int i = WIN_LINES[line_idx][k];
        lv_obj_add_style(app->cell_btns[i], &app->style_win, 0);
    }
}

/* --- Reset --- */
static void reset_game(app_t *app) {
    for (int i = 0; i < N_CELLS; ++i) {
        app->board[i] = '\0';
        lv_label_set_text(app->cell_labels[i], "");
        lv_obj_clear_state(app->cell_btns[i], LV_STATE_DISABLED);
    }
    clear_win_highlight(app);
    app->current = 'X';
    app->game_over = false;
    lv_label_set_text(app->status_label, "Turn: Player 1 (X)");
}

/* --- Gameplay --- */
static void on_cell_clicked(lv_event_t *e) {
    app_t *app = (app_t*)lv_event_get_user_data(e);
    if(app->game_over) return;

    lv_obj_t *btn = lv_event_get_target(e);
    int idx = -1;
    for (int i = 0; i < N_CELLS; ++i)
        if (app->cell_btns[i] == btn) { idx = i; break; }

    if (idx < 0 || app->board[idx]) return;

    app->board[idx] = app->current;
    lv_label_set_text(app->cell_labels[idx], (app->current=='X') ? "X" : "O");
    lv_obj_add_style(app->cell_labels[idx],
        (app->current=='X') ? &app->style_x : &app->style_o, 0);
    lv_obj_add_state(app->cell_btns[idx], LV_STATE_DISABLED);

    int win_line = -1;
    char w = check_winner(app->board, &win_line);
    if (w) {
        char popup_txt[64];
        snprintf(popup_txt, sizeof(popup_txt), "%s (%c) wins!",
                 (w=='X') ? app->player1_name : app->player2_name, w);
        highlight_line(app, win_line);
        app->game_over = true;
        set_cells_enabled(app, false);
        show_gameover_and_home(app, popup_txt);
        return;
    }

    if (board_full(app->board)) {
        app->game_over = true;
        show_gameover_and_home(app, "It's a draw!");
        return;
    }

    app->current = (app->current=='X') ? 'O' : 'X';
    update_status(app);
}

/* --- Reset button --- */
static void on_reset_clicked(lv_event_t *e) {
    app_t *app = (app_t*)lv_event_get_user_data(e);
    reset_game(app);
}

/* --- Navigation button handlers --- */
static void on_easy_clicked(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);
    app->difficulty = DIFF_EASY;
    lv_obj_clean(lv_scr_act());
    create_game(app, lv_scr_act());
}

static void on_hard_clicked(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);
    app->difficulty = DIFF_HARD;
    lv_obj_clean(lv_scr_act());
    create_game(app, lv_scr_act());
}

static void on_1p_clicked(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);
    app->mode = MODE_1P;
    lv_obj_clean(lv_scr_act());
    create_difficulty_screen(app, lv_scr_act());
}

static void on_2p_clicked(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);
    app->mode = MODE_2P;
    app->difficulty = DIFF_NONE;
    lv_obj_clean(lv_scr_act());
    create_game(app, lv_scr_act());
}

/* --- Difficulty screen --- */
static void create_difficulty_screen(app_t *app, lv_obj_t *parent) {
    lv_obj_t *diff = lv_obj_create(parent);
    lv_obj_set_size(diff, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(diff, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(diff, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(diff, lv_color_hex(0xF8FAFC), 0);
    lv_obj_clear_flag(diff, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(diff);
    lv_label_set_text(title, "Select Difficulty");

    lv_obj_t *btn_easy = lv_btn_create(diff);
    lv_obj_set_size(btn_easy, 200, 60);
    lv_label_set_text(lv_label_create(btn_easy), "Easy");
    lv_obj_add_event_cb(btn_easy, on_easy_clicked, LV_EVENT_CLICKED, app);

    lv_obj_t *btn_hard = lv_btn_create(diff);
    lv_obj_set_size(btn_hard, 200, 60);
    lv_label_set_text(lv_label_create(btn_hard), "Hard");
    lv_obj_add_event_cb(btn_hard, on_hard_clicked, LV_EVENT_CLICKED, app);
}

/* --- Homepage --- */
static void create_homepage(app_t *app, lv_obj_t *parent) {
    lv_obj_t *home = lv_obj_create(parent);
    lv_obj_set_size(home, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(home, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(home, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(home, lv_color_hex(0xF8FAFC), 0);
    lv_obj_clear_flag(home, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(home);
    lv_label_set_text(title, "Tic Tac Toe");

    lv_obj_t *btn_1p = lv_btn_create(home);
    lv_obj_set_size(btn_1p, 200, 60);
    lv_label_set_text(lv_label_create(btn_1p), "1 Player");
    lv_obj_add_event_cb(btn_1p, on_1p_clicked, LV_EVENT_CLICKED, app);

    lv_obj_t *btn_2p = lv_btn_create(home);
    lv_obj_set_size(btn_2p, 200, 60);
    lv_label_set_text(lv_label_create(btn_2p), "2 Players");
    lv_obj_add_event_cb(btn_2p, on_2p_clicked, LV_EVENT_CLICKED, app);
}

/* --- Login screen --- */
static void on_login_start_clicked(lv_event_t *e) {
    app_t *app = (app_t *)lv_event_get_user_data(e);

    const char *p1 = lv_textarea_get_text(app->input_p1);
    const char *p2 = lv_textarea_get_text(app->input_p2);

    snprintf(app->player1_name, sizeof(app->player1_name), "%s", p1);
    snprintf(app->player2_name, sizeof(app->player2_name), "%s", p2);

    if (strlen(app->player1_name) == 0) strcpy(app->player1_name, "Player 1");
    if (strlen(app->player2_name) == 0) strcpy(app->player2_name, "Player 2");

    lv_obj_clean(lv_scr_act());
    create_homepage(app, lv_scr_act());
}

static void create_login_screen(app_t *app, lv_obj_t *parent) {
    lv_obj_t *login = lv_obj_create(parent);
    lv_obj_set_size(login, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(login, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(login, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(login, lv_color_hex(0xF1F5F9), 0);
    lv_obj_clear_flag(login, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(login);
    lv_label_set_text(title, "Enter Player Names");

    lv_obj_t *lbl1 = lv_label_create(login);
    lv_label_set_text(lbl1, "Player 1:");
    app->input_p1 = lv_textarea_create(login);
    lv_textarea_set_one_line(app->input_p1, true);
    lv_obj_set_width(app->input_p1, 200);

    lv_obj_t *lbl2 = lv_label_create(login);
    lv_label_set_text(lbl2, "Player 2:");
    app->input_p2 = lv_textarea_create(login);
    lv_textarea_set_one_line(app->input_p2, true);
    lv_obj_set_width(app->input_p2, 200);

    lv_obj_t *btn_start = lv_btn_create(login);
    lv_obj_set_size(btn_start, 200, 60);
    lv_label_set_text(lv_label_create(btn_start), "Start");
    lv_obj_add_event_cb(btn_start, on_login_start_clicked, LV_EVENT_CLICKED, app);
}

/* --- Game board --- */
static void create_game(app_t *app, lv_obj_t *parent) {
    app->current = 'X';
    app->game_over = false;
    for (int i = 0; i < N_CELLS; ++i) app->board[i] = '\0';

    lv_style_init(&app->style_cell);
    lv_style_set_min_width(&app->style_cell, 110);
    lv_style_set_min_height(&app->style_cell, 110);
    lv_style_set_bg_color(&app->style_cell, lv_color_make(245,245,245));
    lv_style_set_border_color(&app->style_cell, lv_color_make(205,205,205));
    lv_style_set_border_width(&app->style_cell, 2);

    lv_style_init(&app->style_win);
    lv_style_set_bg_color(&app->style_win, lv_color_make(214,245,214));

    lv_style_init(&app->style_x);
    lv_style_set_text_color(&app->style_x, lv_color_hex(0xE11D48));
    lv_style_set_text_font(&app->style_x, &lv_font_montserrat_30);

    lv_style_init(&app->style_o);
    lv_style_set_text_color(&app->style_o, lv_color_hex(0x2563EB));
    lv_style_set_text_font(&app->style_o, &lv_font_montserrat_30);

    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    char info[64];
    if (app->mode == MODE_1P)
        snprintf(info, sizeof(info), "%s (X) vs AI (O)", app->player1_name);
    else
        snprintf(info, sizeof(info), "%s (X) vs %s (O)", app->player1_name, app->player2_name);
    lv_obj_t *players = lv_label_create(root);
    lv_label_set_text(players, info);

    app->status_label = lv_label_create(root);
    lv_label_set_text(app->status_label, "Turn: Player 1 (X)");

    static int32_t col_dsc[4] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[4] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t *grid = lv_obj_create(root);
    lv_obj_set_size(grid, 420, 420);
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

    for (int i = 0; i < N_CELLS; ++i) {
        int r = i / 3, c = i % 3;
        lv_obj_t *btn = lv_btn_create(grid);
        lv_obj_add_style(btn, &app->style_cell, 0);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, c, 1,
                                   LV_GRID_ALIGN_STRETCH, r, 1);

        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, "");
        lv_obj_center(lbl);
        app->cell_btns[i] = btn;
        app->cell_labels[i] = lbl;
        lv_obj_add_event_cb(btn, on_cell_clicked, LV_EVENT_CLICKED, app);
    }

    lv_obj_t *controls = lv_obj_create(root);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(controls, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *reset = lv_btn_create(controls);
    lv_label_set_text(lv_label_create(reset), "Reset");
    lv_obj_add_event_cb(reset, on_reset_clicked, LV_EVENT_CLICKED, app);
}

/* --- Entry point --- */
void create_ui(lv_obj_t *parent) {
    static app_t app;
    app.mode = MODE_NONE;
    app.difficulty = DIFF_NONE;
    strcpy(app.player1_name, "");
    strcpy(app.player2_name, "");
    create_login_screen(&app, parent);
}
