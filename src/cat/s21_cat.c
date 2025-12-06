#include "s21_cat.h"

// Длинные опции GNU
struct option const options_GNU[] = {
    {"number-nonblank",  no_argument, NULL, 'b'},
    {"show-ends",        no_argument, NULL, 'E'},
    {"number",           no_argument, NULL, 'n'},
    {"squeeze-blank",    no_argument, NULL, 's'},
    {"show-tabs",        no_argument, NULL, 'T'},
    {"show-nonprinting", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0},
};

// --- 1. Вспомогательные функции для печати символов ---

// Утилитарная функция: печатает непечатаемые символы (кроме \n и \t).
int print_non_printable(int current_char) {
    int processed = 0;
    
    if (current_char >= 0 && current_char <= 31 && current_char != '\t' && current_char != '\n') {
        printf("^%c", current_char + 64);
        processed = 1;
    } else if (current_char == 127) {
        printf("^?");
        processed = 1;
    } else if (current_char >= 128 && current_char <= 255) {
        printf("M-");
        if (current_char >= 128 && current_char <= 159) {
            printf("^%c", current_char - 64);
        } else {
            printf("%c", current_char - 128);
        }
        processed = 1;
    }
    
    return processed;
}

// Печатает символ, применяя флаги -e, -t, -v.
void print_char_with_flags(int current_char, const cat_options_t *options) {
    int is_tab = (current_char == '\t');
    int is_newline = (current_char == '\n');
    int is_printed = 0;

    // 1. Обработка -e (показывает $ в конце строки)
    if (options->e_flag && is_newline) {
        printf("$");
    }

    // 2. Обработка -t (показывает ^I вместо \t)
    if (options->t_flag && is_tab) {
        printf("^I");
        is_printed = 1;
    } 
    
    // 3. Обработка -v (показывает непечатаемые символы)
    if (options->v_flag && !is_tab && !is_newline && !is_printed) {
        if (print_non_printable(current_char) == 1) {
            is_printed = 1;
        }
    } 
    
    // 4. Печать символа, если он не был обработан флагами -t или -v.
    if (!is_printed) {
        printf("%c", current_char);
    }
}

// --- 2. Вспомогательные функции для нумерации и состояния ---

// Инициализация структуры состояния.
void init_state(cat_state_t *state) {
    state->line_number = 1;
    state->empty_lines_count = 0;
    state->last_char = '\n';
}

// Обрабатывает логику нумерации и сжатия в начале строки.
int handle_line_start(int current_char, const cat_options_t *options, cat_state_t *state) {
    int skip_line = 0;

    if (state->last_char == '\n') {
        // Логика сжатия (-s)
        if (options->s_flag && current_char == '\n') {
            state->empty_lines_count++;
            if (state->empty_lines_count > 1) {
                skip_line = 1;
            }
        } else {
            state->empty_lines_count = 0; 
        }

        // Логика нумерации (-n, -b)
        if (skip_line == 0) {
            if (options->n_flag) {
                printf("%6d\t", state->line_number);
                state->line_number++;
            }
            else if (options->b_flag && current_char != '\n') { 
                printf("%6d\t", state->line_number);
                state->line_number++;
            }
        }
    }
    
    return skip_line;
}

// --- 3. Основные функции обработки ---

// Главный цикл чтения и печати символов.
void read_and_print_loop(FILE *file, const cat_options_t *options, cat_state_t *state) {
    int current_char;
    int skip_char = 0; 

    while ((current_char = fgetc(file)) != EOF) {
        skip_char = 0;
        
        if (handle_line_start(current_char, options, state) == 1) {
            skip_char = 1;
        }

        if (skip_char == 0) {
            print_char_with_flags(current_char, options);
        }

        state->last_char = current_char;
    }
}

// Обработка одного файла с передачей состояния
int process_single_file(const char *filename, const cat_options_t *options, cat_state_t *state) {
    FILE *file = NULL;
    int error = 0;

    if (filename == NULL || strcmp(filename, "-") == 0) {
        file = stdin; 
    } else {
        file = fopen(filename, "r");
    }

    if (file) {
        read_and_print_loop(file, options, state);
        
        if (file != stdin) {
            fclose(file);
        }
    } else if (filename != NULL) {
        fprintf(stderr, "s21_cat: %s: No such file or directory\n", filename);
        error = 1;
    }
    
    return error;
}

// Парсинг аргументов командной строки и флагов.
int parse_arguments(int argc, char *argv[], cat_options_t *options) {
    int optchar;
    const char *optstring = "benstvET"; 
    int error = 0;

    optind = 1; 

    while ((optchar = getopt_long(argc, argv, optstring, options_GNU, NULL)) != -1) {
        switch (optchar) {
            case 'b': options->b_flag = 1; break;
            case 'e': options->e_flag = 1; options->v_flag = 1; break;
            case 'n': options->n_flag = 1; break;
            case 's': options->s_flag = 1; break;
            case 't': options->t_flag = 1; options->v_flag = 1; break;
            case 'v': options->v_flag = 1; break;
            case 'E': options->e_flag = 1; break;
            case 'T': options->t_flag = 1; break;
            case '?': 
            default:
                error = 1; 
                break;
        }
    }

    if (options->b_flag && options->n_flag) {
        options->n_flag = 0;
    }

    return error;
}

// --- 4. Главная функция ---

int main(int argc, char **argv) {
    cat_options_t options = {0};
    cat_state_t state;
    int error_n = 0;

    error_n = parse_arguments(argc, argv, &options);

    if (error_n == 0) {
        init_state(&state);
        
        if (optind == argc) {
            // Обработка stdin
            error_n = process_single_file(NULL, &options, &state); 
        } else {
            // Обработка файлов с сохранением состояния между ними
            for (int i = optind; i < argc && error_n == 0; i++) {
                error_n = process_single_file(argv[i], &options, &state);
            }
        }
    }

    return error_n;
}