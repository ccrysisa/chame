# memalloc

memalloc is a simple allocator which implemented [linear allocating](https://en.wikipedia.org/wiki/C_dynamic_memory_allocation#Heap-based) with fixed size chunks. It only supports to collect aligned address in the heap and stack.

## Quick start

```sh
$ make
$ ./memalloc
```

## References

- Tsoding: [memalloc](https://github.com/tsoding/memalloc/)
- JSON Serialization Library in C: https://github.com/tsoding/jim
- IBM: [Data alignment: Straighten up and fly right](https://developer.ibm.com/articles/pa-dalign/)
