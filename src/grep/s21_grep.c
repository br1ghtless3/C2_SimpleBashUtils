#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grep_config_t config;
  init_config(&config);

  if (argc < 2) {
    fprintf(stderr, "Usage: s21_grep [options] pattern [file...]\n");
    return 1;
  }

  if (!parse_args(argc, argv, &config)) {
    cleanup_config(&config);
    return 1;
  }

  if (!compile_regex(&config)) {
    cleanup_config(&config);
    return 1;
  }

  char *files[argc];
  int file_count = collect_files(argc, argv, &config, files);

  config.multiple_files = (file_count > 1);

  for (int i = 0; i < file_count; i++) {
    FILE *file = fopen(files[i], "r");
    if (file == NULL) {
      if (!config.s) {
        fprintf(stderr, "s21_grep: %s: No such file\n", files[i]);
      }
    } else {
      process_file(file, files[i], &config);
      fclose(file);
    }
  }

  cleanup_config(&config);
  return 0;
}

void init_config(grep_config_t *config) {
  config->i = false;
  config->v = false;
  config->c = false;
  config->l = false;
  config->n = false;
  config->h = false;
  config->s = false;
  config->o = false;
  config->e = false;
  config->f = false;
  config->pattern_count = 0;
  config->multiple_files = false;
  config->regex_compiled = false;
  config->regex_flags = REGEX_FLAGS_DEFAULT;
}

void cleanup_config(grep_config_t *config) {
  if (config->regex_compiled) {
    regfree(&config->regex);
    config->regex_compiled = false;
  }
}

bool parse_args(int argc, char *argv[], grep_config_t *config) {
  for (int i = 1; i < argc; i++) {
    if (is_flag(argv[i]) && argv[i][1] != 'e' && argv[i][1] != 'f') {
      if (!parse_simple_flags(argv[i], config)) {
        return false;
      }
    }
  }

  config->regex_flags = REG_EXTENDED;
  if (config->i) {
    config->regex_flags |= REG_ICASE;
  }

  bool error = false;
  bool pattern_found = false;

  for (int i = 1; i < argc && !error; i++) {
    if (is_flag(argv[i])) {
      if (argv[i][1] == 'e') {
        error = !handle_e_flag(&i, argc, argv, config);
        pattern_found = true;
      } else if (argv[i][1] == 'f') {
        error = !handle_f_flag(&i, argc, argv, config);
        pattern_found = true;
      }
    } else if (!pattern_found) {
      add_pattern(argv[i], config);
      pattern_found = true;
    }
  }

  return !error;
}

bool parse_simple_flags(char *flag_str, grep_config_t *config) {
  bool success = true;

  for (int i = 1; flag_str[i] != '\0' && success; i++) {
    char flag = flag_str[i];

    switch (flag) {
      case 'i':
        config->i = true;
        break;
      case 'v':
        config->v = true;
        break;
      case 'c':
        config->c = true;
        break;
      case 'l':
        config->l = true;
        break;
      case 'n':
        config->n = true;
        break;
      case 'h':
        config->h = true;
        break;
      case 's':
        config->s = true;
        break;
      case 'o':
        config->o = true;
        break;
      default:
        fprintf(stderr, "s21_grep: invalid option -- '%c'\n", flag);
        success = false;
        break;
    }

    if (config->o && (config->c || config->l || config->v)) {
      fprintf(stderr,
              "s21_grep: option -o cannot be used with -c, -l, or -v\n");
      success = false;
      break;
    }
  }

  return success;
}

bool is_flag(char *arg) { return arg[0] == '-' && arg[1] != '\0'; }

void add_pattern(const char *pattern, grep_config_t *config) {
  if (config->pattern_count >= MAX_PATTERNS) {
    if (!config->s) {
      fprintf(stderr, "s21_grep: too many patterns\n");
    }
    return;
  }

  strncpy(config->patterns[config->pattern_count], pattern,
          MAX_PATTERN_LEN - 1);
  config->patterns[config->pattern_count][MAX_PATTERN_LEN - 1] = '\0';
  config->pattern_count++;
}

bool handle_e_flag(int *index, int argc, char *argv[], grep_config_t *config) {
  config->e = true;

  char *pattern = NULL;

  if (argv[*index][2] != '\0') {
    pattern = &argv[*index][2];
  } else if (*index + 1 < argc) {
    (*index)++;
    pattern = argv[*index];
  } else {
    fprintf(stderr, "s21_grep: option requires an argument -- 'e'\n");
    return false;
  }

  if (pattern != NULL) {
    add_pattern(pattern, config);
    return true;
  }

  return false;
}

bool read_patterns_from_file(const char *filename, grep_config_t *config) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    return false;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char line[MAX_LINE_LEN];
  int lines_read = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    lines_read++;
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }

    if (config->pattern_count < MAX_PATTERNS) {
      add_pattern(line, config);
    } else {
      if (!config->s) {
        fprintf(stderr, "s21_grep: pattern limit exceeded\n");
      }
      fclose(file);
      return false;
    }
  }

  if (file_size == 0) {
    add_pattern("", config);
  }

  fclose(file);
  return true;
}

bool handle_f_flag(int *index, int argc, char *argv[], grep_config_t *config) {
  config->f = true;

  if (*index + 1 >= argc) {
    fprintf(stderr, "s21_grep: option requires an argument -- 'f'\n");
    return false;
  }

  (*index)++;
  char *filename = argv[*index];

  if (!read_patterns_from_file(filename, config)) {
    if (!config->s) {
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    }
    return false;
  }

  return true;
}

