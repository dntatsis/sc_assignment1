#include "helper.h"
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

int extract_ngraph_max(struct dictionary *dict, const int n, const char *ciphertext)
{ // Finds the most frequent ngraphs in the cleaned ciphertext, storing their maximum frequencies in the hash dictionary.
    int nmax = 1;
    for (int i = 0; i< strlen(ciphertext) - 1; i++){
        char ngraph[n+1];
        for (int j = 0; j < n; j++){
            ngraph[j] = ciphertext[i+j];
        }
        ngraph[n] = '\0';

        if(dic_add(dict,ngraph,n) == false) {
            *dict->value = 1;
        }
        else {
            *dict->value += 1;
            if (*dict->value > nmax) {
                nmax = *dict->value;
            }
        }
        if(n == 2 && ngraph[0] == ngraph[1] && ngraph[1] != ciphertext[i+2]){
            i++;
            continue;

        }
    }
    return nmax;
}

void generate_next_permutation(char *string)
{ // Generates the next lexicographical permutation of a given string.
    for (int i = strlen(string) - 1; i >= 0; i--){
        if (string[i] == 'z') {
            string[i] = 'a'; // Wrap around to 'a'
        } else {
            string[i]++; // Increment the character
            return;   // Stop once the increment is successful
        }
    }
}

