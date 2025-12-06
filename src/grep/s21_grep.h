#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATTERNS 100
#define MAX_PATTERN_LEN 1024
#define MAX_LINE_LEN 1024
#define REGEX_FLAGS_DEFAULT REG_EXTENDED

typedef struct {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool o;
  bool e;
  bool f;

  char patterns[MAX_PATTERNS][MAX_PATTERN_LEN];
  int pattern_count;

  bool multiple_files;
  bool regex_compiled;
  int regex_flags;
  regex_t regex;
} grep_config_t;

void init_config(grep_config_t *config);
void cleanup_config(grep_config_t *config);
bool parse_args(int argc, char *argv[], grep_config_t *config);
bool parse_simple_flags(char *flag_str, grep_config_t *config);
bool is_flag(char *arg);
void add_pattern(const char *pattern, grep_config_t *config);
bool handle_e_flag(int *index, int argc, char *argv[], grep_config_t *config);
bool read_patterns_from_file(const char *filename, grep_config_t *config);
bool handle_f_flag(int *index, int argc, char *argv[], grep_config_t *config);
bool compile_regex(grep_config_t *config);
bool check_line_match(const char *line, grep_config_t *config,
                      regmatch_t *match);
void process_o_matches(const char *filename, int line_num, const char *line,
                       grep_config_t *config);
void process_file_without_patterns(FILE *file, const char *filename,
                                   grep_config_t *config, int *line_num,
                                   int *match_count, bool *file_has_match);
void process_file_with_patterns(FILE *file, const char *filename,
                                grep_config_t *config, int *line_num,
                                int *match_count, bool *file_has_match);
void process_file(FILE *file, const char *filename, grep_config_t *config);
int collect_files(int argc, char *argv[], grep_config_t *config, char **files);
bool is_pattern_or_flag_arg(int index, int argc, char *argv[],
                            grep_config_t *config);
void print_match(const char *filename, int line_num, const char *line,
                 grep_config_t *config);
void print_count(const char *filename, int count, grep_config_t *config);

#endif