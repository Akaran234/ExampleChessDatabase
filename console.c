//
// Created by flimsy on 12/14/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "helperFunctions.h"
#include "console.h"


/* PRINT FUNCTIONS: DISPLAY MENU, INFORMATION, SAMPLE DATA OR FULL GAME... */

/* Prints a numbered list num_of_games in game, and awaits input from user,
 * which will be stored and returned through choice.                                                 */
void print_simplified_list(const SampleInfo arr_sample[], int num_of_games)
{
    system("clear");
    printf("\n\tId |      Name     |   White Name  |   Black Name  |      Date     |\n");
    for (int i = 0; i < num_of_games; i++) {
        printf("\t %d | %10.10s%s | %10.10s%s | %10.10s%s | %10.10s%s |\n",
               arr_sample[i].id,
               arr_sample[i].name, (strlen(arr_sample[i].name) <= 10) ? "   " : "...",
               arr_sample[i].white_name, (strlen(arr_sample[i].white_name) <= 10) ? "   " : "...",
               arr_sample[i].black_name, (strlen(arr_sample[i].black_name) <= 10) ? "   " : "...",
               arr_sample[i].date, (strlen(arr_sample[i].date) <= 10) ? "   " : "...");
    }
}

/* Prints out full game information including moves.                                                 */
void print_full_game(const GameInfo *game)
{
    system("clear");
    printf("\t**************** Game Information ****************\n");
    printf("\n\tName/Tournament: %33s\n", game->name);
    printf("\tClass:           %33s\n", game->class);
    printf("\tGroup:           %33s\n", game->group);
    printf("\tGame nr:         %33s\n", game->game_number);
    printf("\tDate:            %33s\n", game->date);
    printf("\tWhite:           %33s\n", game->white_name);
    printf("\tBlack:           %33s\n", game->black_name);
    printf("\t--------------------------------------------------\n");
    printf("\t           White %s    | mv |     Black %s \n", game->white_result, game->black_result);
    printf("\t----------------------|----|----------------------\n");
    for (int mv = 0; mv < game->game_moves.move_number; ++mv) {
        printf("\t             %5s    | %2d |   %5s  \n", game->game_moves.moves[mv][0],
               mv+1,
               game->game_moves.moves[mv][1]);

        printf("\t----------------------|----|----------------------\n");
    }
    printf("\t                      | -- |\n");
    printf("\t--------------------------------------------------\n");
}

/* Print out the main menu. Note - 3 items in menu.                                                  */
void print_main_menu()
{
    system("clear");
    printf("\t********** Chess Database **********\n\n");
    printf("\t(1) Add new game to database.\n");
    printf("\t(2) View game.\n");
    printf("\t(3) Quit.\n");
    printf("\t>> ");
}

/* Print out the submenu used in view_game. Note - 4 items in menu.                                  */
void print_view_game_submenu()
{
    system("clear");
    printf("\t********** View Game **********\n");
    printf("\t(1) View Unsorted list.\n");
    printf("\t(2) View sorted list.\n");
    printf("\t(3) Custom search.\n");
    printf("\t(4) Back to main menu.\n");
    printf("\t>> ");
}

/* Print out the sorting selection menu. Note - 5 items in menu.                                     */
void print_sorting_menu()
{
    system("clear");
    printf("\t********** Sort by: **********\n");
    printf("\t(1) Name.\n");
    printf("\t(2) White name.\n");
    printf("\t(3) Black name.\n");
    printf("\t(4) Date.\n");
    printf("\t(5) Back to main menu.\n");
    printf("\t>> ");
}

/* Print out the pre-edit menu. Note - 2 items in menu.                                              */
void print_pre_edit_menu()
{
    printf("\t(1) Edit game.\n");
    printf("\t(2) Back to menu.\n");
    printf("\t>> ");
}

/* Print out the edit selection menu. Note - 4 items in menu.                                        */
void print_edit_menu()
{
    system("clear");
    printf("\t********** Edit Menu **********\n");
    printf("\t(1) Change game information "
           "(name, class, group, game nr., date, player names, result).\n");
    printf("\t(2) Change game moves.\n");
    printf("\t(3) Delete game.\n");
    printf("\t(4) Back to menu.\n");
    printf("\t>> ");
}

/* Print information sheet (guidelines) for inputting game moves.                                    */
void print_scan_moves_information()
{
    printf("\t****************************** Info *******************************\n");
    printf("\t* Moves has to be separated by a space (whitespace), like so:     *\n");
    printf("\t* e4 e5(return).                                                  *\n");
    printf("\t* When the end of moves is reached, type 'end' in the             *\n");
    printf("\t* appropriate move field.                                         *\n");
    printf("\t*******************************************************************\n");
}

