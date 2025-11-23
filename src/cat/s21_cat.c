#include "s21_cat.h"
#include "file_processor.h"

int main(int argc, char *argv[]) {
    Flags flags = {0};
    parse_args(argc, argv, &flags);
    int count_files = 0;
    for (int i = 1; i < argc; i++){
        if (argv[i][0] != '-') {
            count_files++;
        }
    }

    if (count_files == 0) {
        fprintf(stderr, "Error, missing files");
        return 0;
    }

    char **files = malloc(count_files * sizeof(char*));
    if (!files) {
        fprintf(stderr, "Error in malloc");
        return 0;
    }

    int index = 0;
    for (int i = 1; i < argc; i++) {
        if(argv[i][0] != '-') {
            files[index] = argv[i];
            index++;
        }
    }

    for (int i = 0; i < count_files; i++) {
        process_file(files[i], &flags);
    }

    free(files);
    return 0;
}