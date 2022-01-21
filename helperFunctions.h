//
// Created by flimsy on 12/7/21.
//

#ifndef CHESSDATABASE_HELPERFUNCTIONS_H
#define CHESSDATABASE_HELPERFUNCTIONS_H

// Macro for error messages.
#define eprintf(...) fprintf(stderr, __VA_ARGS__)

// return values.
#define TRUE 1
#define FALSE 0
#define ERROR -1
#define BACK_TO_MENU -2
#define END_WHITE 1
#define END_BLACK 2
#define CONTINUE 0

// Size values.
#define NAME_MAX 50
#define DATE_MAX 9
#define RESULT_MAX 10
#define MOVES_MAX 150
#define S_MOVE_MAX 6

// Comparing/Array-position values.
#define WHITE_PLAYER 0
#define BLACK_PLAYER 1


typedef struct GameMoves {
    int moves_id;
    int move_number;
    char moves[MOVES_MAX][2][S_MOVE_MAX];
} GameMoves;

typedef struct GameInfo {
    int game_id;
    char name[NAME_MAX];
    char class[NAME_MAX];
    char group[NAME_MAX];
    char game_number[NAME_MAX];
    char date[DATE_MAX];
    char white_name[NAME_MAX];
    char black_name[NAME_MAX];
    char white_result[RESULT_MAX];
    char black_result[RESULT_MAX];
    GameMoves game_moves;
} GameInfo;

typedef struct SampleInfo {
    int id;
    char name[NAME_MAX];
    char date[DATE_MAX];
    char white_name[NAME_MAX];
    char black_name[NAME_MAX];
} SampleInfo;

int is_number(const char str[]);
void flush_input();
void get_string_input(const char *label, char *input_string, int max_size);
void edit_existing_string(const char *label, char *input_string, int max_size);
int edit_existing_moves(const char *label, char *white_move, char *black_move, int blank);
int standard_menu(void (*menu_to_print)(), int num_of_items, int tries);


#endif //CHESSDATABASE_HELPERFUNCTIONS_H
