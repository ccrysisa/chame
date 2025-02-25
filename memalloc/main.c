#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define unimplemented()                                                      \
    do {                                                                     \
        printf("%s:%d %s is unimplemented\n", __FILE__, __LINE__, __func__); \
        assert(false);                                                       \
    } while (0)

#define HEAP_CAPACITY (2 << 19)  // 512 KB
#define CHUNK_LIST_CAPACITY 1024

uint8_t heap[HEAP_CAPACITY] = {0};
// size_t heap_size = 0;

// Metadata of heap area
typedef struct {
    void *addr;
    size_t size;
} Chunk;

typedef struct {
    Chunk chunks[CHUNK_LIST_CAPACITY];
    size_t count;
} Chunk_List;

Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .chunks = {{
        .addr = &heap,
        .size = HEAP_CAPACITY,
    }},
    .count = 1,
};
Chunk_List temp_chunks = {0};

static int chunk_compare(const void *a, const void *b)
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

            if ((char *) chunk->addr + chunk->size == src->chunks[i].addr) {
                chunk->size += src->chunks[i].size;
            } else {
                chunk_list_insert(dst, src->chunks[i].addr,
                                  src->chunks[i].size);
            }
        }
    }
}

void chunk_list_dump(Chunk_List *chunk_list)
{
    printf("chunks (#%zu)\n", chunk_list->count);
    for (size_t i = 0; i < chunk_list->count; i++) {
        printf("  addr: %p\t", chunk_list->chunks[i].addr);
        printf("  size: %zu\n", chunk_list->chunks[i].size);
    }
}

void *heap_alloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    chunk_list_merge(&freed_chunks, &temp_chunks);
    freed_chunks = temp_chunks;

    for (size_t i = 0; i < freed_chunks.count; i++) {
        const Chunk chunk = freed_chunks.chunks[i];
        if (chunk.size >= size) {
            size_t tail_size = chunk.size - size;

            chunk_list_remove(&freed_chunks, i);
            chunk_list_insert(&alloced_chunks, chunk.addr, size);

            if (tail_size > 0) {
                chunk_list_insert(&freed_chunks,
                                  (void *) ((uintptr_t) chunk.addr + size),
                                  tail_size);
            }

            return chunk.addr;
        }
    }
    return NULL;

    // assert(heap_size + size <= HEAP_CAPACITY);
    // void *ptr = heap + heap_size;
    // heap_size += size;
    // chunk_list_insert(&alloced_chunks, ptr, size);
    // return ptr;
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

void heap_collect(void)
{
    unimplemented();
}

int main(void)
{
    for (size_t i = 0; i < 16; i++) {
        void *root = heap_alloc(i);
        if (i % 2 == 0) {
            heap_free(root);
        }
    }

    heap_alloc(16);

    printf("Heap alloced:\n");
    chunk_list_dump(&alloced_chunks);
    printf("Heap freed:\n");
    chunk_list_dump(&freed_chunks);

    heap_collect();

    return 0;
}
