#include "memalloc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uintptr_t heap[HEAP_CAPACITY_WORDS] = {0};
const uintptr_t *stack_base = NULL;
bool reachable_chunks[CHUNK_LIST_CAPACITY] = {false};
uintptr_t *to_free_chunks[CHUNK_LIST_CAPACITY] = {NULL};

Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .chunks = {{
        .addr = heap,
        .size = HEAP_CAPACITY_WORDS,
    }},
    .count = 1,
};
Chunk_List temp_chunks = {0};

int chunk_compare(const void *a, const void *b)
{
    Chunk *chunk_a = (Chunk *) a;
    Chunk *chunk_b = (Chunk *) b;
    return (uintptr_t) chunk_a->addr - (uintptr_t) chunk_b->addr;
}

int chunk_list_find(const Chunk_List *chunk_list, void *ptr)
{
    // size_t l = 0, r = chunk_list->count - 1;
    // while (l <= r) {
    //     size_t m = (l + r) / 2;
    //     if (chunk_list->chunks[m].addr < ptr) {
    //         l = m + 1;
    //     } else if (chunk_list->chunks[m].addr > ptr) {
    //         r = m - 1;
    //     } else {
    //         break;
    //     }
    // }

    const Chunk chunk = (Chunk){.addr = ptr};
    void *result = bsearch(&chunk, &chunk_list->chunks, chunk_list->count,
                           sizeof(chunk_list->chunks[0]), &chunk_compare);
    if (result == NULL) {
        return -1;
    }
    return (Chunk *) result - &chunk_list->chunks[0];
}

void chunk_list_insert(Chunk_List *chunk_list, void *ptr, size_t size)
{
    assert(chunk_list->count < CHUNK_LIST_CAPACITY);

    chunk_list->chunks[chunk_list->count] = (Chunk){
        .addr = ptr,
        .size = size,
    };

    for (size_t i = chunk_list->count; i > 0; i--) {
        if (chunk_list->chunks[i].addr < chunk_list->chunks[i - 1].addr) {
            const Chunk temp = chunk_list->chunks[i];
            chunk_list->chunks[i] = chunk_list->chunks[i - 1];
            chunk_list->chunks[i - 1] = temp;
        } else {
            break;
        }
    }

    chunk_list->count++;
}

void chunk_list_remove(Chunk_List *chunk_list, size_t index)
{
    assert(index < chunk_list->count);
    for (size_t i = index; i < chunk_list->count - 1; i++) {
        chunk_list->chunks[i] = chunk_list->chunks[i + 1];
    }
    chunk_list->count--;
}

void chunk_list_merge(const Chunk_List *src, Chunk_List *dst)
{
    dst->count = 0;

    for (size_t i = 0; i < src->count; i++) {
        if (dst->count == 0) {
            chunk_list_insert(dst, src->chunks[i].addr, src->chunks[i].size);
        } else {
            Chunk *const chunk = &dst->chunks[dst->count - 1];

            if (chunk->addr + chunk->size == src->chunks[i].addr) {
                chunk->size += src->chunks[i].size;
            } else {
                chunk_list_insert(dst, src->chunks[i].addr,
                                  src->chunks[i].size);
            }
        }
    }
}

void chunk_list_dump(const Chunk_List *chunk_list, const char *name)
{
    printf("%s chunks (#%zu)\n", name, chunk_list->count);
    for (size_t i = 0; i < chunk_list->count; i++) {
        printf("  addr: %p\t", (void *) chunk_list->chunks[i].addr);
        printf("  size: %zu\n", chunk_list->chunks[i].size);
    }
}

Node *generate_tree(size_t level_cur, size_t level_max)
{
    if (level_cur >= level_max) {
        return NULL;
    }
    Node *node = heap_alloc(sizeof(Node));
    assert(level_cur + 'a' <= 'z');
    node->x = level_cur + 'a';
    node->left = generate_tree(level_cur + 1, level_max);
    node->right = generate_tree(level_cur + 1, level_max);
    return node;
}