/* Print information sheet (guidelines) for altering information in an existing game.                */
void print_edit_information()
{
    system("clear");
    printf("\t****************************** Info *******************************\n");
    printf("\t* To keep already listed text: Press return without input.        *\n");
    printf("\t* To erase text without new input: Enter '-' and press return.    *\n");
    printf("\t* To alter text: Input new text and press return.                 *\n");
    printf("\t*******************************************************************\n");
}

/* Print information sheet (guidelines) for altering the moves of an existing game.                  */
void print_edit_moves_information()
{
    system("clear");
    printf("\t****************************** Info *******************************\n");
    printf("\t* To keep already listed moves: Press return without input.       *\n");
    printf("\t* To erase move and all following moves: Enter 'end' and          *\n");
    printf("\t*                                        press return.            *\n");
    printf("\t* To alter moves: Input new move(s) separated by a space,         *\n");
    printf("\t*                 and press return.                               *\n");
    printf("\t*                 If the last move was the last entered black     *\n");
    printf("\t*                 enter 'end' in white players move and return.   *\n");
    printf("\t*                 If the white players move is the last,          *\n");
    printf("\t*                 fill black players move with 'end'.             *\n");
    printf("\t*******************************************************************\n");
}

/* SPECIALIZED FUNCTIONS... */

/* Does a scan of all moves of the games and stores
 * all in *moves.                                                                                    */
void scan_moves(GameMoves *moves) {
    int move_num = 1, arr_pos = 0;
    char prt_line[14], wb_move[S_MOVE_MAX * 2 - 1], split_moves[2][S_MOVE_MAX];

    print_scan_moves_information();

    while (move_num <= MOVES_MAX) {
        int player = WHITE_PLAYER;

        sprintf(prt_line, "\t%d move: ", move_num);
        get_string_input(prt_line, wb_move, S_MOVE_MAX * 2 - 1);

        char *token = strtok(wb_move, " ");

        while(token != NULL) {
            if (player <= BLACK_PLAYER) {
                strcpy(split_moves[player], token);
            }
            token = strtok(NULL, " ");
            player++;
        }

        if (strcmp(split_moves[WHITE_PLAYER], "end") == 0) {
            move_num--;
            break;
        }
        else
            strcpy(moves->moves[arr_pos][WHITE_PLAYER], split_moves[WHITE_PLAYER]);

        if (strcmp(split_moves[BLACK_PLAYER], "end") == 0) {
            strcpy(moves->moves[arr_pos][BLACK_PLAYER], "-");  // To avoid random memory when saving.
            break;
        }
        else
            strcpy(moves->moves[arr_pos][BLACK_PLAYER], split_moves[BLACK_PLAYER]);

        move_num++;
        arr_pos++;
    }

    moves->move_number = move_num;
}

/* Prompts the user for information about the game and stores
 * the data in game.                                                                                 */
void scan_game(GameInfo *game) {
    system("clear");
    printf("\t********** Game Info **********\n");
    get_string_input("\tName: ",game->name,NAME_MAX);
    get_string_input("\tClass: ",game->class,NAME_MAX);
    get_string_input("\tGroup: ",game->group,NAME_MAX);
    get_string_input("\tGame Nr.: ",game->game_number,NAME_MAX);
    get_string_input("\tDate: ",game->date,DATE_MAX);
    get_string_input("\tWhite: ",game->white_name,NAME_MAX);
    get_string_input("\tBlack: ",game->black_name,NAME_MAX);
    get_string_input("\tWhite result (1, 0, 1/2, remis): ",game->white_result,RESULT_MAX);
    get_string_input("\tBlack result: ",game->black_result,RESULT_MAX);
}

/* Retrieves an unsorted list of chess games. Return TRUE if the data was
 * retrieved successfully, FALSE otherwise.
 * Output arguments:
 *     arr_sample - Sample info suited for display.
 *     num_of_elements - Number of elements returned.                                                */
int unsorted_list(SampleInfo arr_sample[], int *num_of_elements)
{
    *num_of_elements = get_unsorted_list(arr_sample);
    if (!*num_of_elements) {
        printf("\tNo games found in the database!\n");
        printf("\tPress ENTER to continue...");
        getchar();
        return FALSE;
    }
    return TRUE;
}

/* Retrieves a sorted list of chess games. Return TRUE if the data was
 * retrieved successfully, FALSE otherwise.
 * Output arguments:
 *     arr_sample - Sample info suited for display.
 *     num_of_elements - Number of elements returned.                                                */
int sorted_list(SampleInfo arr_sample[], int *num_of_elements)
{
    int ch;

    if (!(ch = standard_menu(print_sorting_menu, 5, 3))) {
        printf("\tReturning to main menu...\n");
        return TRUE; // hence, no errors were encountered, but max tries was exhausted...
    }

    *num_of_elements = get_sorted_list(arr_sample, ch);
    if (!*num_of_elements) {
        printf("\tNo games found in the database!)\n");
        printf("\tPress ENTER to continue...");
        getchar();
        return FALSE;
    }
    return TRUE;
}

