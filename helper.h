#define MAXC 1500 // Maximum size of ciphertext / plaintext
#define MAX_WORD_LENGTH 30
#define CHECKED_WORDS 5
#define MAXIMUM_NGRAPHS 10
#define PERCENTAGE_REQ 50
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

char *strlwr(char *str);
char **extract_first_k_words(const char *text, int k);
int validate_words_with_dictionary(char **words, int k);

