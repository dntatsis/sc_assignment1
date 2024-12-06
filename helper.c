#include <stdio.h>
#include <stdbool.h>
#include "helper.h"

    // Convert a string to lowercase
    char *
    strlwr(char *str)
{
    unsigned char *p = (unsigned char *)str;

    while (*p)
    {
        *p = tolower((unsigned char)*p); // Convert each character to lowercase
        p++;
    }

    return str;
}

// Extract the first k words from a given text
char **extract_first_k_words(const char *text, int k)
{
    if (k <= 0)
        return NULL; // Return NULL if k is invalid

    // Allocate memory for the word list
    char **word_list = malloc(k * sizeof(char *));
    if (!word_list)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int word_count = 0;                   // Count of words extracted
    size_t word_len = 0;                  // Length of the current word
    char word[MAX_WORD_LENGTH + 1] = {0}; // Buffer for the current word

    // Iterate over the text to extract words
    for (const char *p = text; *p != '\0'; p++)
    {
        // Check for delimiters (whitespace, punctuation)
        if (isspace(*p) || *p == ',' || *p == '?' || *p == '!' || *p == '-' || *p == '\'' || *p == ';')
        {
            if (word_len > 0) // If a word is found
            {
                word[word_len] = '\0';                // Null-terminate the word
                word_list[word_count] = strdup(word); // Duplicate the word into the list
                if (!word_list[word_count])
                {
                    fprintf(stderr, "Memory allocation failed for word\n");
                    for (int i = 0; i < word_count; i++)
                        free(word_list[i]); // Free previously allocated words
                    free(word_list);
                    return NULL;
                }
                word_count++;
                if (word_count == k) // Stop if k words are extracted
                    break;
                word_len = 0; // Reset word length for the next word
            }
        }
        else
        {
            if (word_len < MAX_WORD_LENGTH) // Append character if within word length
            {
                word[word_len++] = tolower(*p);
            }
        }
    }

    // Add the last word if it exists
    if (word_len > 0 && word_count < k)
    {
        word[word_len] = '\0';
        word_list[word_count++] = strdup(word);
    }

    // Fill remaining slots with NULL if fewer than k words are found
    for (int i = word_count; i < k; i++)
    {
        word_list[i] = NULL;
    }

    return word_list;
}

// Function to validate words against a dictionary
int validate_words_with_dictionary(char **words, int k)
{
    int counter = 0; // Count of valid words

    for (int j = 0; j < k; j++)
    {
        if (!words[j])
            continue; // Skip NULL entries

        FILE *cmd;
        char result[100];
        char command[150] = "grep -i -w "; // Grep command to search for words
        strcat(command, words[j]);
        strcat(command, " ./dict.txt"); // Search in the dictionary file

        cmd = popen(command, "r"); // Execute the command
        if (cmd == NULL)
        {
            perror("popen"); // Handle error in opening the process
            exit(EXIT_FAILURE);
        }

        bool found = false;
        // Read results from the command output
        while (fgets(result, sizeof(result), cmd))
        {
            result[strcspn(result, "\n")] = '\0';      // Remove newline character
            if (strcmp(strlwr(result), words[j]) == 0) // Compare words case-insensitively
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            counter++; // Increment valid word count
        }
        pclose(cmd); // Close the process
    }

    return counter; // Return the number of valid words
}
