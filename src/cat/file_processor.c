#include "file_processor.h"
#include "s21_cat.h"

int is_empty_line(const char *line) {
    if (!line) return 1;
    
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] != '\n' && line[i] != '\r' && line[i] != ' ' && line[i] != '\t') {
            return 0;  
        }
    }
    return 1; 
}

int flag_s_use(process_context *Context) { 
    if (!Context->flags->S_squeeze_blank) {
        return 0;
    }

    int is_empty = is_empty_line(Context->line);
    int result = (is_empty && Context->pr_was);
    Context->pr_was = is_empty;
    return result;
}

void flag_n_use(process_context *Context) {
    printf("%6d\t%s", Context->line_num, Context->line);
    Context->line_num++;
}

void flag_b_use(process_context *Context) { 
    if (is_empty_line(Context->line)) {
        printf("%s", Context->line);
    } else {
        printf("%6d\t%s", Context->line_num, Context->line);
        Context->line_num++;
    }
}

void default_outp(const process_context *Context) {
    printf("%s", Context->line);
}

void process_line(process_context *Context) { 
    if (flag_s_use(Context)) {
        return;  
    }

    if (Context->flags->N_number_all) {
        flag_n_use(Context);
    } else if (Context->flags->B_number_nonblank) {
        flag_b_use(Context);
    } else {
        default_outp(Context);
    }
}

int process_file(const char *filename, const Flags *flags) {
    FILE *file = fopen(filename, "r");
    
    if (!file) {
        fprintf(stderr, "No such file or directory");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    int read;
    int line_num = 1;
    int pr_was = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        process_context Context = {
            .flags = flags,
            .line_num = line_num,
            .pr_was = pr_was,
            .line = line
        };

        process_line(&Context);
    
        line_num = Context.line_num;
        pr_was = Context.pr_was;
    }

    free(line);
    fclose(file);
    return 0;
}