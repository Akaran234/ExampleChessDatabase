//
// Created by flimsy on 1/21/22.
//

#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "database.h"

int main()
{
    if (!prepare_database()) {
        printf("INFO: database preparations failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("INFO: database preparations was successful!\n");
    run_terminal_edition();

    return 0;
}