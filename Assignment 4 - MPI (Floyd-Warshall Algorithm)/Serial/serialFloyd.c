#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

int **read2dArrayFromFile(const char *filename, int n)
{
    int **newArray = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; ++i)
    {
        newArray[i] = (int *)malloc(n * sizeof(int));
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (fscanf(file, "%d", &newArray[i][j]) != 1)
            {
                perror("Error reading from file");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);

    return newArray;
}

void write2dArrayToFile(int **arr, int n)
{

    FILE *file = fopen("output.txt", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            fprintf(file, "%d ", arr[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// prints a 2d array
void print2dArray(int **array, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}
// sequential floyd warshall
int **floydWarshall(int numNodes, int **graph)
{
    // Allocate memory for the distance matrix
    int **dist = (int **)malloc(numNodes * sizeof(int *));
    for (int i = 0; i < numNodes; i++)
    {
        dist[i] = (int *)malloc(numNodes * sizeof(int));
    }

    // Initialize the distance matrix
    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            dist[i][j] = graph[i][j];
        }
    }

    // Perform the Floyd-Warshall algorithm
    for (int k = 0; k < numNodes; k++)
    {
        for (int i = 0; i < numNodes; i++)
        {
            for (int j = 0; j < numNodes; j++)
            {
                if (dist[i][k] != -1 && dist[k][j] != -1 &&
                    (dist[i][j] == -1 || dist[i][k] + dist[k][j] < dist[i][j]))
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    return dist;

    // Print the shortest distances
    // printf("Shortest distances between every pair of nodes:\n");
    // print2dArray(dist,numNodes);

    /*
    write2dArrayToFile(dist,numNodes);

    // Free the allocated memory
    for (int i = 0; i < numNodes; i++) {
        free(dist[i]);
    }
    free(dist);
    */
}

int main(int argc, char **argv)
{

    int stat_gen = 1;
    if (argc < 2)
    {

        stat_gen = 0;
    }

    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size > 1)
    {

        printf("Built for a single process");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    double start_time, end_time;
    int numberOfNodes = 0;

    if (stat_gen == 0)
    {

        printf("Enter number of nodes:\n");
        scanf("%d", &numberOfNodes);
    }
    else
    {

        numberOfNodes = atoi(argv[1]);
    }

    int **arr = NULL;
    arr = read2dArrayFromFile("input.txt", numberOfNodes);

    if (stat_gen == 1)
    {

        start_time = MPI_Wtime();
    }

    arr = floydWarshall(numberOfNodes, arr);

    if (stat_gen == 1)
    {

        end_time = MPI_Wtime();
    }

    write2dArrayToFile(arr, numberOfNodes);

    if (stat_gen == 1)
    {

        FILE *file = fopen("stat.txt", "a");

        if (file == NULL)
        {
            perror("Error opening file");
            return 1;
        }

        fprintf(file, "%d %d %.9lf\n", numberOfNodes, size, ((end_time - start_time) * 1e9));

        fclose(file);
    }

    MPI_Finalize();
}