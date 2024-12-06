#define MAXC 1500
#define MAX_WORD_LENGTH 29
#include <stdbool.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "hashdict.h"

int gcd (int a, int b) 
{
	int r; // remainder
	while (b > 0) 
	{
		r = a % b;
		a = b;
		b = r;
	}

	return a;
}

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}
void clean_string(const char *source, char * dest) {

    for (int i=0, j = 0; i< strlen(source); i++) {
        if(source[i] >= 'A' && source[i] <= 'Z') {
            dest[j++] = tolower(source[i]);
        }
        else if (source[i] >= 'a' && source[i] <= 'z') {
            dest[j++] = source[i];
        }
        else if (source[i] == '\0') {
            dest[j++] = source[i];
            break;
        }
    }
    return;
}

int extract_ngram_max(struct dictionary *dict, const int n, const char *ciphertext){
    int nmax = 1;
    for (int i = 0; i< strlen(ciphertext) - 1; i++){
        char ngram[n+1];
        for (int j = 0; j < n; j++){
            ngram[j] = ciphertext[i+j];
        }
        ngram[n] = '\0';

        if(dic_add(dict,ngram,n) == false) {
            *dict->value = 1;
        }
        else {
            *dict->value += 1;
            if (*dict->value > nmax) {
                nmax = *dict->value;
            }
        }
        if(n == 2 && ngram[0] == ngram[1] && ngram[1] != ciphertext[i+2]){
            i++;
            continue;

        }
    }
    return nmax;

}

void generate_next_permutation(char *string) {
    for (int i = strlen(string) - 1; i >= 0; i--){
        if (string[i] == 'z') {
            string[i] = 'a'; // Wrap around to 'a'
        } else {
            string[i]++; // Increment the character
            return;   // Stop once the increment is successful
        }
    }
}

void extract_ngram_list_dist (struct dictionary *dict, const int n, const char *ciphertext, const int ngram_max, char ** ngram_list, int ** ngram_dist, int*nummax, int*maxcnt){

    int counter;
    int counter2;
    counter = 0;
    char ngram[n+1];
    char term_ngram[n+1];
    for(int i = 0; i < n; i++){
        ngram[i] = 'a';
        term_ngram[i] = 'z';
    }
    ngram[n] = '\0';
    term_ngram[n] = '\0';
    while (strcmp(ngram,term_ngram) != 0) {
        if(dic_find(dict,ngram,n) == true && ngram_max == *dict->value) {
            strcpy(ngram_list[(counter)++],ngram);
        }
        if(counter == 10){ break;}
        generate_next_permutation(ngram);
    }
    counter2 = 0; // maximum occurences of one of the most common n_grams
    for (int i = 0; i < counter; i++){
        printf("Distances between instances of %d'th Max %dgraph (%s) are: ",i,n,ngram_list[i]);
        int ppos = -1;
        int cnt = 0;
        for (int j = 0; j < strlen(ciphertext) - (n-1); j++){

            char ngram[n+1];
            for (int k = 0; k < n; k++){
                ngram[k] = ciphertext[j+k];
            }
            ngram[n] = '\0';
            if(strcmp(ngram,ngram_list[i]) == 0){
                if(ppos == -1){
                    ngram_dist[i][cnt++] = 0;
                }
                else{
                    ngram_dist[i][cnt++] = j - ppos;
                }
                ppos = j;
                printf(" %d,", ngram_dist[i][cnt-1]);
            }
        }

        if(cnt > counter2){
            counter2 = cnt;
        }

        printf("\n");
    }
    *nummax =   counter;
    *maxcnt = counter2;

    return; 
}

