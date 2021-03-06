/* File:    matrix_mult_thread.c
 * Purpose: Multiplies two nxn matrices using t different threads.
 * Compile: gcc -g -Wall -o matrix_mult_thread matrix_mult_thread.c -lpthread
 * Run:     ./matrix_mult_thread <matrix size> <number of threads>
 * Input:   None
 * Output:  Resultant matrix and/or time spent doing calculations.
 *
 * Notes:
 *  1.  Time given in seconds.
 *  2.  Limited number of threads allowed.
 *  3.  The bigger the matrices, the bigger the space required in memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Maximum number of threads allowed (>0)
#define MAX_THREADS 200

// Whether or not to print matrices (1/0)
#define PRINT_OUT 0

// Function prototypes (see declaration for more info)
void *thread_Multiply_Matrix(void *p);
double random_number();

// Global variables
double  **first, // Pointer to the first matrix
        **second, // Pointer to the second matrix
        **multiply, // Pointer to the resultant matrix
        begin_serial, // Timestamp when serial part begins
        end_serial, // Timestamp when serial part ends
        begin_parallel, // Timestamp when parallel part begins
        end_parallel; // Timestamp when parallel part ends
int     *sz, // Pointer to the matrix size
        *n_threads; // Pointer to the number of threads

/***************************************************
Main function of the program

@param:
    (int) argc - number of words in argument
    char* argv[] - array of arguments
#return:
    (int) 0 - when successful
    (int) 1 - when no valid arguments provided
***************************************************/
int main(int argc, char* argv[])
{
    // Check the arquments
    if ( argc != 3 )
    {
        printf("\nUsage: matrix_mult_thread [n] [t]\n\tn - Matrix size (nxn)\n\tt - Number of threads\n");

        exit(1); // Force quit
    }

    // Timestamp when serial part starts
    begin_serial = (double) clock() / CLOCKS_PER_SEC;

	srand((unsigned int) time(NULL)); // Random number seeder

    // Variables
    int thread_count=1, // Number of threads
        size=1, // Matrices size (size x size)
        c,
        d;
    pthread_t* thread_handles; // Thread handles

    // Get values from call arguments
    size            = atoi(argv[1]);
	thread_count    = atoi(argv[2]);

    // Check values
    if ( size < 1 || thread_count < 1 )
    {
        printf("\n[!] Invalid arguments - neither matrix size nor number of threads can be zero!\n\n");

        exit(1);
    }

    // Pointer to the size variable
    sz = &size;
    // Pointer to the number of threads
    n_threads = &thread_count;

    printf("Creating two matrices %dx%d... ", size, size);

    // Create matrices in the memory

    first    = (double**) malloc(size * sizeof(double*));
    second   = (double**) malloc(size * sizeof(double*));
    multiply = (double**) malloc(size * sizeof(double*));

    for ( c=0; c<size; c++ )
    {
        first[c]    = (double*) malloc(size * sizeof(double));
        second[c]   = (double*) malloc(size * sizeof(double));
        multiply[c] = (double*) malloc(size * sizeof(double));
    }

    printf("Done!\n");

    // Fill out the matrices

    printf("\nFilling two matrices %dx%d with random numbers... ", size, size);

    for ( c=0; c<size; c++ )
        for ( d=0; d<size; d++ )
        {
            first[c][d]     = random_number();
            second[c][d]    = random_number();
            multiply[c][d]  = 0.0; // Put zeros in the resultant matrix
        }

    printf("Done!\n");

    // Print out the matrices

    if ( PRINT_OUT )
    {
        printf("\n- First matrix:\n\n");

        for ( c=0; c<size; c++ )
        {
            for ( d=0; d<size; d++ )
                printf ("%.2f\t", first[c][d]);

            printf("\n");
        }

        printf("\n- Second matrix:\n\n");

        for ( c=0; c<size; c++ )
        {
            for ( d=0; d<size; d++ )
                printf ("%.2f\t", second[c][d]);

            printf("\n");
        }
    }

    // Calculate the correct number of threads

    if ( thread_count > MAX_THREADS )
    {
        thread_count = MAX_THREADS;

        printf("\n[!] Number of threads lowered to %d (maximum threads limit).\n", MAX_THREADS);
    }

    if ( thread_count > size )
    {
        thread_count = size;

        printf("\n[!] Number of threads lowered to %d (matrix rows number).\n", size);
    }

    printf("\n> Using %d thread(s) to calculate...\n", thread_count);

    // Initialize thread handles vector
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));

    /************* BEGIN PARALLEL ****************/

    // Timestamp when parallel part starts
    begin_parallel = (double) clock() / CLOCKS_PER_SEC;

    // Create the threads and run them
    for( c=0; c<thread_count; c++ )
        pthread_create(&thread_handles[c], NULL, thread_Multiply_Matrix, (void*)c);

    // Join all threads (wait until all of them are finished)
    for( c=0; c<thread_count; c++ )
        pthread_join(thread_handles[c], NULL);

    // Timestamp when parallel part ends
    end_parallel = (double) clock() / CLOCKS_PER_SEC;

    /************** END PARALLEL *****************/

	if ( PRINT_OUT )
    {
        printf("\n- Product of the matrices:\n\n");

        for ( c=0; c<size; c++ )
        {
            for ( d=0; d<size; d++ )
                printf("%.2f\t", multiply[c][d]);

            printf("\n");
        }
    }

    // Clear matrices from the memory

    for ( c=0; c<size; c++ )
    {
        free(first[c]);
        free(second[c]);
        free(multiply[c]);

        first[c]    = NULL;
        second[c]   = NULL;
        multiply[c] = NULL;
    }

    free(first);
    free(second);
    free(multiply);

    first       = NULL;
    second      = NULL;
    multiply    = NULL;

    // Remove thread handlers vector from the memory
    free(thread_handles);
    thread_handles = NULL;

    // Timestamp when serial part ends
    end_serial = (double) clock() / CLOCKS_PER_SEC;

    printf("\nDone!");

    // Calculate each time spent
    double time_parallel    = end_parallel - begin_parallel;
    double time_serial      = end_serial - begin_serial;
    // Show statistics about execution time
    printf("\n____________________________________________________\n\n");
    printf("Execution time (by part):\n\n");
    printf("- Serial:\t%.3f seconds\n", (double) time_serial-time_parallel);
    printf("- Parallel:\t%.3f seconds\n", (double) time_parallel);
    printf("- Total:\t%.3f seconds\n", (double) time_serial);

    // End of the program

    printf("\n[End of the program]\n");

	return 0;
}

