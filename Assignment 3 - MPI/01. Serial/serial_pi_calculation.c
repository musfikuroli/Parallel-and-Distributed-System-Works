#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function declarations
void initializeRandom(unsigned seed);
double calculatePi(int darts);

// Constants
#define DARTS 50000 // Number of Darts thrown at the dartboard
#define ROUNDS 100  // Number of Iterations
#define PROCESSES 4 // Number of Parallel Processes

int main()
{
    int equiNoOfParallelProcess;

    printf("Enter The Equivalent No. of Parallel Process: ");
    scanf("%d", &equiNoOfParallelProcess);

    clock_t start = clock(); // Record the starting time

    double pi, averagePi;
    int iteration;

    printf("Starting Serial Version of pi Calculation Using Dartboard Algorithm...\n");

    initializeRandom(time(NULL)); // Initialize The Random Seed

    averagePi = 0;

    for (iteration = 0; iteration < ROUNDS * equiNoOfParallelProcess; iteration++)
    {

        // Calculate the average pi estimate for this iteration (Average PI of Current Iteration of All Workers/Processes)
        pi = calculatePi(DARTS);

        // Calculate the running average of pi (Average PI till Current Iteration)
        averagePi = ((averagePi * iteration) + pi) / (iteration + 1);

        printf("After %d Throws, Average Value of PI = %10.8f\n", (DARTS * (iteration + 1)), averagePi);
    }

    printf("\nReal value of PI: 3.1415926535897 \n");

    // Printing Execution Time
    clock_t end = clock(); // Record the ending time
    double executionTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nExecution Time: %lf seconds\n", executionTime);

    return 0;
}

// Function to initialize random number generator
void initializeRandom(unsigned seed)
{
    srandom(seed);
}

// Function to estimate pi using the dartboard method
double calculatePi(int darts)
{
    double x, y, pi, randomValue;
    int score = 0, throw;

    for (throw = 0; throw < darts; throw ++)
    {
        // Generate random (x, y) coordinates within the dartboard
        randomValue = (double)random() / RAND_MAX;
        x = (2.0 * randomValue) - 1.0;
        randomValue = (double)random() / RAND_MAX;
        y = (2.0 * randomValue) - 1.0;

        // Check if the dart landed inside the circle
        if ((x * x + y * y) <= 1.0)
            score++; // Increment the score for darts inside the circle
    }

    // Calculate pi based on the number of darts inside the circle
    pi = 4.0 * (double)score / (double)darts;

    return pi;
}
