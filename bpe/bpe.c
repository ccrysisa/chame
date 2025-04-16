#include "bpe.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

static int freq_compare(const void *a, const void *b)
{
    const Freq *fa = a;
    const Freq *fb = b;
    return -(fa->value < fb->value ? -1 : fa->value > fb->value ? 1 : 0);
}

// arguments tokens and pairs are both dynamic array.
void render_tokens(uint32_t *tokens, Pair *pairs)
{
    for (size_t i = 0; i < arrlenu(tokens); i++) {
        assert(tokens[i] < arrlenu(pairs));
        if (pairs[tokens[i]].l == tokens[i]) {
            printf("%c", tokens[i]);
        } else {
            printf("[%u]", tokens[i]);
        }
    }
    printf("\n");
}

void generate_rules(FILE *stream, Pair *pairs)
{
    fprintf(stream, "digraph Rules {\n");
    for (size_t i = 0; i < arrlenu(pairs); i++) {
        if (i == (size_t) pairs[i].l) {
            continue;
        }
        fprintf(stream, "  %zu -> %u\n", i, pairs[i].l);
        fprintf(stream, "  %zu -> %u\n", i, pairs[i].r);
    }
    fprintf(stream, "}\n");
}

void load_rules_from_file(Pair **pairs, const char *file_path)
{
    assert(*pairs == NULL);

    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    assert(m % sizeof(**pairs) == 0);

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    Pair *temp = malloc(m);
    size_t n = fread(temp, sizeof(**pairs), m / sizeof(**pairs), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);

    for (size_t i = 0; i < n; i++) {
        arrput(*pairs, temp[i]);
    }

    free(temp);
}

void save_rules_to_file(Pair *pairs, const char *file_path)
{
    FILE *f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Could not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fwrite(pairs, sizeof(*pairs), arrlenu(pairs), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);
}

void byte_pair_encode(const char *text)
{
    const size_t text_size = strlen(text);

    for (size_t i = 0; i < text_size; i++) {
        arrput(tokens_in, (uint32_t) text[i]);
    }

    // init grammer rule table
    // ```
    //  0  -> { .l=0, .r=??? }
    //  1  -> { .l=1, .r=??? }
    // ...
    // 256 -> { .l=256, .r=??? }
    // ```
    for (size_t i = 0; i < 256; i++) {
        arrput(pairs, (Pair){.l = i});
    }

    printf("Origin text (%zu):\n", arrlenu(tokens_in));
    render_tokens(tokens_in, pairs);

    for (;;) {
        // collect byte pair and its frequence
        hmfree(freq_hm);
        freq_hm = NULL;
        for (int i = 0; i < arrlen(tokens_in) - 1; i++) {
            Pair key = {tokens_in[i], tokens_in[i + 1]};

            ptrdiff_t i = hmgeti(freq_hm, key);
            if (i < 0) {
                hmput(freq_hm, key, 1);
            } else {
                freq_hm[i].value++;
            }
        }

        // add the most frequent pair to grammer rule table
        size_t max_freq_index = 0;
        for (size_t i = 0; i < hmlenu(freq_hm); i++) {
            if (freq_hm[i].value > freq_hm[max_freq_index].value) {
                max_freq_index = i;
            }
        }
        // compression is done
        if (freq_hm[max_freq_index].value <= 1) {
            break;
        }
        arrput(pairs, freq_hm[max_freq_index].key);

        // compress text by replacing byte pair with grammer rule
        arrfree(tokens_out);
        tokens_out = NULL;
        for (size_t i = 0; i < arrlenu(tokens_in);) {
            if (i + 1 >= arrlenu(tokens_in)) {
                arrput(tokens_out, tokens_in[i]);
                i++;
            } else {
                if (!memcmp(&freq_hm[max_freq_index].key, &tokens_in[i],
                            sizeof(Pair))) {
                    arrput(tokens_out, arrlenu(pairs) - 1);
                    i += 2;
                } else {
                    arrput(tokens_out, tokens_in[i]);
                    i++;
                }
            }
        }

        swap(uint32_t *, tokens_in, tokens_out);
    }

    printf("Compressed text (%zu):\n", arrlenu(tokens_in));
    render_tokens(tokens_in, pairs);

    printf("Grammer rules: %zu (with 256 leaf rules)\n", arrlenu(pairs));
}

const char *shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

// dynamic array
uint32_t *tokens_in = NULL;
uint32_t *tokens_out = NULL;
Pair *pairs = NULL;

// hash map
Freq *freq_hm = NULL;
