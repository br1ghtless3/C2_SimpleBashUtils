#ifndef S21_CAT_H_
#define S21_CAT_H_

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int b_flag;
  int e_flag;
  int n_flag;
  int s_flag;
  int t_flag;
  int v_flag;
} cat_options_t;

typedef struct {
  int line_number;
  int empty_lines_count;
  int last_char;
} cat_state_t;

int parse_arguments(int argc, char *argv[], cat_options_t *options);

int process_file(const char *filename, const cat_options_t *options);

void init_state(cat_state_t *state);

void read_and_print_loop(FILE *file, const cat_options_t *options,
                         cat_state_t *state);

int handle_line_start(int current_char, const cat_options_t *options,
                      cat_state_t *state);

void print_char_with_flags(int current_char, const cat_options_t *options);

int print_non_printable(int current_char);

#endif