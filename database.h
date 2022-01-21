//
// Created by flimsy on 12/19/21.
//

#ifndef CHESSDATABASE_DATABASE_H
#define CHESSDATABASE_DATABASE_H

#include "helperFunctions.h"

int prepare_database();
int clear_tables();
int insert_data(GameInfo *data);
int update_data(GameInfo *data);
int update_moves(GameInfo *data, int new_move_count);
int search_data(SampleInfo arr_sample[], const char search_word[]);
int delete_game(GameInfo *data);
int get_unsorted_list(SampleInfo arr_sample[]);
int get_sorted_list(SampleInfo arr_sample[], int column);
int get_game_by_id(GameInfo *data);

#endif //CHESSDATABASE_DATABASE_H
