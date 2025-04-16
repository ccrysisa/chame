#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "bpe.h"

int main(int argc, char **argv)
{
    const char *program = shift_args(&argc, &argv);
    const char *input_file_path = NULL;
    const char *output_file_path = NULL;

    while (argc > 0) {
        const char *flag = shift_args(&argc, &argv);

        if (!strcmp(flag, "-h")) {
            fprintf(stdout, "Usage: %s -i <input.bin> -o <output.dot> [-h]\n",
                    program);
            return 0;
        } else if (!strcmp(flag, "-i")) {
            if (argc == 0) {
                fprintf(stderr, "ERROR: expect input file\n");
                return 1;
            }
            input_file_path = shift_args(&argc, &argv);
        } else if (!strcmp(flag, "-o")) {
            if (argc == 0) {
                fprintf(stderr, "ERROR: expect output file\n");
                return 1;
            }
            output_file_path = shift_args(&argc, &argv);
        } else {
            fprintf(stderr, "ERROR: unknown flag `%s`\n", flag);
            return 1;
        }
    }

    if (input_file_path == NULL) {
        fprintf(stderr, "Error: input file is not provided\n");
        return 1;
    }
    if (output_file_path == NULL) {
        fprintf(stderr, "Error: output file is not provided\n");
        return 1;
    }

    load_rules_from_file(&pairs, input_file_path);

    FILE *f = fopen(output_file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not open file `%s`: %s\n",
                output_file_path, strerror(errno));
        exit(1);
    }
    generate_rules(f, pairs);

    return 0;
}
