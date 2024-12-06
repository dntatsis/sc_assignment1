#include "helper.h"

extern float default_frequency[26];

// Structure to hold statistical distance and corresponding shift
struct sd
{
    float statistical_distance; // Distance metric
    int shift;                  // Shift value
} typedef sd;

// Function to swap two floats
void swap(float *a, float *b)
{
    float t = *a;
    *a = *b;
    *b = t;
}

// Function to swap two integers
void swap_indx(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

// Partitions the array for quicksort
float partition(sd arr[], int low, int high)
{
    int pivot = arr[high].statistical_distance; // Pivot value
    int i = (low - 1);                          // Smaller element index

    for (int j = low; j <= high - 1; j++)
    {
        // Check if current element is smaller than pivot
        if (arr[j].statistical_distance < pivot)
        {
            i++; // Increment smaller element index
            swap(&arr[i].statistical_distance, &arr[j].statistical_distance);
            swap_indx(&arr[i].shift, &arr[j].shift);
        }
    }
    // Place pivot in correct position
    swap(&arr[i + 1].statistical_distance, &arr[high].statistical_distance);
    swap_indx(&arr[i + 1].shift, &arr[high].shift);

    return (i + 1); // Return pivot index
}

// QuickSort implementation
void quickSort(sd arr[], int low, int high)
{
    if (low < high)
    {
        // Partition the array
        int pi = partition(arr, low, high);

        // Sort elements before and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main()
{
    char *arr;
    arr = malloc(MAXC * sizeof(char)); // Allocate memory for input text
    if (arr == NULL)
    {
        printf("Memory error\n");
        return 1;
    }

    // Provide the ciphertext here
    strcpy(arr, "Yt gj, tw sty yt gj, ymfy nx ymj vzjxynts: Bmjymjw 'ynx stgqjw ns ymj rnsi yt xzkkjw Ymj xqnslx fsi fwwtbx tk tzywfljtzx ktwyzsj, Tw yt yfpj fwrx flfnsxy f xjf tk ywtzgqjx Fsi gd tuutxnsl jsi ymjr. Yt inj - yt xqjju, St rtwj; fsi gd f xqjju yt xfd bj jsi Ymj mjfwy-fhmj fsi ymj ymtzxfsi sfyzwfq xmthpx Ymfy kqjxm nx mjnw yt: 'ynx f htsxzrrfynts Ijatzyqd yt gj bnxm'i"); // Example ciphertext

    char *arr_clean;
    arr_clean = malloc(MAXC * sizeof(char)); // Allocate memory for cleaned text
    if (arr_clean == NULL)
    {
        printf("Memory error\n");
        return 1;
    }

    // Clean the text to include only alphabetic characters
    for (int i = 0, j = 0; i < strlen(arr); i++)
    {
        if (arr[i] >= 'A' && arr[i] <= 'Z')
        {
            arr_clean[j++] = tolower(arr[i]); // Convert to lowercase
        }
        else if (arr[i] >= 'a' && arr[i] <= 'z')
        {
            arr_clean[j++] = arr[i];
        }
        else if (arr[i] == '\0')
        {
            arr_clean[j++] = arr[i]; // Add null terminator
            break;
        }
    }
    printf("Clean string is: \"%s\"\n", arr_clean);
    float cipher_frequency[26];  // Cipher text frequency
    sd statistical_distance[26]; // Array to store statistical distances

    // Initialize statistical distance and cipher frequency
    for (int i = 0; i < 26; i++)
    {
        cipher_frequency[i] = 0.0;
        statistical_distance[i].statistical_distance = 0.0;
        statistical_distance[i].shift = i;
    }

    // Count letter frequencies in the cleaned text
    for (int i = 0; i < strlen(arr_clean); i++)
    {
        cipher_frequency[arr_clean[i] - 'a'] += 1.0;
    }

    // Calculate statistical distances for each shift
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            statistical_distance[i].statistical_distance += 0.5 * fabs(default_frequency[j] - cipher_frequency[(j + i) % 26]);
        }
    }

    // Find minimum statistical distance and corresponding key
    float min = statistical_distance[0].statistical_distance;
    int key = 0;
    for (int i = 0; i < 26; i++)
    {
        cipher_frequency[i] /= (float)strlen(arr_clean) / 100; 
        if (statistical_distance[i].statistical_distance < min)
        {
            min = statistical_distance[i].statistical_distance;
        }
    }

    // Sort statistical distances to prioritize smaller distances
    quickSort(statistical_distance, 0, 25);

    int cnt = 0; // Counter for possible keys
    while (cnt < 26)
    {
        char *decrypted_plaintext = malloc(MAXC * sizeof(char)); // Allocate memory for plaintext
        key = statistical_distance[cnt].shift;                   // Get current key

        // Decrypt the text using the current key
        for (int i = 0; i < strlen(arr); i++)
        {
            if (arr[i] >= 'A' && arr[i] <= 'Z')
            {
                decrypted_plaintext[i] = toupper((tolower(arr[i]) - 'a' + 26 - key) % 26 + 'a');
            }
            else if (arr[i] >= 'a' && arr[i] <= 'z')
            {
                decrypted_plaintext[i] = (arr[i] - 'a' + 26 - key) % 26 + 'a';
            }
            else
            {
                decrypted_plaintext[i] = arr[i];
            }
        }

        // Extract words and validate using a dictionary
        char **words = extract_first_k_words(decrypted_plaintext, CHECKED_WORDS);
        printf("\nTesting Key = %d, with statistical distance %.3f. Extracted Words were: ", key, statistical_distance[cnt].statistical_distance);
        for (int i = 0; i < CHECKED_WORDS && words[i] != NULL; i++)
        {
            printf("%d: %s, ", i + 1, words[i]);
        }

        // Check validity of the decrypted text
        int valid = validate_words_with_dictionary(words, 5);
        float ratio = (float)valid / CHECKED_WORDS;

        // If valid, print plaintext and stop executing
        if (ratio >= (float)PERCENTAGE_REQ / 100)
        {
            printf("\nKey = %d is valid.\nPlain text is: %s\n", key, decrypted_plaintext);
            free(decrypted_plaintext);
            break;
        }
        else
        {
            printf("\nKey = %d produces non-words %.4f, discarded.\n", key, ratio);
            free(decrypted_plaintext);
        }
        cnt++;
    }

    // Free allocated memory
    free(arr);
    free(arr_clean);

    return 0;
}
