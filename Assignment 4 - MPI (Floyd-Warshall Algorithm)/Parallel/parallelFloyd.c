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
void floydWarshall(int numNodes, int **graph)
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

    // Print the shortest distances
    printf("Shortest distances between every pair of nodes:\n");
    print2dArray(dist, numNodes);

    // Free the allocated memory
    for (int i = 0; i < numNodes; i++)
    {
        free(dist[i]);
    }
    free(dist);
}

// returns a copy of the provided source 2d array
int **copy2dArray(int n, int **source)
{
    int **destination = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; ++i)
    {
        destination[i] = (int *)malloc(n * sizeof(int));
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            destination[i][j] = source[i][j];
        }
    }

    return destination;
}

// flattens a 2d array to a 1d array
int *flattenArray(int **array, int n)
{
    int *flatArray = (int *)malloc(n * n * sizeof(int));

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            flatArray[i * n + j] = array[i][j];
        }
    }

    return flatArray;
}

// prints a flat 1d array
void printFlatArray(int *flatArray, int n)
{
    for (int i = 0; i < n * n; ++i)
    {
        printf("%d ", flatArray[i]);
    }
    printf("\n");
}

// transforms a 1d array into a 2d array
int **reshapeArray(int *flatArray, int n)
{
    int **newArray = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; ++i)
    {
        newArray[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; ++j)
        {
            newArray[i][j] = flatArray[i * n + j];
        }
    }

    return newArray;
}

