#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>

using namespace std;

#define N 100000000
typedef double FLOAT;

FLOAT Compute(FLOAT a, FLOAT b)
{
    FLOAT c = a * a + b * b + 2 * a * b;
    FLOAT d = a * c + b * c + 2 * a * b * c;
    return c / (d + 1.0);
}

int main(int argc, char **argv)
{
    FLOAT *A = new FLOAT[N];
    assert(A);

    FLOAT *B = new FLOAT[N];
    assert(B);

    FLOAT *C = new FLOAT[N];
    assert(C);

    // Initialization
    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = i;
        C[i] = 0;
    }

// Computation
#pragma lara marker OmpMeasure
{
    int i;

#pragma lat_omp parallel for
    for (i = 0; i < N; i++)
    {
        C[i] = Compute(A[i], B[i]);
    }

#pragma lat_omp parallel for
    for (i = 0; i < N; i++)
    {
        C[i] = Compute(A[i], B[i]);
    }
}

    // Gather result
    FLOAT sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += C[i];
    }
    cout << "sum = " << sum << endl;
}
