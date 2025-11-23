#include "s21_cat.h"

void parse_args(int argc, char *argv[], Flags *flags) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Обработка коротких флагов
            if (strcmp(argv[i], "-n") == 0) {
                flags->N_number_all = 1;
            } else if (strcmp(argv[i], "-b") == 0) {
                flags->B_number_nonblank = 1;
            } else if (strcmp(argv[i], "-s") == 0) {
                flags->S_squeeze_blank = 1;
            } else if (strcmp(argv[i], "-e") == 0) {
                flags->E_show_ends = 1;
                flags->V_show_nonprint = 1; // -e подразумевает -v
            } else if (strcmp(argv[i], "-t") == 0) {
                flags->T_show_tabs = 1;
                flags->V_show_nonprint = 1; // -t подразумевает -v
            } else if (strcmp(argv[i], "-v") == 0) {
                flags->V_show_nonprint = 1;
            }
            // Обработка GNU-style длинных флагов
            else if (strcmp(argv[i], "--number") == 0) {
                flags->N_number_all = 1;
            } else if (strcmp(argv[i], "--number-nonblank") == 0) {
                flags->B_number_nonblank = 1;
            } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
                flags->S_squeeze_blank = 1;
            }
        }
    }
}