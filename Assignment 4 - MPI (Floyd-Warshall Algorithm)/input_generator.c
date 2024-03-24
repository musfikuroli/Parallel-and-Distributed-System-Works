#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generateRandomWeight() {
    return rand() % 991 + 10; // Generates random weight between 10 and 1000
}

int** fillRandom2dArray(int n) {
    int** randomArray = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        randomArray[i] = (int*)malloc(n * sizeof(int));
    }

    for (int i = 0; i < n; ++i) {
        randomArray[i][i] = 0;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {

            randomArray[i][j] = generateRandomWeight();
            randomArray[j][i] = randomArray[i][j];
        }
    }


    for(int i = 0; i < 3; i++){

        FILE* file;

        if(i == 0){

            file = fopen("Serial/input.txt", "w");

        }else if(i == 1){

            file = fopen("Parallel/input.txt", "w");

        }else{

            file = fopen("Pipeline/input.txt", "w");
        }

        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                fprintf(file, "%d ", randomArray[i][j]);
            }
            fprintf(file, "\n");
        }

        fclose(file);
    }
    
    return randomArray;
}

int** readArrayFromFile(const char* filename, int n) {
    int** newArray = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        newArray[i] = (int*)malloc(n * sizeof(int));
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (fscanf(file, "%d", &newArray[i][j]) != 1) {
                perror("Error reading from file");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);

    return newArray;
}

int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Usage: %s <integer>\n", argv[0]);
        return 1;  // Return an error code
    }

    srand(time(0));

    // Convert the second command line argument to an integer
    int n = atoi(argv[1]);
    
    int** randomArray = fillRandom2dArray(n);

    // Don't forget to free the allocated memory for both arrays
    for (int i = 0; i < n; ++i) {
        free(randomArray[i]);
    }
    free(randomArray);
    
    return 0;
}
