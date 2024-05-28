//
// Created by radu on 15.05.2024.
//

#ifndef SI_PROIECT_CLI_H
#define SI_PROIECT_CLI_H

#include <stdio.h>
#include <time.h>
#include "../commons/data_types.h"

int get_line_count(char *string);

void split_options(char *string, char **options_out);

int select_option(int number_of_options, char **options);

void scan_network(char *out_address);

void print_progress(int blocks_received, int blocks_total);

void init_cli();

#endif //SI_PROIECT_CLI_H