char* decrypt_ciphertext(const char *ciphertext, const char *key, int key_length) {
    // Allocate memory for the decrypted plaintext
    size_t text_length = strlen(ciphertext);
    char *decrypted_plaintext = malloc((text_length + 1) * sizeof(char)); // +1 for null terminator
    if (!decrypted_plaintext) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int cnt = 0; // Counter for key cycling

    for (size_t j = 0; j < text_length; j++) {
        int keyc = key[cnt % key_length] - 'a'; // Current key character as shift value

        if (isupper(ciphertext[j])) {
            // Handle uppercase letters
            decrypted_plaintext[j] = toupper((tolower(ciphertext[j]) - 'a' + 26 - keyc) % 26 + 'a');
            cnt++; // Increment key counter for alphabetic characters
        } else if (islower(ciphertext[j])) {
            // Handle lowercase letters
            decrypted_plaintext[j] = (ciphertext[j] - 'a' + 26 - keyc) % 26 + 'a';
            cnt++; // Increment key counter for alphabetic characters
        } else {
            // Copy non-alphabetic characters as-is
            decrypted_plaintext[j] = ciphertext[j];
        }
    }

    // Null-terminate the decrypted plaintext
    decrypted_plaintext[text_length] = '\0';
    return decrypted_plaintext;
}


char** extract_first_k_words(const char *text, int k) {
    if (k <= 0) return NULL;

    char **word_list = malloc(k * sizeof(char *));
    if (!word_list) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int word_count = 0;
    size_t word_len = 0;
    char word[MAX_WORD_LENGTH + 1] = {0};

    for (const char *p = text; *p != '\0'; p++) {
        if (isspace(*p) || *p == ',' || *p == '?' || *p == '!' || *p == '-' || *p == '\'' || *p == ';') {
            if (word_len > 0) {
                word[word_len] = '\0';
                word_list[word_count] = strdup(word);
                if (!word_list[word_count]) {
                    fprintf(stderr, "Memory allocation failed for word\n");
                    for (int i = 0; i < word_count; i++) free(word_list[i]);
                    free(word_list);
                    return NULL;
                }
                word_count++;
                if (word_count == k) break;
                word_len = 0;
            }
        } else {
            if (word_len < MAX_WORD_LENGTH) {
                word[word_len++] = tolower(*p);
            }
        }
    }

    if (word_len > 0 && word_count < k) {
        word[word_len] = '\0';
        word_list[word_count++] = strdup(word);
    }

    for (int i = word_count; i < k; i++) {
        word_list[i] = NULL;
    }

    return word_list;
}

// Function to validate words against a dictionary
int validate_words_with_dictionary(char **words, int k) {
    int counter = 0;
    for (int j = 0; j < k; j++) {
        if (!words[j]) continue;

        FILE *cmd;
        char result[100];
        char command[150] = "grep -i -w ";
        strcat(command, words[j]);
        strcat(command, " ./20k.txt");

        cmd = popen(command, "r");
        if (cmd == NULL) {
            perror("popen");
            exit(EXIT_FAILURE);
        }

        bool found = false;
        while (fgets(result, sizeof(result), cmd)) {
            result[strcspn(result, "\n")] = '\0'; // Remove newline
            if (strcmp(strlwr(result), words[j]) == 0) {
                found = true;
                break;
            }
        }

        if (found) counter++;

        pclose(cmd);
    }

    return counter;
}

void calculate_key(const char *ciphertext_clean, int current_gcd, const float *default_frequency, char *key) {
    // Loop through each segment of the ciphertext
    for (int j = 0; j < current_gcd; j++) {
        float cipher_frequency[26] = {0.0};
        float statistical_distance[26] = {0.0};
        int cnt = 0;

        // Calculate cipher frequencies for this segment
        for (int k = j; k < strlen(ciphertext_clean); k += current_gcd) {
            cipher_frequency[ciphertext_clean[k] - 'a'] += 1.0;
            cnt++;
        }

        // Normalize the frequencies to percentages
        for (int k = 0; k < 26; k++) {
            cipher_frequency[k] = (cipher_frequency[k] / cnt) * 100.0;
        }

        // Calculate the statistical distance for each possible shift
        for (int k = 0; k < 26; k++) {
            for (int l = 0; l < 26; l++) {
                statistical_distance[k] += 0.5 * fabs(default_frequency[l] - cipher_frequency[(l + k) % 26]);
            }
        }

        // Find the shift with the minimum statistical distance
        int best_shift = 0;
        float min_distance = statistical_distance[0];
        for (int k = 1; k < 26; k++) {
            if (statistical_distance[k] < min_distance) {
                min_distance = statistical_distance[k];
                best_shift = k;
            }
        }

        // Store the key character for this segment
        key[j] = (char)('a' + best_shift);
    }

    // Null-terminate the key
    key[current_gcd] = '\0';
}

