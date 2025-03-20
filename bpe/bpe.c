#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct Pair {
    char pair[2];
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

Freq *freq_hm = NULL;  // hash map

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
    const int text_size = strlen(text);

    /// collect byte pair and its frequence
    for (int i = 0; i < text_size - 1; i++) {
        Pair key = {.pair = {text[i], text[i + 1]}};

        ptrdiff_t i = hmgeti(freq_hm, key);
        if (i < 0) {
            hmput(freq_hm, key, 1);
        } else {
            freq_hm[i].value++;
        }
    }

    // sort byte pair by its frequence
    Freq *freqs = NULL;  // dynamic array
    for (size_t i = 0; i < hmlenu(freq_hm); i++) {
        arrput(freqs, freq_hm[i]);
    }

    qsort(freqs, arrlenu(freqs), sizeof(*freqs), freq_compare);

    for (size_t i = 0; i < arrlenu(freqs); i++) {
        const Freq *freq = &freqs[i];
        printf("%c%c => %zu\n", freq->key.pair[0], freq->key.pair[1],
               freq->value);
    }

    return 0;
}
