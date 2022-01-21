//
// Created by flimsy on 12/18/21.
//
#include <stdio.h>
#include <stdarg.h>
#include <sqlite3.h>
#include <string.h>

#include "database.h"

/* ********** DATABASE QUERIES **********                                                          */

const char beginTransaction[] = "BEGIN TRANSACTION;";

const char commitTransaction[] = "COMMIT;";

const char rollbackTransaction[] = "ROLLBACK;";

const char dropTables[] = "DROP TABLE IF EXISTS game;"
                          "DROP TABLE IF EXISTS moves;"
                          "DROP TABLE IF EXISTS single_move;";

const char tableGame[] = "CREATE TABLE IF NOT EXISTS game("
                         "id INTEGER PRIMARY KEY,"
                         "g_name TEXT,"
                         "g_class TEXT,"
                         "g_group TEXT,"
                         "game_number TEXT,"
                         "date TEXT,"
                         "white_name TEXT,"
                         "black_name TEXT,"
                         "white_result TEXT,"
                         "black_result TEXT"
                         ");";

const char tableMoves[] = "CREATE TABLE IF NOT EXISTS moves("
                          "id INTEGER PRIMARY KEY,"
                          "number_of_moves INTEGER,"
                          "game_id INTEGER,"
                          "FOREIGN KEY(game_id) REFERENCES game(id)"
                          ");";

const char tableSingleMove[] = "CREATE TABLE IF NOT EXISTS single_move("
                               "id INTEGER PRIMARY KEY,"
                               "move_number INTEGER,"
                               "white_move TEXT,"
                               "black_move TEXT,"
                               "moves_id INTEGER,"
                               "FOREIGN KEY(moves_id) REFERENCES moves(id)"
                               ");";

const char insertIntoGame[] = "INSERT INTO game VALUES ("
                              "?, ?, ?, ?, ?, ?, ?, ?, ?, ?"
                              ");";

const char insertIntoMoves[] = "INSERT INTO moves VALUES ("
                               "?, ?, ?"
                               ");";

const char insertIntoSingleMove[] = "INSERT INTO single_move VALUES("
                                    "?, ?, ?, ?, ?"
                                    ");";

const char updateGame[] = "UPDATE game SET g_name = ?, "
                          "g_class = ?, g_group = ?, game_number = ?, date = ?, white_name = ?, "
                          "black_name = ?, white_result = ?, black_result = ? "
                          "WHERE id = ?;";

const char updateMoveCount[] = "UPDATE moves SET number_of_moves = ? WHERE id = ?;";

const char updateMove[] = "UPDATE single_move SET white_move = ?, black_move = ? "
                          "WHERE moves_id = ? AND move_number = ?;";

const char deleteAllSingleMoves[] = "DELETE FROM single_move WHERE moves_id = ?;";

const char deleteSingleMove[] = "DELETE FROM single_move WHERE moves_id = ? AND move_number = ?;";

const char deleteMoves[] = "DELETE FROM moves WHERE game_id = ?;";

const char deleteGameInformation[] = "DELETE FROM game WHERE id = ?;";

const char selectAll[] = "SELECT * FROM game;";

const char selectAllOrderByName[] = "SELECT * FROM game ORDER BY g_name;";

const char selectAllOrderByWhiteName[] = "SELECT * FROM game ORDER BY white_name;";

const char selectAllOrderByBlackName[] = "SELECT * FROM game ORDER BY black_name;";

const char selectAllOrderByDate[] = "SELECT * FROM game ORDER BY date;";

const char selectGameById[] = "SELECT * FROM game "
                              "INNER JOIN moves ON game.id = moves.game_id "
                              "WHERE game.id = ?;";

const char selectSingleMovesById[] = "SELECT * FROM single_move WHERE moves_id = ?;";

const char selectSearch[] = "SELECT * FROM game WHERE ("
                            "g_name LIKE ? OR g_class LIKE ? OR g_group LIKE ? OR "
                            "game_number LIKE ? OR white_name LIKE ? OR black_name LIKE ?);";

