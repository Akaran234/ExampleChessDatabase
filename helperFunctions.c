//
// Created by flimsy on 1/7/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helperFunctions.h"

/* Check if a str is only digits. Return 1 (TRUE) if only digits,
 * 0 (FALSE) otherwise                                                                        */
int is_number(const char str[])
{
    int length = (int)strlen(str);
    for (int i = 0; i < length; i++) {
        if (!isdigit(str[i]))
            return FALSE;
    }
    return TRUE;
}

/* Returns the number of digits in a number.                                                  */
int count_digits(int num)
{
    int count = 0;

    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}

/* Flushing stdin stream for overflowing characters.                                          */
void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        printf("flushing character: %c\n", c);
    }
}

/* Displays label, awaits input from user via stdin,
 * collects input to input_string according to size
 * and finally flushes the stdin stream.                                                      */
void get_string_input(const char *label, char *input_string, int max_size)
{
    char format[10];
    /* Note: A problem arose when trying to assign directly to input_string,
     *       not when a string was entered, but in the event of no input
     *       (newline skip). The only solution was to assign *str with all memory
     *       set to zero.                                                        */
    char *str = calloc(max_size, 1);
    sprintf(format, "%%%d[^\n]c", max_size-1);

    printf("%s", label);
    scanf(format, str);

    if (strcmp(str, "\0") == 0)
        strcpy(input_string, "-");
    else
        strcpy(input_string, str);

    free(str);
    flush_input();
}

/* Displays label and input_string, which should contain the existing string that should be altered.
 * Awaits input from user via stdin -> return              = keep original string,
 *                                     '-' + return        = delete existing string,
 *                                     new string + return = replace original string.
 * and finally flushes the stdin stream.                                                      */
void edit_existing_string(const char *label, char *input_string, int max_size)
{
    char *str, format[count_digits(max_size) + 10];
    str = (char *)calloc(max_size, 1);

    sprintf(format, "%%%d[^\n]c", max_size-1);

    printf("\t(Existing) %s%s\n", label, input_string);
    printf("\t(New)      %s", label);
    scanf(format, str);

    flush_input();

    if (strcmp(str, "\0") == 0) {
        printf("\tKeeping text: %s\n", input_string);
    } else if (strcmp(str, "-") == 0) {
        printf("\tText erased '-'\n");
        strcpy(input_string, "-");
    } else {
        printf("\tNew text: %s\n", str);
        strcpy(input_string, str);
    }
    free(str);
}

/* Displays label and existing moves, which should contain the existing moves, if any.
 *
 * input parameter 'blank' is set to TRUE if white_move and black_move doesn't contain
 * any valid data and '-' should be displayed. Otherwise, set to FALSE.
 *
 * Awaits input from user via stdin -> return                = keep original moves,
 *                                     'end' + return        = end at this move,
 *                                     new moves + return    = replace original moves.
 *                                     (if new moves contains 'end' the return value
 *                                      will indicate end of input.)
 *
 * Returns: END_WHITE (1), END_BLACK (2), CONTINUE (0) otherwise.                    */
int edit_existing_moves(const char *label, char *white_move, char *black_move, int blank)
{
    int return_code = CONTINUE, wb_max = S_MOVE_MAX * 2 - 1;;
    char *str, format[count_digits(wb_max) + 10], split_moves[2][S_MOVE_MAX];
    str = (char *)calloc(wb_max, 1);

    sprintf(format, "%%%d[^\n]c", wb_max-1);
    printf("\t(Existing) %s%s %s\n",
           label,
           (blank) ? "-" : white_move,
           (blank) ? "-" : black_move);
    printf("\t(New)      %s", label);
    scanf(format, str);

    flush_input();

    if (strcmp(str, "\0") == 0) {
        printf("\tKeeping moves: %s %s\n", white_move, black_move);
    } else {
        printf("\tNew text: %s\n", str);
        int player = WHITE_PLAYER;
        char *token = strtok(str, " ");

        while(token != NULL) {
            if (player <= BLACK_PLAYER) {
                strcpy(split_moves[player], token);
            }
            token = strtok(NULL, " ");
            player++;
        }

        if (strcmp(split_moves[WHITE_PLAYER], "end") == 0) {   // if 'end some_move' is inputted...
            return_code = END_WHITE;
        } else
            strcpy(white_move, split_moves[WHITE_PLAYER]);

        if (strcmp(split_moves[BLACK_PLAYER], "end") == 0) {
            strcpy(black_move, "-");  // To avoid random memory when saving.
            return_code = END_BLACK;
        } else
            strcpy(black_move, split_moves[BLACK_PLAYER]);
    }

    free(str);
    return return_code;
}

/* Standard menu:
 * Displays a menu with an assigned number of items for an assigned number of tries.
 * Returns the menu item number selected, if 'tries' were exhausted 0 is returned (FALSE).
 * (input parameter) menu_to_print - void function displaying the menu.
 * (input parameter) num_of_items  - number of selectable items in menu (1 - number_of_items).
 * (input parameter) tries         - number of tries accepted.                                */
int standard_menu(void (*menu_to_print)(), int num_of_items, int tries)
{
    int ch, max_tries = tries-1, num_of_digits = count_digits(num_of_items);
    char choice[num_of_digits], format[18]; // format: 7 char + a max of 10 digits in int and null char.

    sprintf(format, "%%%ds", num_of_digits);

    while (TRUE) {
        menu_to_print();
        scanf(format, choice);
        flush_input();

        ch = (int)strtol(choice, NULL, 10);

        if (0 < ch && ch <= num_of_items) {
            return ch;
        }


        if (max_tries == 0) {
            printf("\tMax number of tries used!\n");
            printf("\tPress ENTER to continue...");
            getchar();
            return FALSE;
        }

        printf("\tInvalid choice! Please try again...\n");
        max_tries--;
    }
}