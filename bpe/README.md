# LLM Tokenizer (BPE)

Using [BPE](https://en.wikipedia.org/wiki/Byte_pair_encoding) algorithm to implement/recreate a LLM tokenizer.

## Quick start

```sh
$ make
$ ./build/bper -o <rules.bin>
$ ./build/bpe2dot -i <rules.bin> -o <rules.dot>
$ dot -Tpng <rules.dot> > <rules.png>
```

## References

- Wikipedia: [Byte pair encoding](https://en.wikipedia.org/wiki/Byte_pair_encoding)
- Wikipedia: [Backus–Naur form (BNF)](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form)