/* *********** DATABASE FUNCTIONS **********                                                       */

/* If another error (statement error) occurred during a transaction, this function is
 * called as an 'emergency rollback' before the db is closed.                                      */
void do_fast_rollback(sqlite3 **db)
{
    int status;
    char *err_msg;
    printf("INFO: FAST ROLLBACK!\n");
    status = sqlite3_exec(*db, rollbackTransaction, NULL, NULL, &err_msg);
    if (status != SQLITE_OK) {
        eprintf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

/* If status indicates an error - display error_msg,
 * frees the error_msg and closes the database.
 * returns FALSE on NO error and TRUE on error.                                                    */
int is_exec_error(sqlite3 **db, int status, char **error_msg)
{
    if (status != SQLITE_OK) {
        eprintf("SQL error: %s\n", *error_msg);
        sqlite3_free(error_msg);
        sqlite3_close(*db);
        return TRUE;
    }
    return FALSE;
}

/* The transaction_flag should be set to TRUE if the error check is preformed during
 * a transaction for 'emergency rollback', otherwise FALSE.
 *
 * if status indicates an error - display via sqlite3_errmsg,
 * finalizes the statement and closes the database.
 *
 * returns FALSE on NO error, TRUE otherwise.                                                      */
int is_statement_error(sqlite3 **db, sqlite3_stmt **stmt, int status, int transaction_flag)
{
    if (status != SQLITE_OK) {
        eprintf("Failed to execute statement: %s\n", sqlite3_errmsg(*db));

        if (transaction_flag)
            do_fast_rollback(db);

        sqlite3_finalize(*stmt);
        sqlite3_close(*db);
        return TRUE;
    }
    return FALSE;
}

/* The transaction_flag should be set to TRUE if the error check is preformed during
 * a transaction for 'emergency rollback', otherwise FALSE.
 *
 * if status indicates an error - display via sqlite3_errmsg,
 * finalizes the statement and closes the database.
 *
 * returns FALSE on NO error, TRUE otherwise.                                                      */
int is_binding_error(sqlite3 **db, sqlite3_stmt **stmt, int status, int transaction_flag)
{
    if (status != SQLITE_OK) {
        eprintf("Failed to bind value: %s\n", sqlite3_errmsg(*db));

        if (transaction_flag)
            do_fast_rollback(db);

        sqlite3_finalize(*stmt);
        sqlite3_close(*db);
        return TRUE;
    }
    return FALSE;
}

/* SHOULD ONLY BE CALLED IF SQLITE_DONE IS EXPECTED!
 *
 * The transaction_flag should be set to TRUE if the error check is preformed during
 * a transaction for 'emergency rollback', otherwise FALSE.
 *
 * Checks if the status is equal to SQLITE_DONE, if this is the case FALSE is returned.
 * If the status is not equal to SQLITE_DONE, an appropriate error msg is displayed, statement
 * is finalized, the database is closed and TRUE is returned.                                      */
int is_statement_step_error(sqlite3 **db, sqlite3_stmt **stmt, int status, int transaction_flag)
{
    if (status != SQLITE_DONE) {
        eprintf("Failed to execute statement step: %s\n", sqlite3_errmsg(*db));

        if (transaction_flag)
            do_fast_rollback(db);

        sqlite3_finalize(*stmt);
        sqlite3_close(*db);
        return TRUE;
    }
    return FALSE;
}

/* Attempts to open database chess.db on the given sqlite3 database                                */
int open_database_conn(sqlite3 **db)
{
    int status = sqlite3_open("chess.db", db);
    if (status != SQLITE_OK) {
        eprintf("ERROR: cannot open database: %s\n", sqlite3_errmsg(*db));
        sqlite3_close(*db);
        return FALSE;
    }
    return TRUE;
}

/* Prepares the database - creating the tables if they don't exist.
 * returns TRUE if preparations happened without errors, otherwise FALSE.                          */
int prepare_database()
{
    char *err_msg = 0;
    sqlite3 *db;

    if (!open_database_conn(&db))
        return FALSE;

    // setting up tables if not exist
    int status = sqlite3_exec(db, tableGame, 0, 0, &err_msg);
    if (is_exec_error(&db, status, &err_msg))
        return FALSE;

    status = sqlite3_exec(db, tableMoves, 0, 0, &err_msg);
    if (is_exec_error(&db, status, &err_msg))
        return FALSE;

    status = sqlite3_exec(db, tableSingleMove, 0, 0, &err_msg);
    if (is_exec_error(&db, status, &err_msg))
        return FALSE;

    sqlite3_close(db);
    return TRUE;
}

/* Drops all tables (game, moves, single_move) from database.
 * Returns TRUE on success and FALSE on error.                                                     */
int clear_tables() {
    char *err_msg = 0;
    sqlite3 *db;

    if (!open_database_conn(&db))
        return FALSE;

    int status = sqlite3_exec(db, dropTables, 0, 0, &err_msg);
    if (is_exec_error(&db, status, &err_msg))
        return FALSE;

    sqlite3_close(db);
    return TRUE;
}

/* db: if db is initialized as NULL, db is opened and closed automatic.
 *     if db is not NULL, it will be assumed to be open, and will NOT be closed at the end.
 *     */
int do_statement(sqlite3 *db, SampleInfo arr_sample[], GameInfo *game, GameMoves *game_moves,
                 int transaction_flag, const char *sql, const char *format, ...)
{
    if ((arr_sample != NULL && game != NULL) || (arr_sample != NULL && game_moves != NULL) ||
        (game != NULL && game_moves != NULL)) {
        eprintf("ERROR: only one, either arr_sample, game or game_moves, can be passed...\n");
        return FALSE;
    }

    int len, status, close = FALSE, return_code = TRUE;
    sqlite3_stmt *stmt;

    // opening database...
    if (db == NULL) {
        if (!open_database_conn(&db))
            return FALSE;
        close = TRUE;
    }

    // preparing statement...
    status = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (is_statement_error(&db, &stmt, status, transaction_flag))
        return FALSE;

    // handle bindings if any...
    if (format != NULL) {
        // HANDLE AS STATEMENT WITH BINDINGS...
        // arguments expected...
        va_list args;
        len = (int)strlen(format);
        char format_specifiers[((int) (len / 2)) + 1], c;
        int number_arg = 0, flag = 0;

        while ((c = *format++) != '\0') {
            if (c == '%' && flag == 0) {
                flag = 1;
                continue;
            }

            if (flag == 1) {
                format_specifiers[number_arg] = c;
                flag = 0;
                number_arg++;
            }
        }

        va_start(args, format);
        for (int i = 1, arr_pos = 0; i <= number_arg; i++, arr_pos++) {
            c = format_specifiers[arr_pos];
            if (c == 's') {
                status = sqlite3_bind_text(stmt, i, va_arg(args, char *), -1, SQLITE_TRANSIENT);
                if (is_binding_error(&db, &stmt, status, transaction_flag))
                    return FALSE;
            } else if (c == 'd') {
                status = sqlite3_bind_int(stmt, i, va_arg(args, int));
                if (is_binding_error(&db, &stmt, status, transaction_flag))
                    return FALSE;
            } else if (c == 'b') {
                // skipping because this binding should be left blank (if INT PRIMARY KEY has to be invoked)
            } else {
                eprintf("ERROR: argument nr. %d, id of unknown type!\n", i);
            }
        }
        va_end(args);
    }

    // executing statement...
    if (arr_sample != NULL) {
        // getting row results if any and copies sample data into arr_sample...
        int count = 0;
        while((status = sqlite3_step(stmt)) == SQLITE_ROW) {
            arr_sample[count].id = sqlite3_column_int(stmt, 0);
            strcpy(arr_sample[count].name, (char *)sqlite3_column_text(stmt, 1));
            strcpy(arr_sample[count].date, (char *)sqlite3_column_text(stmt, 5));
            strcpy(arr_sample[count].white_name, (char *)sqlite3_column_text(stmt, 6));
            strcpy(arr_sample[count].black_name, (char *)sqlite3_column_text(stmt, 7));
            count++;
        }

        if (is_statement_step_error(&db, &stmt, status, transaction_flag))
            return FALSE;

        // return_code = number of rows inserted into arr_sample...
        return_code = count;
    } else if (game != NULL) {
        // since only one result is expected the steps are done manually
        status = sqlite3_step(stmt);
        if (status == SQLITE_ROW) {
            strcpy(game->name, (char *)sqlite3_column_text(stmt, 1));
            strcpy(game->class, (char *)sqlite3_column_text(stmt, 2));
            strcpy(game->group, (char *)sqlite3_column_text(stmt, 3));
            strcpy(game->game_number, (char *)sqlite3_column_text(stmt, 4));
            strcpy(game->date, (char *)sqlite3_column_text(stmt, 5));
            strcpy(game->white_name, (char *)sqlite3_column_text(stmt, 6));
            strcpy(game->black_name, (char *)sqlite3_column_text(stmt, 7));
            strcpy(game->white_result, (char *)sqlite3_column_text(stmt, 8));
            strcpy(game->black_result, (char *)sqlite3_column_text(stmt, 9));
            game->game_moves.moves_id = sqlite3_column_int(stmt, 10);
            game->game_moves.move_number = sqlite3_column_int(stmt, 11);
        } else {
            eprintf("ERROR: no rows found by id(%d)...\n", game->game_id);
            return FALSE;
        }

        status = sqlite3_step(stmt);
        if (is_statement_step_error(&db, &stmt, status, transaction_flag))
            return FALSE;

    } else if (game_moves != NULL) {
        // since GameMoves has been passed, we expect a number of moves to be store as a result of sql...
        int move;
        while((status = sqlite3_step(stmt)) == SQLITE_ROW) {
            move = sqlite3_column_int(stmt, 1);
            strcpy(game_moves->moves[move - 1][WHITE_PLAYER], (char *)sqlite3_column_text(stmt, 2));
            strcpy(game_moves->moves[move - 1][BLACK_PLAYER], (char *)sqlite3_column_text(stmt, 3));
        }

        if (is_statement_step_error(&db, &stmt, status, FALSE))
            return FALSE;

    } else {
        // execute statement. Since arr_sample and game is NULL, no data is expected from database...
        status = sqlite3_step(stmt);
        if (is_statement_step_error(&db, &stmt, status, transaction_flag))
            return FALSE;
    }

    // finalize stmt...
    sqlite3_finalize(stmt);

    // close db if required...
    if (close)
        sqlite3_close(db);
    return return_code;
}

/* Attempts to insert data into the database.
 * returns TRUE on success, otherwise FAlSE                                                        */
int insert_data(GameInfo *data)
{
    sqlite3 *db;
    int last_row;

    // opening database...
    if (!open_database_conn(&db))
        return FALSE;

    // begin transaction... all or nothing...
    if (!do_statement(db, NULL, NULL, NULL, FALSE,
                      beginTransaction, NULL))
        return FALSE;

    // execute statement insertIntoGame...
    if (!do_statement(db, NULL, NULL, NULL,TRUE, insertIntoGame,
                      "%b%s%s%s%s%s%s%s%s%s", data->name, data->class, data->group, data->game_number,
                      data->date, data->white_name, data->black_name, data->white_result, data->black_result))
        return FALSE;

    // getting last row...
    last_row = (int)sqlite3_last_insert_rowid(db);

    // execute statement insertIntoMoves...
    if (!do_statement(db, NULL,NULL,NULL,TRUE,
                      insertIntoMoves, "%b%d%d", data->game_moves.move_number, last_row))
        return FALSE;

    // getting last row...
    last_row = (int)sqlite3_last_insert_rowid(db);

    // execute statement(s) insertIntoSingleMove for every move inputted...
    for (int move = 1, arr_pos = 0; move <= data->game_moves.move_number; move++, arr_pos++) {
        if (!do_statement(db, NULL, NULL, NULL,TRUE, insertIntoSingleMove,
                          "%b%d%s%s%d", move, data->game_moves.moves[arr_pos][WHITE_PLAYER],
                          data->game_moves.moves[arr_pos][BLACK_PLAYER], last_row))
            return FALSE;
    }

    // commit transaction...
    if (!do_statement(db, NULL, NULL, NULL, TRUE, commitTransaction, NULL))
        return FALSE;

    // closing database...
    sqlite3_close(db);
    return TRUE;
}

/* Updates data for game with game_id in game table.
 * Returns FALSE (0) on error and TRUE on success.                                                 */
int update_data(GameInfo *data)
{
    sqlite3 *db = NULL;

    return do_statement(db, NULL, NULL, NULL, FALSE,updateGame,
                        "%s%s%s%s%s%s%s%s%s%d", data->name, data->class, data->group, data->game_number,
                        data->date, data->white_name, data->black_name, data->white_result, data->black_result,
                        data->game_id);
}

/* Updates data for moves and single_move related to game_id.
 * Returns FALSE (0) on error and TRUE on success.                                                 */
int update_moves(GameInfo *data, int new_move_count)
{
    int old_move_count = data->game_moves.move_number;
    int max_move = (new_move_count <= old_move_count) ? old_move_count : new_move_count;
    sqlite3 *db = NULL;

    // open database...
    if (!open_database_conn(&db))
        return FALSE;

    // beginning transaction. All or nothing...
    if (!do_statement(db, NULL, NULL, NULL, FALSE,
                      beginTransaction, NULL))
        return FALSE;

    for (int move_num = 1, arr_pos = 0; move_num <= max_move; move_num++, arr_pos++) {
        if (old_move_count < new_move_count && old_move_count < move_num) {
            // execute statement with insertIntoSingleMove...
            if (!do_statement(db, NULL, NULL, NULL, TRUE,
                              insertIntoSingleMove,"%b%d%s%s%d", move_num,
                              data->game_moves.moves[arr_pos][WHITE_PLAYER],
                              data->game_moves.moves[arr_pos][BLACK_PLAYER], data->game_moves.moves_id))
                return FALSE;

        } else if (strcmp(data->game_moves.moves[arr_pos][WHITE_PLAYER], "-") == 0) {
            // execute statement with deleteSingleMove...
            if (!do_statement(db, NULL, NULL, NULL, TRUE, deleteSingleMove,
                              "%d%d", data->game_moves.moves_id, move_num))
                return FALSE;

        } else {
            // execute statement with updateMove...
            if (!do_statement(db, NULL, NULL, NULL, TRUE, updateMove,
                              "%s%s%d%d", data->game_moves.moves[arr_pos][WHITE_PLAYER],
                              data->game_moves.moves[arr_pos][BLACK_PLAYER],
                              data->game_moves.moves_id, move_num))
                return FALSE;
        }
    }

    // execute statement for updateMoveCount...
    if (!do_statement(db, NULL, NULL, NULL, TRUE, updateMoveCount,
                      "%d%d", new_move_count, data->game_moves.moves_id))
        return FALSE;

    // committing transaction...
     if (!do_statement(db, NULL, NULL, NULL, TRUE,
                       commitTransaction, NULL))
         return FALSE;

    // closing database connection...
    sqlite3_close(db);
    return TRUE;
}

/* Retrieves a simplified list of all chess games from the database table that
 * matches the search word.
 * On success the number of elements retrieved is returned, on error 0 (FALSE)
 * is returned.                                                                                    */
int search_data(SampleInfo arr_sample[], const char search_word[])
{
    sqlite3 *db = NULL;

    return do_statement(db, arr_sample, NULL, NULL, FALSE, selectSearch,
                        "%s%s%s%s%s%s", search_word, search_word, search_word, search_word,
                        search_word, search_word);
}

/* Deletes game with game_id from the database.
 * Return TRUE on successful deletion and FALSE on error executing request.                        */
int delete_game(GameInfo *data)
{
    sqlite3 *db;

    // opening database...
    if (!open_database_conn(&db))
        return FALSE;

    // begins transaction...
    if (!do_statement(db, NULL, NULL, NULL, FALSE,
                      beginTransaction, NULL))
        return FALSE;

    // deletes single moves entries related to game...
    if (!do_statement(db, NULL, NULL, NULL, TRUE,
                      deleteAllSingleMoves, "%d", data->game_moves.moves_id))
        return FALSE;


    // deletes the entry in moves table related to game...
    if (!do_statement(db, NULL, NULL, NULL, TRUE,
                      deleteMoves, "%d", data->game_id))
        return FALSE;

    // deletes the entry in game table with game_id...
    if (!do_statement(db, NULL, NULL, NULL, TRUE,
                      deleteGameInformation,"%d", data->game_id))
        return FALSE;

    // commits transaction...
    if (!do_statement(db, NULL, NULL, NULL, TRUE,
                      commitTransaction, NULL))
        return FALSE;

    // closing database...
    sqlite3_close(db);
    return TRUE;
}

/* Retrieves a simplified (unsorted) list of all chess games in the database.
 * on success the number of elements retrieved is returned, on error 0 (FALSE)
 * is returned.                                                                                    */
int get_unsorted_list(SampleInfo arr_sample[])
{
    sqlite3 *db = NULL;

    return do_statement(db, arr_sample, NULL, NULL, FALSE, selectAll, NULL);
}

/* Retrieves a simplified list of all chess games in the database sorted by either,
 * name, white_name, black_name or date.
 * on success the number of elements retrieved is returned, on error 0 (FALSE)
 * is returned.                                                                                    */
int get_sorted_list(SampleInfo arr_sample[], int column)
{
    sqlite3 *db = NULL;

    switch (column) {
        case 1:
            return do_statement(db, arr_sample, NULL, NULL, FALSE,
                                selectAllOrderByName, NULL);
        case 2:
            return do_statement(db, arr_sample, NULL, NULL, FALSE,
                                selectAllOrderByWhiteName, NULL);
        case 3:
            return do_statement(db, arr_sample, NULL, NULL, FALSE,
                                selectAllOrderByBlackName, NULL);
        case 4:
            return do_statement(db, arr_sample, NULL, NULL, FALSE,
                                selectAllOrderByDate, NULL);
        default:
            eprintf("ERROR: invalid column got through first check.\n");
            return FALSE;
    }
}

/* Gets a data from the database by id. If an error was encountered 0 (FALSE)
 * is returned, TRUE is returned if everything went accordingly and the data
 * information was stored in 'data', FALSE, otherwise.                                             */
int get_game_by_id(GameInfo *data)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int status, moves_num;

    if(!open_database_conn(&db))
        return FALSE;
    printf("INFO: database opened...\n");

    // begin transaction... all or nothing...
    if (!do_statement(db, NULL, NULL, NULL, FALSE,
                      beginTransaction, NULL))
        return FALSE;

    // retrieving game by id...
    if (!do_statement(db, NULL, data, NULL, TRUE,
                      selectGameById, "%d", data->game_id))
        return FALSE;

    // retrieving all moves related to game via moves_id...
    if (!do_statement(db, NULL, NULL, &data->game_moves, TRUE,
                      selectSingleMovesById, "%d", data->game_moves.moves_id))
        return FALSE;

    // committing transaction...
    if (!do_statement(db, NULL, NULL, NULL, TRUE,
                      commitTransaction, NULL))
        return FALSE;

    // closes...
    sqlite3_close(db);
    printf("INFO: data retrieved (database - get_game_by_id...\n");
    return TRUE;
}



