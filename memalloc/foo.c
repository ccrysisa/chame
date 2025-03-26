// A file to explain and test data alignment in real hardware.
// https://developer.ibm.com/articles/pa-dalign

#include <stdio.h>
#define HEAP_CAP (2 << 19)

char heap[HEAP_CAP];  // 512 byte

typedef struct Foo {
    char x;
    char y;
    void *p;
} Foo;

Foo foo = {0};

int main(void)
{
    printf("start address of heap: %p\n", (void *) heap);
    printf("heap %% sizeof(void *): %zu\n", (size_t) heap % sizeof(void *));

    printf("sizeof(char)   ==  %zu\n", sizeof(char));
    printf("sizeof(void *) ==  %zu\n", sizeof(void *));
    printf("sizeof(Foo)    ==  %zu\n", sizeof(Foo));

    printf("address of foo:    %p\n", (void *) &foo);
    printf("address of foo.x:  %p\n", (void *) &foo.x);
    printf("address of foo.y:  %p\n", (void *) &foo.y);
    printf("address of foo.p:  %p\n", (void *) &foo.p);

    return 0;
}