void extract_ngraph_list_dist (struct dictionary *dict, const int n, const char *ciphertext, const int ngraph_max, char ** ngraph_list, int ** ngraph_dist, int*nummax, int*maxcnt){
    // Creates a list of the most frequent n-graphs and calculates the distances between their occurrences, storing them for GCD calculations.
    int counter;
    int counter2;
    counter = 0;
    char ngraph[n+1];
    char term_ngraph[n+1];
    for(int i = 0; i < n; i++){
        ngraph[i] = 'a';
        term_ngraph[i] = 'z';
    }
    ngraph[n] = '\0';
    term_ngraph[n] = '\0';
    while (strcmp(ngraph,term_ngraph) != 0) {
        if(dic_find(dict,ngraph,n) == true && ngraph_max == *dict->value) {
            strcpy(ngraph_list[(counter)++],ngraph);
        }
        if (counter == MAXIMUM_NGRAPHS)
        {
            break;
        }
        generate_next_permutation(ngraph);
    }
    counter2 = 0; // maximum occurences of one of the most common n_graphs
    for (int i = 0; i < counter; i++){
        printf("Distances between instances of %d-graph %s are: ", n, ngraph_list[i]);
        int ppos = -1;
        int cnt = 0;
        for (int j = 0; j < strlen(ciphertext) - (n-1); j++){

            char ngraph[n+1];
            for (int k = 0; k < n; k++){
                ngraph[k] = ciphertext[j+k];
            }
            ngraph[n] = '\0';
            if(strcmp(ngraph,ngraph_list[i]) == 0){
                if(ppos == -1){
                    ngraph_dist[i][cnt++] = 0;
                }
                else{
                    ngraph_dist[i][cnt++] = j - ppos;
                }
                ppos = j;
            }
        }
        for (int j = 0; j < cnt; j++)
        {
            printf("%d", ngraph_dist[i][j]);
            if (j < cnt - 1)
                printf(", "); // Add a comma if not the last element
        }

        if(cnt > counter2){
            counter2 = cnt;
        }

        printf("\n");
    }
    *nummax =  counter;
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

void calculate_key(const char *ciphertext_clean, int current_gcd, const float *default_frequency, char *key) {
    // Computes the decryption key by matching ciphertext frequency with a standard English letter frequency table through Kasiski analysis.

        // Loop through each segment of the ciphertext
        for (int j = 0; j < current_gcd; j++)
    {
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

void decrypt_plaintexts (int *number_of_max_digraphs, int **ngraph_max_gcds, const char * ciphertext_original, const char * ciphertext_clean) {

    int counter = 0;
    int * checked_gcds = malloc(100*sizeof(int));
    for(int i=0; i < *number_of_max_digraphs ; i++){
        const int number_of_gcds = ngraph_max_gcds[i][0];
        for (int l=1; l < 1 + number_of_gcds; l++){         // check that ngraph_max_gcds[i][l] hasn't been checked so far, if it has, skip.
            bool flag = false;
            const int current_gcd = ngraph_max_gcds[i][l];
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
            const int k = CHECKED_WORDS;
            char **words = extract_first_k_words(decrypted_plaintext, k);
            printf("Checking for key with length = %d. Extracted Words: ",current_gcd);
            for (int i = 0; i < k && words[i] != NULL; i++) {
                printf("%d: %s, ", i + 1, words[i]);
            }
            const int valid = validate_words_with_dictionary(words, k);

            float ratio = (float)valid / k;
            if (ratio < 0.5){
                printf(" => Discarded key \"%s\" (produced non-words)\n",key);
                continue;
            }
            printf("=> Key is \"%s\" (w/ success rate %.2f)\n\n",key,ratio);
            printf("Plaintext is %s\n\n",decrypted_plaintext);
        }

    }
    free(checked_gcds);
    printf("\n");
}

int main () {

    const char *ciphertext = "Ftkins Xpk Wvzu; llna eeryc Tdgq nyx Btsl? W ldvfomx Flvs! Wpzr tizoc, Xcs hycyiuq! Xywct mauiz Nizbv X ez meqz xf ycda; Goma Flp tdgxu-zxzqedwcv Pvmi jqazzstvf ti. Buqv ob X, pnfmvs hvgdaegl xpq affas, Qnki umrztthx bu iidxy hd hpnf qizozbs! Csg vrm aj rza ilrzi emviwdgw ehroqh wcg hxepjm Qwtoetxu kiifl; kvdj wuhpb mpfbt hyecmdq. Xysgtjbyi afeer je! avu jwd xymhtps yivaae, Qdcuhlv btc wcth, iaqsg flv ktppgo jqxpvr gteyt, Fg Yi kvtn eel etdirrn dzrygwyi, Ss iwsh alm aykkpgh phyaq, pvti-weakil arv. Rgdrn hrl Nlîjvbp eak Nikeufpiln, Rezze, rbs ppy alm axysg leeymwdw ysgt, Eel wtmme pn Bi. Qlwbdsp hwtq slezxijgan. Jvnlb! flfi hweya gzgwy hwn vvcete me hwt jvlpl. Eeexpne fhml: Temwcv lrhvl flvgt lsekw wr Ovgwpzn, oi ets nspgigo e lueusb, lmgo nwurvr eppzz, ucmozbv prq wvwexioixrt omueict, heexl eomme hd Zvv- zlvm, wkobbiepro imkv utee, jeafmeu sdaa oma rets.";

    struct dictionary *my_dict = dic_new(0);

    char* ciphertext_clean;
    ciphertext_clean = malloc((strlen(ciphertext) + 1) * sizeof(char));
    if (ciphertext_clean == NULL) {
        printf("Memory error\n");
        return 1;
    }
    clean_string(ciphertext,ciphertext_clean); //remove non letters, make everything lowercase

    printf("Clean string is: %s\n\n",ciphertext_clean);
    const int digraph_max = extract_ngraph_max(my_dict,2,ciphertext_clean);

    char **digraph_list;
    digraph_list = malloc(MAXIMUM_NGRAPHS * sizeof(char *));
    for (int i = 0; i < MAXIMUM_NGRAPHS; i++)
    {
        digraph_list[i] = malloc(3*sizeof(char));
    }

    int **digraph_dist;
    digraph_dist = malloc(MAXIMUM_NGRAPHS * sizeof(int *));
    for (int i = 0; i < MAXIMUM_NGRAPHS; i++)
    {
        digraph_dist[i] = malloc(digraph_max*sizeof(int));
    }
// array that contains distances between each instance of one of the most common digraphs

    int number_of_max_digraphs = 0;
    int maximum_occurences_of_max_digraphs = 0;

    extract_ngraph_list_dist(my_dict,2,ciphertext_clean,digraph_max,digraph_list,digraph_dist,&number_of_max_digraphs,&maximum_occurences_of_max_digraphs);

    int **d_max_gcds;
    d_max_gcds = malloc(number_of_max_digraphs*sizeof(int *));
    for (int i = 0; i < MAXIMUM_NGRAPHS; i++)
    {
        d_max_gcds[i] = malloc(maximum_occurences_of_max_digraphs*sizeof(int));
    }
    for (int i= 0; i< number_of_max_digraphs; i++){ // find all gcd of distances between each common digraph, requiring that distance >= 3
        int cnt = 0;
        printf("GCDs of distances between instances of 2-graph %s are:", digraph_list[i]);
        for (int j = 1; j < maximum_occurences_of_max_digraphs; j++)
        {
            for(int k = j + 1; k < maximum_occurences_of_max_digraphs; k++){
                int pgcd = gcd(digraph_dist[i][j],digraph_dist[i][k]);
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
        d_max_gcds[i][0] = cnt;
        printf("  Total: %d GCD(s),  Values: [", cnt);
        for (int j = 1; j < 1 + cnt; j++)
        {
            printf("%d", d_max_gcds[i][j]);
            if (j < cnt)
                printf(", ");
        }
        printf("]\n\n");
    }

    decrypt_plaintexts(&number_of_max_digraphs,d_max_gcds,ciphertext,ciphertext_clean);

    const int trigraph_max = extract_ngraph_max(my_dict,3,ciphertext_clean);
    char **trigraph_list;
    trigraph_list = malloc(MAXIMUM_NGRAPHS * sizeof(char *));
    for (int i = 0; i < MAXIMUM_NGRAPHS; i++)
    {
        trigraph_list[i] = malloc(4*sizeof(char));
    }
    int **trigraph_dist;
    trigraph_dist = malloc(MAXIMUM_NGRAPHS * sizeof(char *));
    for (int i = 0; i < MAXIMUM_NGRAPHS; i++)
    {
        trigraph_dist[i] = malloc(trigraph_max*sizeof(char));
    }

    int number_of_max_trigraphs = 0;
    int maximum_occurences_of_max_trigraphs = 0;
    extract_ngraph_list_dist(my_dict,3,ciphertext_clean,trigraph_max,trigraph_list,trigraph_dist,&number_of_max_trigraphs,&maximum_occurences_of_max_trigraphs);
    int **t_max_gcds;
    t_max_gcds = malloc(number_of_max_trigraphs *sizeof(int *));
    for(int i=0; i<number_of_max_trigraphs; i++){
        t_max_gcds[i] = malloc(maximum_occurences_of_max_trigraphs*sizeof(char));
    }
    for (int i= 0; i< number_of_max_trigraphs; i++){
        int cnt = 0;
        printf("GCDs of distances between instances of 3-graph %s are:", trigraph_list[i]);
        for (int j = 1; j < maximum_occurences_of_max_trigraphs; j++)
        {
            for(int k = j + 1; k < maximum_occurences_of_max_trigraphs; k++){
                int pgcd = gcd(trigraph_dist[i][j],trigraph_dist[i][k]);
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
        t_max_gcds[i][0] = cnt;
        printf("  Total: %d GCD(s),  Values: [", cnt);
        for (int j = 1; j < 1 + cnt; j++)
        {
            printf("%d", t_max_gcds[i][j]);
            if (j < cnt)
                printf(", ");
        }
        printf("]\n\n");
    }
    decrypt_plaintexts(&number_of_max_trigraphs,t_max_gcds,ciphertext,ciphertext_clean);
    dic_delete(my_dict);
    return 0;
}

