#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Maximum number of threads allowed (>0)
#define MAX_THREADS 200

// Whether or not to print matrices (1/0)
#define PRINT_OUT 1

// Function prototypes
void *thread_Multiply_Matrix(void *p);
double random_number();

// Global variables
double  **first,
        **second,
        **multiply;
int     *sz,
        *threads;

int main()
{
	srand((unsigned int) time(NULL)); // Random number seeder

    // Variables
    int thread_count=1, // Number of threads
        size=2, // Matrices size (size x size)
        c,
        d;
    pthread_t* thread_handles; // Thread handles

    // Pointer to the size variable
    sz = &size;
    // Pointer to the number of threads
    threads = &thread_count;

    printf("Creating two matrices %dx%d... ", (int) size, (int) size);

    // Create matrices in the memory

    /*
    double  _first[size][size],
            _second[size][size],
            _multiply[size][size];

    // Assign the pointers
    first       = &_first;
    second      = &_second;
    multiply    = &_multiply;
    */

    first    = (double**) malloc(size * sizeof(double*));
    second   = (double**) malloc(size * sizeof(double*));
    multiply = (double**) malloc(size * sizeof(double*));

    for ( c=0; c<size; c++ )
    {
        first[c]    = (double*) malloc(sizeof(double));
        second[c]   = (double*) malloc(sizeof(double));
        multiply[c] = (double*) malloc(sizeof(double));
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

    /*
    if ( size % thread_count != 0 )
        do
            thread_count++;
        while ( thread_count < thread_limit && size % thread_count != 0 );
    */

    // Initialize thread handles
    //pthread_t pth[thread_count + 1];
    //thread_handles = &pth;
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
    /*
    // Create the threads
    for( c=0; c<thread_count; c++ )
        pthread_create(&pth[c], NULL, thread_Multiply_Matrix, (void*)c);

    // Join all threads
    for( c=0; c<size; c++ )
        pthread_join(pth[c], NULL);
    */

    // Remove thread handlers from the memory
    free(thread_handles);

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
        //free(first[c]);
        //free(second[c]);
        //free(multiply[c]);
    }

    free(first);
    free(second);
    free(multiply);

    printf("\nDone!\n\n[End of the program]\n");

	return 0;
}

void *thread_Multiply_Matrix(void *p)
{
	// Get the argument and declare variables
	int     t_number = (int) p,
            size = *sz,
            thread_count = *threads,
            i,
            j,
            k;

    //printf("Hi from thread #%d\n", t_number+1);

    // Perform the calculation
    if ( thread_count == 1 )
    {
        for ( i=0; i<size; i++ )
            for( j=0; j<size; j++ )
            {
                    //printf("R[%d][%d] = ", i+1, j+1);
                    printf("Thread #%d: R[%d][%d]\n", (int) t_number+1, i+1, j+1);
                    for( k=0; k<size; k++ )
                    {
                        //printf("+ A[%d][%d] * B[%d][%d] ", i+1, k+1, k+1, j+1);
                        multiply[i][j] += first[i][k] * second[k][j];
                    }
                    //printf("\n-----\n");
            }
    }
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

            printf("\n[!] Thread #%d was elected to calculate +%d extra line(s) (odd matrix size).\n", t_number+1, dif);
        }

        for ( i=start; i<end; i++ )
            for( j=0; j<size; j++ )
            {
                    //printf("R[%d][%d] = ", i+1, j+1);
                    //printf("Thread #%d: R[%d][%d]\n", t_number+1, i+1, j+1);
                    for( k=0; k<size; k++ )
                    {
                        //printf("+ A[%d][%d] * B[%d][%d] ", i+1, k+1, k+1, j+1);
                        multiply[i][j] += first[i][k] * second[k][j];
                    }
                    //printf("\n-----\n");
            }
    }

    // Terminate the thread
    pthread_exit(NULL);
}

// Function that generate and return a random number between specified range
double random_number()
{
    return (double) rand() / (double) RAND_MAX;
}
