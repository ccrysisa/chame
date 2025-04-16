#include <stdio.h>
#include <string.h>
#include "bpe.h"

int main(int argc, char **argv)
{
    const char *program = shift_args(&argc, &argv);
    const char *output_file_path = NULL;

    const char *const text =
        "The original BPE algorithm operates by iteratively replacing the most "
        "common contiguous sequences of characters in a target text with "
        "unused 'placeholder' bytes. The iteration ends when no sequences can "
        "be found, leaving the target text effectively compressed. "
        "Decompression can be performed by reversing this process, querying "
        "known placeholder terms against their corresponding denoted sequence, "
        "using a lookup table. In the original paper, this lookup table is "
        "encoded and stored alongside the compressed text.";

    while (argc > 0) {
        const char *flag = shift_args(&argc, &argv);

        if (!strcmp(flag, "-h")) {
            fprintf(stdout, "Usage: %s -o <output.bin> [-h]\n", program);
            return 0;
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

    if (output_file_path == NULL) {
        fprintf(stderr, "Error: output file is not provided\n");
        return 1;
    }

    byte_pair_encode(text);
    save_rules_to_file(pairs, output_file_path);

    // for (size_t i = 0; i < arrlenu(pairs); i++) {
    //     printf("%zu: (%u, %u)\n", i, pairs[i].l, pairs[i].r);
    // }

    /*
        // sort byte pair by its frequence
        Freq *freqs = NULL;  // dynamic array
        for (size_t i = 0; i < hmlenu(freq_hm); i++) {
            arrput(freqs, freq_hm[i]);
        }

        qsort(freqs, arrlenu(freqs), sizeof(*freqs), freq_compare);

        for (size_t i = 0; i < arrlenu(freqs); i++) {
        for (size_t i = 0; i < 10; i++) {
            const Freq *freq = &freqs[i];
            printf("(%u, %u)\t=> %zu\n", freq->key.l, freq->key.r, freq->value);
        }

    */

    return 0;
}
