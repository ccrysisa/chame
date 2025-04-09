#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#define swap(x, y)       \
    do {                 \
        typeof(x) t = x; \
        (x) = y;         \
        (y) = t;         \
    } while (0)

typedef struct Pair {
    uint32_t l;
    uint32_t r;
} Pair;

typedef struct Freq {
    Pair key;
    size_t value;
} Freq;

static int freq_compare(const void *a, const void *b)
{
    const Freq *fa = a;
    const Freq *fb = b;
    return -(fa->value < fb->value ? -1 : fa->value > fb->value ? 1 : 0);
}

// arguments tokens and pairs are both dynamic array.
static void render_tokens(uint32_t *tokens, Pair *pairs)
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

static void generate_rules(Pair *pairs)
{
    printf("digraph Rules {\n");
    for (size_t i = 0; i < arrlenu(pairs); i++) {
        if (i == (size_t) pairs[i].l) {
            continue;
        }
        printf("  %zu -> %u\n", i, pairs[i].l);
        printf("  %zu -> %u\n", i, pairs[i].r);
    }
    printf("}\n");
}

int main(void)
{
    const char *const text =
        "The original BPE algorithm operates by iteratively replacing the most "
        "common contiguous sequences of characters in a target text with "
        "unused 'placeholder' bytes. The iteration ends when no sequences can "
        "be found, leaving the target text effectively compressed. "
        "Decompression can be performed by reversing this process, querying "
        "known placeholder terms against their corresponding denoted sequence, "
        "using a lookup table. In the original paper, this lookup table is "
        "encoded and stored alongside the compressed text.";
    const size_t text_size = strlen(text);

    // dynamic array
    uint32_t *tokens_in = NULL;   // original text by replacing char to uint32_t
    uint32_t *tokens_out = NULL;  // compressed text
    Pair *pairs = NULL;           // grammer rule table

    // hash map
    Freq *freq_hm = NULL;  // store byte pair's frequence

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

    render_tokens(tokens_in, pairs);

    for (;;) {
        printf("%zu\n", arrlenu(tokens_in));

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

        swap(tokens_in, tokens_out);
    }

    render_tokens(tokens_in, pairs);
    printf("Grammer rules: %zu (with 256 leaf rules)\n", arrlenu(pairs));
    generate_rules(pairs);

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
