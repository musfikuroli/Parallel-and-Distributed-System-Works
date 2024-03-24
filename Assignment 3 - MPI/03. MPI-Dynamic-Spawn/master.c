#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define DARTS 50000 // Number of darts thrown at the dartboard
#define ROUNDS 100  // Number of iterations
#define MASTER 0    // ID of the master task

// Function declarations
void initializeRandom();
double calculatePi(int darts);

int main(int argc, char *argv[])
{

    double localPi, pi, averagePi, receivedPi, piSum;
    int taskId, numTasks, i, iteration;
    clock_t start = 0, end = 0;
    MPI_Status status;
    MPI_Comm workerComm;                  // Communicator for the worker processes
    char worker_program[50] = "./worker"; // Worker Executable Program Path

    MPI_Init(&argc, &argv);                   // Initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks); // Get the total number of tasks
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);   // Get the task's ID

    if (taskId == MASTER)
    {
        printf("Enter The Children/Worker Number to be Spawned: \n");

        initializeRandom(); // Initialize random seed based on the task's ID

        // Spawning Process
        int spawn_number;
        scanf("%d", &spawn_number);

        start = clock(); // Record the starting time

        MPI_Comm_spawn(worker_program, MPI_ARGV_NULL, spawn_number, MPI_INFO_NULL, 0, MPI_COMM_SELF, &workerComm, MPI_ERRCODES_IGNORE);

        // MPI_Comm_spawn() Attributes:
        // 1. Worker Program (./worker_program)
        // 2. Additional Command-line Arguments (MPI_ARGV_NULL) [No Additional Arguments]
        // 3. Number of Worker Processes to Spawn (spawn_number)
        // 4. To Pass Additional Information (MPI_INFO_NULL)
        // 5. Master/Parent Process Rank (0)
        // 6. Communicator for The Parent Process (MPI_COMM_SELF) [Parent Process Communicates with Itself]
        // 7. Inter-communicator for The Parent Process to Communicate with The Spawned Processes (&workerComm) [MPI_Comm workerComm;]
        // 8. Handling Error Codes (MPI_ERRCODES_IGNORE)
        // Reset piSum to zero at the beginning of each iteration

        // Initialize the average pi value to 0
        averagePi = 0;

        for (iteration = 0; iteration < ROUNDS; iteration++)
        {
            piSum = 0;

            // Calculate the local pi estimate
            localPi = calculatePi(DARTS);

            // printf("Master PI: %lf\n", localPi);

            // Receive the local pi estimates from worker tasks
            for (i = 0; i < spawn_number; i++)
            {
                MPI_Recv(&receivedPi, 1, MPI_DOUBLE, i, MPI_ANY_TAG, workerComm, &status);
                // Accumulate the received pi estimates
                piSum = piSum + receivedPi;
            }

            // Calculate the average pi estimate for this iteration (Average PI of Current Iteration of All Workers/Processes)
            pi = (piSum + localPi) / (spawn_number + 1);

            // Calculate the running average of pi (Average PI till Current Iteration)
            averagePi = ((averagePi * iteration) + pi) / (iteration + 1);

            printf("After %d Throws, Average Value of PI = %10.8f\n", (DARTS * (iteration + 1) * spawn_number), averagePi);
        }

        printf("\nReal value of PI: 3.1415926535897\n");

        // Printing Execution Time
        end = clock(); // Record the ending time
        double executionTime = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\nExecution Time: %lf seconds\n", executionTime);

        // Finalize the communicator for the worker processes
        MPI_Comm_free(&workerComm);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}

// Function to initialize random number generator
void initializeRandom()
{
    unsigned seed = (unsigned)time(NULL); // Use current time as the seed
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