/***************************************************
Function that multiply lines of a first matrix with
columns of a second matrix.

@param:
    (void) *p - thread number, integer
#return:
    (void)
***************************************************/
void *thread_Multiply_Matrix(void *p)
{
	// Get the argument and declare variables
	int     t_number = (int) p, // Thread number
            size = *sz, // Size of the matrices
            thread_count = *n_threads, // Number of threads
            i,
            j,
            k;

    // Perform the calculation
    if ( thread_count == 1 )

        for ( i=0; i<size; i++ )
            for( j=0; j<size; j++ )
                for( k=0; k<size; k++ )
                    multiply[i][j] += first[i][k] * second[k][j];

    else
    {
        // Calculate the thread's boundaries
        int rows = size / thread_count;
        int start = rows * t_number;
        int end = start + rows;
        int dif = size - ( thread_count * rows );

        // If it is the last thread and the matrix size is odd
        if ( t_number == thread_count-1 && size % 2 != 0 && dif > 0 )
        {
            end += dif;

            printf("\n[!] Thread #%d was elected to calculate %d extra line(s) (odd matrix size).\n", t_number+1, dif);
        }

        for ( i=start; i<end; i++ )
            for( j=0; j<size; j++ )
                for( k=0; k<size; k++ )
                    multiply[i][j] += first[i][k] * second[k][j];
    }

    // Terminate the thread
    pthread_exit(NULL);
}

/***************************************************
Function that generate and return a random number
between fixed range (0 to 1).

@param:
    (void)
#return:
    (double) - random generated number
***************************************************/
double random_number()
{
    return (double) rand() / (double) RAND_MAX;
}
