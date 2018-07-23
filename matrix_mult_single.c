#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Whether or not to print matrices (1/0)
#define PRINT_OUT 1

// Function prototype
double random_number();

// Main function
int main()
{
    srand((unsigned int)time(NULL)); // Random number seeder

    // Variables
    long size=4, // Matrices sizes (size x size)
         c,
         d,
         k;
    double  **first,
            **second,
            **multiply;
    long double sum=0;

    printf("Creating two matrices %dx%d... ", size, size);

    // Create matrices in the memory

    first = (double**) malloc(size * sizeof(double*));
    multiply = (double**) malloc(size * sizeof(double*));
    second = (double**) malloc(size * sizeof(double*));

    for ( c=0; c<size; c++ )
    {
        first[c] = (double*) malloc(sizeof(double));
        multiply[c] = (double*) malloc(sizeof(double));
        second[c] = (double*) malloc(sizeof(double));
    }

    printf("Done!\n");

    // Fill out the matrices

    printf("\nFilling two matrices %dx%d with random numbers... ", size, size);

    for ( c=0; c<size; c++ )
        for ( d=0; d<size; d++ )
        {
            //printf("first[%d][%d]... ", c, d);
            first[c][d] = random_number();
            //printf("Done!\n");

            //printf("second[%d][%d]... ", c, d);
            second[c][d] = random_number();
            //printf("Done!\n");
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

    printf("\n- Product of the matrices:\n\n");

    // Perform multiplication
    for ( c=0; c<size; c++ )
    {
        for ( d=0; d<size; d++ )
        {
            sum = 0;

            printf("R[%d][%d]\t= ", c+1, d+1);
            for ( k=0; k<size; k++ )
                sum += first[c][k] * second[k][d];

            printf("%llf\n", sum);
            multiply[c][d] = sum;
        }

        // Remove from memory
        //free(&second[d]);
    }

    if ( PRINT_OUT )
    {
        for ( c=0; c<size; c++ )
        {
            for ( d=0; d<size; d++ )
                printf("%.2f\t", multiply[c][d]);

            printf("\n");
        }
    }

    printf("\nDone!\n\n[End of the program]\n");

    return 0;
}

// Function that generate and return a random number between specified range
double random_number()
{
    return ((double) rand() / (double) (RAND_MAX)) * 5.0;
}