void decrypt_plaintexts (int *number_of_max_digrams, int **ngram_max_gcds, const char * ciphertext_original, const char * ciphertext_clean) {

    int counter = 0;
    int * checked_gcds = malloc(100*sizeof(int));
    for(int i=0; i < *number_of_max_digrams ; i++){
        const int number_of_gcds = ngram_max_gcds[i][0];
        for (int l=1; l < 1 + number_of_gcds; l++){         // check that ngram_max_gcds[i][l] hasn't been checked so far, if it has, skip.
            bool flag = false;
            const int current_gcd = ngram_max_gcds[i][l];
            for(int j = 0; j < counter; j++){
                if(checked_gcds[j] == current_gcd){
                    flag = true;
                    break;
                }
            }
            if(flag == true){
                continue;
            }
            checked_gcds[counter++] = current_gcd;
            char key[current_gcd + 1];
            const float default_frequency[26] = {8.2, 1.5, 2.8, 4.2, 12.7, 2.2, 2.0, 6.1, 7.0, 0.1, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.1, 6.0, 6.3, 9.0, 2.8, 1.0, 2.4, 0.1, 2.0, 0.1};

            calculate_key(ciphertext_clean, current_gcd, default_frequency, key);

            char *decrypted_plaintext = decrypt_ciphertext(ciphertext_original, key,strlen(key));
            const int k = 5;
            char **words = extract_first_k_words(decrypted_plaintext, k);
            printf("Extracted Words: ");
            for (int i = 0; i < k && words[i] != NULL; i++) {
                printf("%d: %s, ", i + 1, words[i]);
            }
            printf("\n");
            const int valid = validate_words_with_dictionary(words, k);

            float ratio = (float)valid / k;
            if (ratio < 0.5){
                printf("Length: %d => Discarded key \"%s\" (produced non-words)\n",current_gcd,key);
                continue;
            }
            printf("\nLength: %d => Key is \"%s\" (w/ success rate %.2f)\n",current_gcd,key,ratio);
            printf("Plaintext is %s\n\n",decrypted_plaintext);
        }

    }
    free(checked_gcds);
    printf("\n");
}