static void print_tree_recur(Node *root, Jim *jim)
{
    if (root) {
        jim_object_begin(jim);

        jim_member_key(jim, "value");
        jim_string_sized(jim, &root->x, 1);
        jim_member_key(jim, "left");
        print_tree_recur(root->left, jim);
        jim_member_key(jim, "right");
        print_tree_recur(root->right, jim);

        jim_object_end(jim);
    } else {
        jim_null(jim);
    }
}

void print_tree(Node *root, Jim *jim)
{
    if (root) {
        print_tree_recur(root, jim);
    } else {
        jim_object_begin(jim);
        jim_null(jim);
        jim_object_end(jim);
    }
}

void *heap_alloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    const size_t size_words =
        (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    for (size_t i = 0; i < freed_chunks.count; i++) {
        const Chunk chunk = freed_chunks.chunks[i];
        if (chunk.size >= size_words) {
            size_t tail_size = chunk.size - size_words;

            chunk_list_remove(&freed_chunks, i);
            chunk_list_insert(&alloced_chunks, chunk.addr, size_words);

            if (tail_size > 0) {
                chunk_list_insert(&freed_chunks, chunk.addr + size_words,
                                  tail_size);
            }

            return chunk.addr;
        }
    }
    return NULL;
}

void heap_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    int i = chunk_list_find(&alloced_chunks, ptr);
    assert(i >= 0);
    if (i < 0) {
        return;
    }
    const Chunk chunk = alloced_chunks.chunks[i];
    chunk_list_remove(&alloced_chunks, i);
    chunk_list_insert(&freed_chunks, chunk.addr, chunk.size);
}

// using DFS to collect reachable chunks in heap
static void mark_region(const uintptr_t *start, const uintptr_t *end)
{
    for (; start <= end; start++) {
        const uintptr_t *p = (uintptr_t *) (*start);
        for (size_t i = 0; i < alloced_chunks.count; i++) {
            const Chunk *chunk = &alloced_chunks.chunks[i];
            if (p >= chunk->addr && p < chunk->addr + chunk->size &&
                !reachable_chunks[i]) {
                reachable_chunks[i] = true;
                mark_region(chunk->addr, chunk->addr + chunk->size);
            }
        }
    }
}

void heap_collect(void)
{
    const uintptr_t *stack_start = __builtin_frame_address(0);

    // mark all reachable chunks recursively
    memset(reachable_chunks, 0, sizeof(reachable_chunks));
    mark_region(stack_start, stack_base);

    // remove all unreachable chunks
    memset(to_free_chunks, 0, sizeof(to_free_chunks));
    size_t to_free_chunks_count = 0;
    for (size_t i = 0; i < alloced_chunks.count; i++) {
        if (!reachable_chunks[i]) {
            assert(to_free_chunks_count < CHUNK_LIST_CAPACITY);
            to_free_chunks[to_free_chunks_count++] =
                alloced_chunks.chunks[i].addr;
        }
    }
    for (size_t i = 0; i < to_free_chunks_count; i++) {
        heap_free(to_free_chunks[i]);
    }

    // merge adjacent chunks
    chunk_list_merge(&freed_chunks, &temp_chunks);
    freed_chunks = temp_chunks;
}

int main(void)
{
    stack_base = (uintptr_t *) __builtin_frame_address(0);
    assert((uintptr_t) stack_base % sizeof(uintptr_t) == 0);

    Node *root = generate_tree(0, 5);
    Jim jim = {
        .sink = stdout,
        .write = (Jim_Write) fwrite,
    };
    print_tree(root, &jim);

    printf("\n--------------------------\n");

    heap_collect();

    chunk_list_dump(&alloced_chunks, "allocated");
    chunk_list_dump(&freed_chunks, "freed");

    printf("\n--------------------------\n");

    root = NULL;
    heap_collect();

    chunk_list_dump(&alloced_chunks, "allocated");
    chunk_list_dump(&freed_chunks, "freed");

    return 0;
}
