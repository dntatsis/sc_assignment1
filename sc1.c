#define MAXC 400

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int main () {

    char* arr;
    arr = malloc(MAXC * sizeof(char));
    if (arr == NULL) {
        printf("Memory error\n");
        return 1;
    }

    strcpy(arr,"Yt gj, tw sty yt gj, ymfy nx ymj vzjxynts: Bmjymjw 'ynx stgqjw ns ymj rnsi yt xzkkjw Ymj xqnslx fsi fwwtbx tk tzywfljtzx ktwyzsj, Tw yt yfpj fwrx flfnsxy f xjf tk ywtzgqjx Fsi gd tuutxnsl jsi ymjr. Yt inj—yt xqjju, St rtwj; fsi gd f xqjju yt xfd bj jsi Ymj mjfwy-fhmj fsi ymj ymtzxfsi sfyzwfq xmthpx Ymfy kqjxm nx mjnw yt: 'ynx f htsxzrrfynts Ijatzyqd yt gj bnxm'i.");

    char* arr_clean;
    arr_clean = malloc(MAXC * sizeof(char));
    if (arr_clean == NULL) {
        printf("Memory error\n");
        return 1;
    }
    for (int i=0, j = 0; i< strlen(arr); i++) {
        if(arr[i] >= 'A' && arr[i] <= 'Z') {
            arr_clean[j++] = tolower(arr[i]);
        }
        else if (arr[i] >= 'a' && arr[i] <= 'z') {
            arr_clean[j++] = arr[i];
        }
        else if (arr[i] == '\0') {
            arr_clean[j++] = arr[i];
            break;
        }
    }
    printf("Clean string is: %s\n",arr_clean);

    float default_frequency[26] = {8.2, 1.5, 2.8, 4.2, 12.7, 2.2, 2.0, 6.1, 7.0, 0.1, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.1, 6.0, 6.3, 9.0, 2.8, 1.0, 2.4, 0.1, 2.0, 0.1};
    float cipher_frequency[26];
    float statistical_distance[26];

    for (int i = 0; i < 26; i++){
        cipher_frequency[i] = 0.0;
        statistical_distance[i] = 0.0;
    }
    for (int i = 0; i< strlen(arr_clean); i++){
        cipher_frequency[arr_clean[i] - 'a'] += 1.0;
    }

    for (int i = 0; i < 26; i++){ // for each possible shift i
        for (int j = 0; j < 26; j++){ // for each letter j
            statistical_distance[i] += 0.5*fabs(default_frequency[j] - cipher_frequency[(j + i) % 26]);
        }
    }
     float min = statistical_distance[0];
     int key = 0;
    for (int i = 0; i < 26; i++){
        cipher_frequency[i] /= (float)strlen(arr_clean) / 100;
        printf("Frequency of %c: %.3f, ",'a' + i, cipher_frequency[i]);
        printf("statistical distance of k=%d: %.3f\n",i, statistical_distance[i]);
        if(statistical_distance[i] < min){
            min = statistical_distance[i];
            key = i;
        }
    }

    for(int i = 0; i < strlen(arr); i++){
        if(arr[i] >= 'A' && arr[i] <= 'Z') {
            arr[i] = toupper((tolower(arr[i]) - 'a' + 26 - key) % 26 + 'a');
        }
        else if (arr[i] >= 'a' && arr[i] <= 'z') {
            arr[i] = (arr[i] - 'a' + 26 - key) % 26 + 'a';
        }
    }
    printf("Key = %d\nPlain text is: %s\n",key,arr);

    return 0;
}