int main () {

    const char* ciphertext = "Gc qhi irjo S brb'l dbma cgg mqs utb uegc lw d hjxs yry fyr es ztdnc vl dfh gazvr sa ad? Frk utb mrz lkjn hg t ayr vfn lrh tx vytkq sl ocnbbe lde! Yf, lh'k hbjc ozkr L'a fhh yfgw dm hlhksqw dl. Klg kzth zivmdgiid mvgrbk dfhfw tfc eo wfcum kmsn, xcsd cysf mvc qjkd frdwesqw hsx kxgl yscp og lc essnhgjpd! Vmry sm o alddn! Jrcc th Esy'k csqfals! Jsjc kr wvw zfywn, zyu lh ykcuw! Ggyi dh las cczk dfdh ytnc eo qys dbv ecti tge!";

	struct dictionary* my_dict = dic_new(0);

    char* ciphertext_clean;
    ciphertext_clean = malloc((strlen(ciphertext) + 1) * sizeof(char));
    if (ciphertext_clean == NULL) {
        printf("Memory error\n");
        return 1;
    }
    clean_string(ciphertext,ciphertext_clean); //remove non letters, make everything lowercase

    printf("Clean string is: %s\n\n",ciphertext_clean);
    const int digram_max = extract_ngram_max(my_dict,2,ciphertext_clean);

    char **digram_list;
    digram_list = malloc(10*sizeof(char *));
    for(int i=0; i<10; i++){
        digram_list[i] = malloc(3*sizeof(char));
    }

    int **digram_dist;
    digram_dist = malloc(10*sizeof(int *));
    for(int i=0; i<10; i++){
        digram_dist[i] = malloc(digram_max*sizeof(int));
    }
// array that contains distances between each instance of one of the most common digrams

    int number_of_max_digrams = 0;
    int maximum_occurences_of_max_digrams = 0;

    extract_ngram_list_dist(my_dict,2,ciphertext_clean,digram_max,digram_list,digram_dist,&number_of_max_digrams,&maximum_occurences_of_max_digrams);

    int **d_max_gcds;
    d_max_gcds = malloc(number_of_max_digrams*sizeof(int *));
    for(int i=0; i<10; i++){
        d_max_gcds[i] = malloc(maximum_occurences_of_max_digrams*sizeof(int));
    }
    for (int i= 0; i< number_of_max_digrams; i++){ // find all gcd of distances between each common digram, requiring that distance >= 3
        int cnt = 0;
        printf("GCDs of distances between instances of %d'th Digram are:",i);
        for (int j = 1; j < maximum_occurences_of_max_digrams; j++){
            for(int k = j + 1; k < maximum_occurences_of_max_digrams; k++){
                int pgcd = gcd(digram_dist[i][j],digram_dist[i][k]);
                bool flag = false;
                for (int l = 1; l < 1+cnt; l++){
                    if(d_max_gcds[i][l] == pgcd){
                        flag = true;
                        break;
                    }
                }
                if(pgcd >= 3 && flag == false) {
                    d_max_gcds[i][1+cnt++] = pgcd;
                }
            }

        }
        printf("(%d) ",cnt);
        d_max_gcds[i][0] = cnt;
        for(int j = 1; j < 1 + cnt; j++){
            printf("%d, ", d_max_gcds[i][j]);
        }
        printf("\n");
    }

    decrypt_plaintexts(&number_of_max_digrams,d_max_gcds,ciphertext,ciphertext_clean);

    const int trigram_max = extract_ngram_max(my_dict,3,ciphertext_clean);
    char **trigram_list;
    trigram_list = malloc(10*sizeof(char *));
    for(int i=0; i<10; i++){
        trigram_list[i] = malloc(4*sizeof(char));
    }
    int **trigram_dist;
    trigram_dist = malloc(10*sizeof(char *));
    for(int i=0; i<10; i++){
        trigram_dist[i] = malloc(trigram_max*sizeof(char));
    }

    int number_of_max_trigrams = 0;
    int maximum_occurences_of_max_trigrams = 0;
    extract_ngram_list_dist(my_dict,3,ciphertext_clean,trigram_max,trigram_list,trigram_dist,&number_of_max_trigrams,&maximum_occurences_of_max_trigrams);
    int **t_max_gcds;
    t_max_gcds = malloc(number_of_max_trigrams *sizeof(int *));
    for(int i=0; i<number_of_max_trigrams; i++){
        t_max_gcds[i] = malloc(maximum_occurences_of_max_trigrams*sizeof(char));
    }
    for (int i= 0; i< number_of_max_trigrams; i++){
        int cnt = 0;
        printf("GCDs of distances between instances of %d'th Digram are:",i);
        for (int j = 1; j < maximum_occurences_of_max_trigrams; j++){
            for(int k = j + 1; k < maximum_occurences_of_max_trigrams; k++){
                int pgcd = gcd(trigram_dist[i][j],trigram_dist[i][k]);
                bool flag = false;
                for (int l = 1; l < 1+cnt; l++){
                    if(t_max_gcds[i][l] == pgcd){
                        flag = true;
                        break;
                    }
                }
                if(pgcd >= 3 && flag == false) {
                    t_max_gcds[i][1+cnt++] = pgcd;
                }
            }

        }
        printf("(%d) ",cnt);
        t_max_gcds[i][0] = cnt;
        for(int j = 1; j < 1 + cnt; j++){
            printf("%d, ", t_max_gcds[i][j]);
        }
        printf("\n");
    }
    decrypt_plaintexts(&number_of_max_trigrams,t_max_gcds,ciphertext,ciphertext_clean);
    dic_delete(my_dict);
    return 0;
}

