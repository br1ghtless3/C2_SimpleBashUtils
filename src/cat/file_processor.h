#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#define _POSIX_C_SOURCE 200809L
#include "s21_cat.h"

typedef struct {
    const Flags *flags;
    int line_num; // номер текущей
    int pr_was; // предыдущая - пустая?
    const char *line; // текущая строка
} process_context;

int process_file(const char *filename, const Flags *flags);
int flag_s_use(process_context *Context);
void flag_n_use(process_context *Context);
void flag_b_use(process_context *Context);
void default_outp(const process_context *Context);
int is_empty_line(const char *line);
void process_line(process_context *Context); 

#endif