/* Gets a string input from user and searches the database table 'game'
 * for entries that matches the search. Table columns that are searched:
 * g_name, g_class, g_group, game_number, white_name, black_name.
 * Returns TRUE if the transaction with database executed without errors,
 * otherwise FALSE.                                                                                  */
int search(SampleInfo arr_sample[], int *num_of_elements)
{
    char search_word[NAME_MAX-3];
    char mod_src[NAME_MAX];

    // prompts for search input...
    get_string_input("\tSearch: ", search_word, NAME_MAX-3);

    // modify search_word... "%search_word%"
    strcpy(mod_src, "%");
    strcat(mod_src, search_word);
    strcat(mod_src, "%");

    *num_of_elements = search_data(arr_sample, mod_src);

    if (!*num_of_elements) {
        printf("\tNo entries fits the search: '%s'!\n", search_word);
        printf("\tPress ENTER to continue...");
        getchar();
        return FALSE;
    }
    return TRUE;
}

/* Displays a sample list of all games in the database,
 * prompt the user for a choice of game to display and returns result,
 * 0 (FALSE) if 'back to menu' or 0 if max_tries has reached.                                        */
int list_games(const SampleInfo arr_sample[], int num_of_elements) {
    char choice[5];
    int max_tries = 3;

    // printing list and asks for either a choice of game or 'b' for back to menu...
    print_simplified_list(arr_sample, num_of_elements);
    while (TRUE) {
        if (max_tries == 0) {
            printf("\tMax tries used!\n");
            printf("\tPress ENTER to continue...");
            getchar();
            return FALSE;
        }

        printf("\n\t(choose a game or 'b' for menu) >> ");
        scanf("%4s", choice);
        flush_input();
        if (is_number(choice))
            return atoi(choice); // already checked successfully as a numeric value...
        else if (strcmp(choice, "b") == 0)
            return FALSE;
        else
            printf("\tInvalid choice, please try again...\n");

        max_tries--;
    }
}

/* Gets a chosen game by id and stores it in game.
 * return TRUE if all went accordingly, FALSE otherwise.                                             */
int get_game(GameInfo *game)
{
    if (get_game_by_id(game))
        return TRUE;
    return FALSE;
}

/* Prompts user to alter information on chosen game, and updates the game in the database.           */
void edit_game_information(GameInfo *game)
{
    print_edit_information();
    edit_existing_string("\tName: ",game->name,NAME_MAX);
    edit_existing_string("\tClass: ",game->class,NAME_MAX);
    edit_existing_string("\tGroup: ",game->group,NAME_MAX);
    edit_existing_string("\tGame Nr.: ",game->game_number,NAME_MAX);
    edit_existing_string("\tDate: ",game->date,DATE_MAX);
    edit_existing_string("\tWhite: ",game->white_name,NAME_MAX);
    edit_existing_string("\tBlack: ",game->black_name,NAME_MAX);
    edit_existing_string("\tWhite result (1, 0, 1/2, remis): ",game->white_result,RESULT_MAX);
    edit_existing_string("\tBlack result: ",game->black_result,RESULT_MAX);
}

/* Prompts user to alter the moves of a chosen game, after alteration the database information
 * is updated.
 * Returns: integer representing the new number of moves.*/
int edit_game_moves(GameInfo *game)
{
    int move_num = 1, arr_pos = 0, status, new_move_count = 0;
    char label[14];

    // print edit information...
    print_edit_moves_information();

    // edit moves...
    while (move_num <= MOVES_MAX) {
        // preparing label...
        sprintf(label, "\t%d move: ", move_num);

        // call edit of move...
        status = edit_existing_moves(label,
                                     game->game_moves.moves[arr_pos][WHITE_PLAYER],
                                     game->game_moves.moves[arr_pos][BLACK_PLAYER],
                                     (move_num <= game->game_moves.move_number) ? FALSE : TRUE);


        // If not CONTINUE, wrapping up, inserts blanks ('-') moves greater than the new_move_count.
        // this is in the case of editing resulting in fewer moves than stored...
        if (status != CONTINUE || move_num == MOVES_MAX-1) {
            if (status == END_WHITE)
                move_num--;
            new_move_count = move_num;

            int first_flag = TRUE;
            while (move_num <= game->game_moves.move_number) {
                if (first_flag && status == END_BLACK) {
                    move_num++;
                    first_flag = FALSE;
                }

                strcpy(game->game_moves.moves[move_num][WHITE_PLAYER], "-");
                strcpy(game->game_moves.moves[move_num][BLACK_PLAYER], "-");
                move_num++;
            }
            break;
        }
        move_num++;
        arr_pos++;
    }
    return new_move_count;
}

