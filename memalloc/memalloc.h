#ifndef __MEMALLOC_H
#define __MEMALLOC_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define JIM_IMPLEMENTATION
#include "jim.h"

#define unimplemented()                                                      \
    do {                                                                     \
        printf("%s:%d %s is unimplemented\n", __FILE__, __LINE__, __func__); \
        abort();                                                             \
    } while (0)

#define HEAP_CAPACITY (1 << 19)  // 512 KB
// #define HEAP_CAPACITY 64000
#define CHUNK_LIST_CAPACITY 1024

static_assert(HEAP_CAPACITY % sizeof(uintptr_t) == 0,
              "The heap capacity is not disible by the size of the pointer of "
              "the platform.");
#define HEAP_CAPACITY_WORDS (HEAP_CAPACITY / sizeof(uintptr_t))

// Metadata of heap area
typedef struct {
    void *addr;
    size_t size;
} Chunk;

typedef struct {
    Chunk chunks[CHUNK_LIST_CAPACITY];
    size_t count;
} Chunk_List;

int chunk_compare(const void *a, const void *b);
int chunk_list_find(const Chunk_List *chunk_list, void *ptr);
void chunk_list_insert(Chunk_List *chunk_list, void *ptr, size_t size);
void chunk_list_remove(Chunk_List *chunk_list, size_t index);
void chunk_list_merge(const Chunk_List *src, Chunk_List *dst);
void chunk_list_dump(Chunk_List *chunk_list);

typedef struct Node {
    char x;
    struct Node *left;
    struct Node *right;
} Node;

Node *generate_tree(size_t level_cur, size_t level_max);
void print_tree(Node *root, Jim *jim);

void *heap_alloc(size_t size);
void heap_free(void *ptr);
void heap_collect(void);

#endif