bool compile_regex(grep_config_t *config) {
  if (config->pattern_count == 0) {
    return true;
  }

  char combined_pattern[MAX_PATTERN_LEN * MAX_PATTERNS + 100] = "";

  for (int i = 0; i < config->pattern_count; i++) {
    if (i > 0) {
      strcat(combined_pattern, "|");
    }

    if (strlen(config->patterns[i]) == 0) {
      if (config->o) {
      } else {
        strcat(combined_pattern, ".*");
      }
    } else {
      strcat(combined_pattern, "(");
      strcat(combined_pattern, config->patterns[i]);
      strcat(combined_pattern, ")");
    }
  }

  if (strlen(combined_pattern) == 0) {
    if (config->o) {
      config->regex_compiled = false;
      return true;
    }
  }

  int ret = regcomp(&config->regex, combined_pattern, config->regex_flags);
  if (ret != 0) {
    if (!config->s) {
      fprintf(stderr, "s21_grep: invalid regular expression\n");
    }
    return false;
  }

  config->regex_compiled = true;
  return true;
}

bool check_line_match(const char *line, grep_config_t *config,
                      regmatch_t *match) {
  if (!config->regex_compiled) {
    return config->v;
  }

  int ret = regexec(&config->regex, line, 1, match, 0);

  if (config->v) {
    return (ret != 0);
  } else {
    return (ret == 0);
  }
}

void process_o_matches(const char *filename, int line_num, const char *line,
                       grep_config_t *config) {
  if (!config->regex_compiled) {
    return;
  }
  const char *ptr = line;
  regmatch_t match;
  int offset = 0;
  size_t line_len = strlen(line);
  if (config->v) {
    return;
  }
  while (offset <= (int)line_len) {
    int ret = regexec(&config->regex, ptr, 1, &match, 0);
    if (ret != 0 || match.rm_so == -1) {
      break;
    }
    int start = offset + match.rm_so;
    int end = offset + match.rm_eo;
    if (start < end) {
      if (config->multiple_files && !config->h) {
        printf("%s:", filename);
      }
      if (config->n) {
        printf("%d:", line_num);
      }
      for (int j = start; j < end; j++) {
        putchar(line[j]);
      }
      putchar('\n');
    }
    if (match.rm_so == match.rm_eo) {
      offset++;
      if (*ptr != '\0') {
        ptr++;
      } else {
        break;
      }
    } else {
      offset += match.rm_eo;
      ptr += match.rm_eo;
    }
    if (ptr > line + line_len) {
      break;
    }
  }
}

void process_file_without_patterns(FILE *file, const char *filename,
                                   grep_config_t *config, int *line_num,
                                   int *match_count, bool *file_has_match) {
  char line[MAX_LINE_LEN];

  if (config->v) {
    while (fgets(line, sizeof(line), file) != NULL) {
      (*line_num)++;
      (*match_count)++;
      *file_has_match = true;

      if (!config->l && !config->c) {
        print_match(filename, *line_num, line, config);
      }
      if (config->l && *file_has_match) {
        break;
      }
    }
  }
}

void process_file_with_patterns(FILE *file, const char *filename,
                                grep_config_t *config, int *line_num,
                                int *match_count, bool *file_has_match) {
  char line[MAX_LINE_LEN];

  while (fgets(line, sizeof(line), file) != NULL) {
    (*line_num)++;

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }

    regmatch_t match;
    bool line_matches = check_line_match(line, config, &match);

    if (line_matches) {
      (*match_count)++;
      *file_has_match = true;

      if (!config->l) {
        if (config->o) {
          process_o_matches(filename, *line_num, line, config);
        } else if (!config->c) {
          print_match(filename, *line_num, line, config);
        }
      }
    }

    if (config->l && *file_has_match && !config->c) {
      break;
    }
  }
}

void process_file(FILE *file, const char *filename, grep_config_t *config) {
  int line_num = 0;
  int match_count = 0;
  bool file_has_match = false;

  if (config->pattern_count == 0 || !config->regex_compiled) {
    process_file_without_patterns(file, filename, config, &line_num,
                                  &match_count, &file_has_match);
  } else {
    process_file_with_patterns(file, filename, config, &line_num, &match_count,
                               &file_has_match);
  }

  if (config->l && file_has_match) {
    printf("%s\n", filename);
  } else if (config->c && !config->o) {
    print_count(filename, match_count, config);
  }
}

int collect_files(int argc, char *argv[], grep_config_t *config, char **files) {
  int file_count = 0;

  for (int i = 1; i < argc; i++) {
    if (!is_flag(argv[i])) {
      bool skip = is_pattern_or_flag_arg(i, argc, argv, config);
      if (!skip) {
        files[file_count++] = argv[i];
      }
    }
  }

  return file_count;
}

bool is_pattern_or_flag_arg(int index, int argc, char *argv[],
                            grep_config_t *config) {
  (void)argc;

  for (int j = 0; j < config->pattern_count; j++) {
    if (strcmp(argv[index], config->patterns[j]) == 0) {
      return true;
    }
  }

  if (index > 1 && argv[index - 1][0] == '-') {
    char flag = argv[index - 1][1];
    if (flag == 'e' || flag == 'f') {
      return true;
    }
  }

  return false;
}

void print_match(const char *filename, int line_num, const char *line,
                 grep_config_t *config) {
  if (config->multiple_files && !config->h) {
    printf("%s:", filename);
  }
  if (config->n) {
    printf("%d:", line_num);
  }
  printf("%s\n", line);
}

void print_count(const char *filename, int count, grep_config_t *config) {
  if (config->multiple_files && !config->h) {
    printf("%s:", filename);
  }
  printf("%d\n", count);
}