/* MAIN MENU FUNCTIONS... */

/* Prompts user for information about the game and inserts data into database.                       */
int input_game()
{
    GameInfo game;
    scan_game(&game);
    scan_moves(&game.game_moves);
    return insert_data(&game);
}

/* Pre edit game:
 * Prints the full game passed, and prompts the pre edit menu. Returns TRUE (1) if the edit menu was requested.
 * On error, ERROR (-1) is returned. Back to main menu, BACK_TO_MENU (-2) is returned.                            */
int pre_edit_game(const GameInfo *game)
{
    int ch;

    print_full_game(game);
    ch = standard_menu(print_pre_edit_menu, 4, 3);

    if (ch == 1) {
        return TRUE;
    } else if (ch == 2) {
        printf("\tReturning to main menu...\n");
        return BACK_TO_MENU;
    } else {
        printf("\tReturning to main menu...\n");
        eprintf("ERROR: An invalid choice passed through the standard_menu function!\n");
        return ERROR;
    }
}

/* Edit Game:
 * Prompts the user with several choices of editing, and executes the request accordingly.
 * Returns TRUE if protocol was executed without errors, FALSE otherwise.                            */
int edit_game(GameInfo *game)
{
    int ch, status;

    status = pre_edit_game(game);

    if (status == BACK_TO_MENU)
        return TRUE;
    else if (status == ERROR)
        return FALSE;

    ch = standard_menu(print_edit_menu, 4, 3);

    switch(ch) {
        case 1: // edit game information.
            edit_game_information(game);
            if (!update_data(game))
                return FALSE;
            printf("INFO: data updated successfully...\n");
            return TRUE;

        case 2: {// 2 edit game moves.
            int new_moves_count = edit_game_moves(game);
            printf("INFO: number of moves after editing: %d\n", new_moves_count);
            if (!update_moves(game, new_moves_count))
                return FALSE;
            printf("INFO: moves updated successfully...\n");
            break;
        }
        case 3: // 3 delete game.
            if (!delete_game(game))
                return FALSE;
            printf("INFO: data deleted successfully...\n");
            break;

        case 4: // 4 back to menu.
            printf("\tReturning to main menu...\n");
            return TRUE;

        default:
            eprintf("ERROR: an error occurred which should have been prevented by standard menu.\n");
            return FALSE;
    }
    return TRUE;
}

/* View Game:
 * Controls the view game menu. Whenever a request has been executed, either successfully or not,
 * this function return either TRUE (1), if the protocol was successfully executed, or FALSE (0) if
 * an error occurred.                                                                                */
int view_game()
{
    // insert options (submenu): 1. list games unsorted, 2. list game sorted by (..?..) or 3. search by (..?..)
    SampleInfo arr_sample[100];
    int ch, id, num_of_samples = 0;

    if (!(ch = standard_menu(print_view_game_submenu, 4, 3))) {
        printf("\tReturning to main menu...\n");
        return TRUE; // hence, no errors were encountered, but max tries was exhausted...
    }

    // responding to choice...
    if (ch == 1) {
        if (!unsorted_list(arr_sample, &num_of_samples))  // unsorted list...
            return FALSE;
    }
    else if (ch == 2) {
        if (!sorted_list(arr_sample, &num_of_samples))    // sorted list...
            return FALSE;
    }
    else if (ch == 3) {
        if (!search(arr_sample, &num_of_samples))         // search list...
            return FALSE;
    }
    else if (ch == 4) {
        return TRUE;                                                     // back to menu...
    }

    // displaying simplified list of games and acting accordingly...
    id = list_games(arr_sample, num_of_samples);

    if (id) {
        GameInfo game;
        game.game_id = id;
        if (get_game(&game)) {
            printf("INFO: game retrieved successfully...\n");
            if (edit_game(&game))
                printf("INFO: edit game protocol executed successfully...\n\tReturning to main menu...\n");
        }
        else
            return FALSE;
    }
    return TRUE;
}

/* Main driver function.                                                                             */
void run_terminal_edition() {
    char choice[2];
    int ch;

    while (TRUE) {
        print_main_menu();
        scanf("%1s", choice);
        flush_input();

        ch = (int)strtol(choice, NULL, 10);

        if (ch == 1) {
            if (input_game())
                printf("INFO: input_game protocol executed without errors...\n");
        }
        else if (ch == 2) {
            if (view_game())
                printf("INFO: view_game protocol executed without errors...\n");
        }
        else if (ch == 3)
            break;
        else
            printf("\tInvalid choice: %d!\n\n", ch);
    }
}
