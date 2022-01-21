# ExampleChessDatabase

Description:
------------
A simple Chess Database writen in C (C99). Interaction with user is handled through a console (terminal).
Database used is SQLite version 3.

The program enables user (preferably a chessplayer) to input played games into the database, view games 
unsorted or sorted, edit games, delete games and search for a specific game.

How to run:
-----------
Can either be compiled via console, but a Make-file is also provided.
Console Example (gcc): gcc main.c helperFunctions.h console.h console.c database.c database.h helperFunctions.c -lsqlite3 -std=c99
 
