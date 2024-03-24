#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function declarations
void initializeRandom(unsigned seed);
double calculatePi(int darts);

// Constants
#define DARTS 50000 // Number of darts thrown at the dartboard
#define ROUNDS 100  // Number of iterations
#define MASTER 0    // ID of the master task

int main(int argc, char *argv[])
{
    double localPi, pi, averagePi, receivedPi, piSum;
    int taskId, numTasks, source, messageType, errorCode, i, iteration;
    MPI_Status status;
    clock_t start = 0, end = 0;

    MPI_Init(&argc, &argv);                   // Initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks); // Get the total number of tasks
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);   // Get the task's ID
    printf("MPI Task %d has been started...\n", taskId);

    if (taskId == MASTER)
    {
        start = clock(); // Record the starting time
    }

    initializeRandom(taskId); // Initialize random seed based on the task's ID

    averagePi = 0; // Initialize the average pi value to 0

    for (iteration = 0; iteration < ROUNDS; iteration++)
    {
        // Each task calculates its local pi estimate
        localPi = calculatePi(DARTS);

        if (taskId != MASTER)
        { // For Worker Tasks (Not The Master)
            messageType = iteration;
            // Send the local pi estimate to the master task
            errorCode = MPI_Send(&localPi, 1, MPI_DOUBLE, MASTER, messageType, MPI_COMM_WORLD);

            // MPI_Send Arguments:
            // 1. Message Data (&localPi),
            // 2. Message Size (1),
            // 3. Data Type (MPI_DOUBLE),
            // 4. Destination Rank (MASTER == 0),
            // 5. Message Tag (messageType == iteration == ROUNDS),
            // 6. Communicator (MPI_COMM_WORLD).
        }
        else
        { // For the Master Task
            messageType = iteration;
            piSum = 0;

            for (i = 1; i < numTasks; i++)
            {
                // Receive the local pi estimates from worker tasks
                errorCode = MPI_Recv(&receivedPi, 1, MPI_DOUBLE, MPI_ANY_SOURCE, messageType, MPI_COMM_WORLD, &status);

                // MPI_Recv Arguments:
                // 1. Buffer to Receive The Message (&receivedPi),
                // 2. Message Size (1),
                // 3. Data Type (MPI_DOUBLE),
                // 4. Source Rank (MPI_ANY_SOURCE),
                // 5. Message tag (messageType),
                // 6. Communicator (MPI_COMM_WORLD).
                // 7. Message Status Argument (&status)

                // Accumulate the received pi estimates
                piSum = piSum + receivedPi;
            }

            // Calculate the average pi estimate for this iteration (Average PI of Current Iteration of All Workers/Processes)
            pi = (piSum + localPi) / numTasks;

            // Calculate the running average of pi (Average PI till Current Iteration)
            averagePi = ((averagePi * iteration) + pi) / (iteration + 1);

            printf("After %d Throws, Average Value of PI = %10.8f\n", (DARTS * (iteration + 1) * numTasks), averagePi);
        }
    }

    if (taskId == MASTER)
    {
        printf("\nReal value of PI: 3.1415926535897\n");

        // Printing Execution Time
        end = clock(); // Record the ending time
        double executionTime = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\nExecution Time: %lf seconds\n", executionTime);
    }

    MPI_Finalize(); // Finalize MPI
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

        // // Generate random x and y coordinates for a dart throw
        // x = (double)rand() / RAND_MAX; // Random number between 0 and 1
        // y = (double)rand() / RAND_MAX; // Random number between 0 and 1

        // Check if the dart landed inside the circle
        if ((x * x + y * y) <= 1.0)
            score++; // Increment the score for darts inside the circle
    }

    // Calculate pi based on the number of darts inside the circle
    pi = 4.0 * (double)score / (double)darts;

    return pi;
}