// checks if a number is a perfect square
int isPerfectSquare(int num)
{
    int sqrt_num = sqrt(num);
    return (sqrt_num * sqrt_num == num);
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

    if (!isPerfectSquare(size))
    {
        printf("Number of processes needs to be perfect square");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int numberOfNodes = 0;

    double start_time, end_time;

    if (stat_gen == 1)
    {
        numberOfNodes = atoi(argv[1]);
    }

    int **random2dArray = NULL;
    int *flatArray = NULL;

    // Master (Rank 0) taking No. of Nodes as input.
    // Reads the Adjacency Matrix of the Graph from "input.txt".
    // 2D Array (random2dArray) is Converted to 1D Array (flatArray).
    if (rank == 0)
    {

        if (stat_gen == 0)
        {

            printf("Enter number of nodes:\n");
            scanf("%d", &numberOfNodes);
        }

        if (numberOfNodes % ((int)sqrt(size)) != 0)
        {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        random2dArray = read2dArrayFromFile("input.txt", numberOfNodes);
        // int** copiedMatrix = copy2dArray(numberOfNodes,random2dArray);
        // floydWarshall(numberOfNodes,copiedMatrix);
        flatArray = flattenArray(random2dArray, numberOfNodes);
    }

    if (rank == 0 && stat_gen == 1)
    {
        start_time = MPI_Wtime();
    }

    // The Master Broadcasts the No. of Nodes to All The Others
    MPI_Bcast(&numberOfNodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // 1. Data Buffer ("&numberOfNodes"): Address of the data to be broadcast. This can be a single variable or an array.
    // 2. Count ("1") : Number of elements in the data buffer to be broadcast
    // 3. Datatype ("MPI_INT"): MPI datatype of the elements in the data buffer.
    // 4. Root Process Rank ("0"): Rank of the process that is broadcasting the data. This process will send its data to all other processes.
    // 5. Communicator ("MPI_COMM_WORLD"): Communicator over which the broadcast operation is performed.
    //                                     Typically, MPI_COMM_WORLD is used to include all processes in the MPI job.

    // Other Processes are Allocating Memory as the size of No. of Nodes
    if (rank != 0)
    {

        flatArray = (int *)malloc(numberOfNodes * numberOfNodes * sizeof(int));
    }

    // Master Broadcasts the flatArray(1D) (Matrix) to All the others
    MPI_Bcast(flatArray, numberOfNodes * numberOfNodes, MPI_INT, 0, MPI_COMM_WORLD);

    // All the Other Processes are Making flatArray(1D) to 2D Array again
    if (rank != 0)
    {
        random2dArray = reshapeArray(flatArray, numberOfNodes);
    }

    // Creating The Processor Grid with dimensions as a square while using the given number of processes (size).
    int dims[2] = {sqrt(size), sqrt(size)};
    MPI_Dims_create(size, 2, dims);
    // 1. Size ("size"): Total number of processes for which dimensions need to be determined.
    // 2. Number of Dimensions ("2"): Number of dimensions for the Cartesian grid to be created. Typically, this is set to 2 for a 2D grid.
    // 3. Dimensions Array ("dims"):  An integer array used to store the dimensions of the Cartesian grid.
    //                                The function will populate this array with suitable dimensions based on the total number of processes
    //                                and the specified number of dimensions.

    int periods[2] = {0, 0}; // Both dimensions are set to 0, indicating non-periodic boundaries in both dimensions.

    // Creating A New Communicator of Cartesian grid topology.
    // where processes are arranged in a logical grid according to the specified dimensions and periodicity.
    MPI_Comm comm_cart;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_cart);
    // 1. Communicator ("MPI_COMM_WORLD"): The communicator over which the Cartesian topology is created. Typically, MPI_COMM_WORLD is used to include all processes in the MPI job.
    // 2. Number of Dimensions ("2"): The number of dimensions in the Cartesian grid. In this case, it's set to 2 for a 2D grid.
    // 3. Dimensions Array ("dims"): An integer array containing the dimensions of the Cartesian grid. These dimensions are determined by the MPI_Dims_create function.
    // 4. Periods Array ("periods"): An integer array indicating whether the grid is periodic in each dimension. A value of 1 indicates periodicity, while 0 indicates non-periodicity. This array should have the same length as the number of dimensions.
    // 5. Reorder ("0"): A flag indicating whether to reorder the processes in the new communicator. If set to 0, the processes will retain their original ranks. If set to 1, MPI may reorder the processes for better performance.
    // 6. New Communicator ("&comm_cart"): The new communicator representing the Cartesian topology. This communicator will be used for Cartesian communication operations.

    // Getting the Co-Ordinate of The Processes in the Cartesian Grid
    int coords[2];
    MPI_Cart_coords(comm_cart, rank, 2, coords);
    // 1. Communicator ("comm_cart"): The Cartesian communicator representing the grid topology.
    // 2. Process Rank ("rank"): The rank of the process in the Cartesian communicator comm_cart.
    // 3. Number of Dimensions ("2"): The maximum number of dimensions of the Cartesian grid. This should match the number of dimensions used when creating the Cartesian communicator.
    // 4. Coordinates Array ("coords"): An integer array to store the coordinates of the process in the Cartesian grid. This array will be populated by the function with the coordinates corresponding to the given process rank.

    // Creating the Row Communicator
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, coords[0], rank, &row_comm);
    // 1. Original Communicator ("MPI_COMM_WORLD"): The original communicator from which the new communicator will be derived. In this case, it's typically the global communicator including all MPI processes.
    // 2. Color ("coords[0]"): An integer value representing the color used for grouping processes. In this context, it's often derived from the process's row coordinate in the Cartesian grid.
    // 3. Key ("rank"): An integer value used to order processes within each new communicator. It's typically set to the rank of the process within the original communicator.
    // 4. New Communicator ("&row_comm"): The new communicator resulting from the split operation. Processes with the same color are grouped together in this communicator, forming a subset of processes from the original communicator.

    // Getting the rank of the current process within the newly created row communicator
    int row_rank;
    MPI_Comm_rank(row_comm, &row_rank);
    // 1. Communicator: The communicator in which the rank of the calling process will be determined. In this case, it's typically a communicator representing a subset of processes from the original communicator, grouped based on a certain criterion (e.g., row-wise grouping in a Cartesian grid).
    // 2. Rank Pointer: A pointer to an integer variable where the rank of the calling process within the specified communicator will be stored. After calling this function, the variable pointed to by &row_rank will contain the rank of the calling process within the communicator row_comm.

    // Similarly Creating the Column Communicator & Getting the rank of the current process within the newly created Column communicator
    MPI_Comm col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, coords[1] + dims[0], rank, &col_comm);
    int col_rank;
    MPI_Comm_rank(col_comm, &col_rank);

    // Calculating the range of indices for the rows and columns that each process is responsible for processing.
    // These ensure that each process handles a distinct portion of the overall matrix,
    int rowIndexStart = coords[0] * (numberOfNodes / ((int)sqrt(size)));
    int rowIndexEnd = rowIndexStart + (numberOfNodes / ((int)sqrt(size))) - 1;

    int colIndexStart = coords[1] * (numberOfNodes / ((int)sqrt(size)));
    int colIndexEnd = colIndexStart + (numberOfNodes / ((int)sqrt(size))) - 1;

    /*
      Process 0         Process 1
    +---------+       +---------+
    |         |       |         |
    |    A    |       |    B    |
    |         |       |         |
    +---------+       +---------+

    Process 2         Process 3
    +---------+       +---------+
    |         |       |         |
    |    C    |       |    D    |
    |         |       |         |
    +---------+       +---------+

    1. Row Broadcast:
    Process 0 broadcasts row A to Process 2.
    Process 1 broadcasts row B to Process 3.
    Process 2 receives row A from Process 0.
    Process 3 receives row B from Process 1.

    2. Column Broadcast:
    Process 0 broadcasts column A to Process 1.
    Process 2 broadcasts column C to Process 3.
    Process 1 receives column A from Process 0.
    Process 3 receives column C from Process 2.

    3. Update Matrix:
    Each process computes the shortest paths for the elements within its block (A, B, C, or D) using the Floyd-Warshall algorithm.

    4. Barrier:
    All processes wait at a barrier until all processes have completed their computations.
    */
    for (int k = 0; k < numberOfNodes; k++)
    {

        int row[(numberOfNodes) / ((int)sqrt(size))];
        int row_root_coords[2];
        row_root_coords[0] = k / (numberOfNodes / ((int)sqrt(size)));
        row_root_coords[1] = coords[1];

        if (k >= rowIndexStart && k <= rowIndexEnd)
        {

            int index = 0;
            for (int i = colIndexStart; i <= colIndexEnd; i++)
            {

                row[index] = random2dArray[k][i];
                index++;
            }
            // send to all processes in the same column
        }

        MPI_Bcast(row, colIndexEnd - colIndexStart + 1, MPI_INT, row_root_coords[0], col_comm);

        int col[(numberOfNodes) / ((int)sqrt(size))];
        int col_root_coords[2];
        col_root_coords[0] = coords[0];
        col_root_coords[1] = k / (numberOfNodes / ((int)sqrt(size)));

        if (k >= colIndexStart && k <= colIndexEnd)
        {

            int index = 0;
            for (int i = rowIndexStart; i <= rowIndexEnd; i++)
            {

                col[index] = random2dArray[i][k];
                index++;
            }
            // send to all processes in the same row
        }

        MPI_Bcast(col, rowIndexEnd - rowIndexStart + 1, MPI_INT, col_root_coords[1], row_comm);

        // Slaves Storing the Received Values/Indexes (as Rows)
        if (k < rowIndexStart || k > rowIndexEnd)
        {

            int index = 0;
            for (int i = colIndexStart; i <= colIndexEnd; i++)
            {

                random2dArray[k][i] = row[index];
                index++;
            }
        }

        // Slaves Storing the Received Values/Indexes (as Cols)
        if (k < colIndexStart || k > colIndexEnd)
        {

            int index = 0;
            for (int i = rowIndexStart; i <= rowIndexEnd; i++)
            {

                random2dArray[i][k] = col[index];
                index++;
            }
        }

        // Running The Floyd-Warshal
        for (int i = rowIndexStart; i <= rowIndexEnd; i++)
        {

            for (int j = colIndexStart; j <= colIndexEnd; j++)
            {

                if (random2dArray[i][k] != INT_MAX && random2dArray[k][j] != INT_MAX && random2dArray[i][k] + random2dArray[k][j] < random2dArray[i][j])
                {
                    random2dArray[i][j] = random2dArray[i][k] + random2dArray[k][j];
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Each Process is Sending Back the Final Matrix of Their Portion as A 1D flatArray
    if (rank != 0)
    {

        int *flatArray = flattenArray(random2dArray, numberOfNodes);
        MPI_Send(flatArray, numberOfNodes * numberOfNodes, MPI_INT, 0, 555, MPI_COMM_WORLD);
    }
    // Main Process is receiving from All others and Making Again the 2D Array from the 1D flatArrays.
    else
    {

        for (int i = 1; i < size; i++)
        {
            int receivedArray[numberOfNodes * numberOfNodes];
            MPI_Recv(receivedArray, numberOfNodes * numberOfNodes, MPI_INT, i, 555, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int **reshapedArray = reshapeArray(receivedArray, numberOfNodes);
            int from_coords[2];
            MPI_Cart_coords(comm_cart, i, 2, from_coords);

            int curRowIndexStart = from_coords[0] * (numberOfNodes / ((int)sqrt(size)));
            int curRowIndexEnd = curRowIndexStart + (numberOfNodes / ((int)sqrt(size))) - 1;

            int curColIndexStart = from_coords[1] * (numberOfNodes / ((int)sqrt(size)));
            int curColIndexEnd = curColIndexStart + (numberOfNodes / ((int)sqrt(size))) - 1;

            for (int m = curRowIndexStart; m <= curRowIndexEnd; m++)
            {

                for (int n = curColIndexStart; n <= curColIndexEnd; n++)
                {

                    random2dArray[m][n] = reshapedArray[m][n];
                }
            }
        }

        if (rank == 0 && stat_gen == 1)
        {
            end_time = MPI_Wtime();
        }
        write2dArrayToFile(random2dArray, numberOfNodes);
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0 && stat_gen == 1)
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

    MPI_Comm_free(&col_comm);
    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&comm_cart);
    MPI_Finalize();

    return 0;
}