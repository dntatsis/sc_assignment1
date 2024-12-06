# Security and Cryptography Assignment 1

File structure:
```
.
├── dict.txt (contains dictionary for comparison purposes, [source](https://www.ef.com/wwen/english-resources/english-vocabulary/top-3000-words/) )
├── README.md 
├── hashdict.c (contains hash table implementation, [source](https://github.com/exebook/hashdict.c))
├── hashdict.h
├── caesar.c (used to solve exercise 1A)
└── vigenere.c (used to solve exercise 1B)
```

The programs can be compiled as follows:
` gcc -o caesar caesar.c helper.c`
` gcc -o vigenere vigenere.c helper.c hashdict.c `

and be run using:

` ./caesar `

` ./vigenere `

Constants that may be useful for improving the heuristic of key validity can be modified in helper.h.
