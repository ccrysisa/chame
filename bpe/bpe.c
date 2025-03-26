#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct Pair {
    uint32_t l;
    uint32_t r;
} Pair;

typedef struct Freq {
    Pair key;
    size_t value;
} Freq;

Freq *freq_hm = NULL;  // hash map which store byte pair's frequence

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

    for (size_t i = 0; i < text_size; i++) {
        arrput(tokens_in, (uint32_t) text[i]);
    }

    // init grammer rule table
    for (size_t i = 0; i < 256; i++) {
        arrput(pairs, (Pair){.l = i});
    }

    // collect byte pair and its frequence
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
    const Freq *const freq = &freq_hm[max_freq_index];
    printf("(%u, %u)\t=> %zu\n", freq->key.l, freq->key.r, freq->value);
    printf("(%c, %c)\t=> %zu\n", freq->key.l, freq->key.r, freq->value);
    arrput(pairs, freq_hm[max_freq_index].key);

    // compress text by replacing byte pair with grammer rule
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

    render_tokens(tokens_in, pairs);
    render_tokens(tokens_out, pairs);
    printf("%zu\n", arrlenu(tokens_in));
    printf("%zu\n", arrlenu(tokens_out));

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
