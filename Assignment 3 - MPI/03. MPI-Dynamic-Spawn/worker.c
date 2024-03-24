#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define DARTS 50000 // Number of Darts Thrown at The Dartboard
#define ROUNDS 100  // Number of iterations

// Function declarations
void initializeRandom(unsigned seed);
double calculatePi(int darts);

int main(int argc, char *argv[])
{
    int taskId, numTasks;
    int size;
    MPI_Comm parent;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks); // Get the total number of tasks
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);   // Get the task's ID

    MPI_Comm_get_parent(&parent);
    if (parent == MPI_COMM_NULL)
        perror("No parent!");

    MPI_Comm_remote_size(parent, &size);
    if (size != 1)
        perror("Something's wrong with the parent");

    initializeRandom(taskId); // Initialize random seed based on the task's ID

    double localPi = 0;

    for (int round = 0; round < ROUNDS; round++)
    {
        // Calculate the local pi estimate for each round
        localPi = calculatePi(DARTS);

        // Send the local pi estimate to the master task for each round
        MPI_Send(&localPi, 1, MPI_DOUBLE, 0, 0, parent);
    }

    MPI_Finalize();
    return 0;
}

// Function to initialize random number generator
void initializeRandom(unsigned seed)
{
    // unsigned seed = (unsigned)time(NULL); // Use current time as the seed
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
