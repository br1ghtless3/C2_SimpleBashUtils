#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int N_number_all; // -n
    int B_number_nonblank; // -b
    int E_show_ends; // -e
    int S_squeeze_blank; // -s
    int T_show_tabs; // -t
    int V_show_nonprint; // -v
} Flags;

void parse_args(int argc, char *argv[], Flags *flags);

#endif