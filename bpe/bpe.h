#ifndef __BPE_H
#define __BPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define swap(type, x, y) \
    do {                 \
        type t = x;      \
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

void render_tokens(uint32_t *tokens, Pair *pairs);
void generate_rules(FILE *stream, Pair *pairs);
void load_rules_from_file(Pair **pairs, const char *file_path);
void save_rules_to_file(Pair *pairs, const char *file_path);
void byte_pair_encode(const char *text);

const char *shift_args(int *argc, char ***argv);

// dynamic array
extern uint32_t *tokens_in;   // original text by replacing char to uint32_t
extern uint32_t *tokens_out;  // compressed text
extern Pair *pairs;           // grammer rule table

// hash map
extern Freq *freq_hm;  // store byte pair's frequence

#endif
