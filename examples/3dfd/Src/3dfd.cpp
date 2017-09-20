/*
 *   
 *   This is an example of 3D stencil computations.  It uses the
 *   cache-oblivious algorithm in the paper "The Cache Oblivious Stencil
 *   Computations" published in the Proceedings of the 2006 International
 *   Conference on Supercomputing by M. Frigo and V. Strumpen.
 *
 *   This program was originally written by Matteo Frigo and Yuxiong He.
 *   Chi-Keung (CK) Luk added autotuning pragmas to it.
 *
 *   You need the Intel Compiler v12 or above to compile this program.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h> // Cilk requires Intel compiler
#include "util_time.h"

#ifdef PARALLEL
// Cilk constructs used in parallel version
#define MY_SPAWN cilk_spawn
#define MY_SYNC cilk_sync
#define MY_FOR cilk_for
#else
// Normal C constructs used in serial version
#define MY_SPAWN
#define MY_SYNC
#define MY_FOR         for
#endif

int num_worker_threads = 0; // number of worker threads used by Cilk

float **A; // The toggle array used
float *vsq;

#ifdef CHECK_RESULT
// array holding the reference result
float *B;
#endif

// Default sizes of the grid and number of time steps
int Nx = 100;
int Ny = 100;
int Nz = 20;
int T = 40;
int Nxy;

// ds is the slope used by the cache-oblivious stencil algorithm
const int ds = 4;
float coef[ds + 1];

static inline float &aref(int t, int x, int y, int z)
{
    return A[t & 1][Nxy * z + Nx * y + x];
}

static inline float &aref(int t, int s)
{
    return A[t & 1][s];
}

static inline float &vsqref(int x, int y, int z)
{
    return vsq[Nxy * z + Nx * y + x];
}

static inline float &vsqref(int s)
{
    return vsq[s];
}

// Cache-oblivious version
static void co_basecase(int t0, int t1,
                        int x0, int dx0, int x1, int dx1,
                        int y0, int dy0, int y1, int dy1,
                        int z0, int dz0, int z1, int dz1)
{
    int _Nx = Nx;
    int Nxy = _Nx * Ny;
    int sx2 = _Nx * 2;
    int sx3 = _Nx * 3;
    int sx4 = _Nx * 4;
    int sxy2 = Nxy * 2;
    int sxy3 = Nxy * 3;
    int sxy4 = Nxy * 4;
    float c0 = coef[0], c1 = coef[1], c2 = coef[2], c3 = coef[3], c4 = coef[4];

    for (int t = t0; t < t1; ++t)
    {
        for (int z = z0; z < z1; ++z)
        {
            for (int y = y0; y < y1; ++y)
            {
                float *A_cur = &A[t & 1][z * Nxy + y * _Nx];
                float *A_next = &A[(t + 1) & 1][z * Nxy + y * _Nx];
                float *vvv = &vsq[z * Nxy + y * _Nx];
#ifdef VECTORIZE
#pragma simd
#endif
                for (int x = x0; x < x1; ++x)
                {
                    float div = c0 * A_cur[x] + c1 * ((A_cur[x + 1] + A_cur[x - 1]) + (A_cur[x + _Nx] + A_cur[x - _Nx]) + (A_cur[x + Nxy] + A_cur[x - Nxy])) + c2 * ((A_cur[x + 2] + A_cur[x - 2]) + (A_cur[x + sx2] + A_cur[x - sx2]) + (A_cur[x + sxy2] + A_cur[x - sxy2])) + c3 * ((A_cur[x + 3] + A_cur[x - 3]) + (A_cur[x + sx3] + A_cur[x - sx3]) + (A_cur[x + sxy3] + A_cur[x - sxy3])) + c4 * ((A_cur[x + 4] + A_cur[x - 4]) + (A_cur[x + sx4] + A_cur[x - sx4]) + (A_cur[x + sxy4] + A_cur[x - sxy4]));
                    A_next[x] = 2 * A_cur[x] - A_next[x] + vvv[x] * div;
                }
            }
        }
        x0 += dx0;
        x1 += dx1;
        y0 += dy0;
        y1 += dy1;
        z0 += dz0;
        z1 += dz1;
    }
}

// Loop-based version
static void loop_basecase(int t,
                          int x0, int x1,
                          int y0, int y1,
                          int z)
{
    int _Nx = Nx;
    int Nxy = _Nx * Ny;
    int sx2 = _Nx * 2;
    int sx3 = _Nx * 3;
    int sx4 = _Nx * 4;
    int sxy2 = Nxy * 2;
    int sxy3 = Nxy * 3;
    int sxy4 = Nxy * 4;
    float c0 = coef[0], c1 = coef[1], c2 = coef[2], c3 = coef[3], c4 = coef[4];

    for (int y = y0; y < y1; ++y)
    {
        float *A_cur = &A[t & 1][z * Nxy + y * _Nx];
        float *A_next = &A[(t + 1) & 1][z * Nxy + y * _Nx];
        float *vvv = &vsq[z * Nxy + y * _Nx];

#ifdef VECTORIZE
#pragma simd
#endif
        for (int x = x0; x < x1; ++x)
        {
            float div = c0 * A_cur[x] + c1 * ((A_cur[x + 1] + A_cur[x - 1]) + (A_cur[x + _Nx] + A_cur[x - _Nx]) + (A_cur[x + Nxy] + A_cur[x - Nxy])) + c2 * ((A_cur[x + 2] + A_cur[x - 2]) + (A_cur[x + sx2] + A_cur[x - sx2]) + (A_cur[x + sxy2] + A_cur[x - sxy2])) + c3 * ((A_cur[x + 3] + A_cur[x - 3]) + (A_cur[x + sx3] + A_cur[x - sx3]) + (A_cur[x + sxy3] + A_cur[x - sxy3])) + c4 * ((A_cur[x + 4] + A_cur[x - 4]) + (A_cur[x + sx4] + A_cur[x - sx4]) + (A_cur[x + sxy4] + A_cur[x - sxy4]));
            A_next[x] = 2 * A_cur[x] - A_next[x] + vvv[x] * div;
        }
    }
}

static void loop_cilk(int t0, int t1,
                      int x0, int x1,
                      int y0, int y1,
                      int z0, int z1)
{

    for (int t = t0; t < t1; ++t)
    {
        MY_FOR(int z = z0; z < z1; ++z)
        {
            loop_basecase(t, x0, x1,
                          y0, y1, z);
        }
    }
}

// Default values of the parameters used in the cache-oblivious stencil algorithm
static const int NPIECES = 2;
static const int dt_threshold = 3;
static const int dx_threshold = 1000;
static const int dy_threshold = 3;
static const int dz_threshold = 3;


static void co_cilk(int t0, int t1,
                    int x0, int dx0, int x1, int dx1,
                    int y0, int dy0, int y1, int dy1,
                    int z0, int dz0, int z1, int dz1)
{
    int dt = t1 - t0, dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;
    int i;

    if (dx >= dx_threshold && dx >= dy && dx >= dz &&
        dt >= 1 && dx >= 2 * ds * dt * NPIECES)
    {
        int chunk = dx / NPIECES;

        for (i = 0; i < NPIECES - 1; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0 + i * chunk, ds, x0 + (i + 1) * chunk, -ds,
                             y0, dy0, y1, dy1,
                             z0, dz0, z1, dz1);
        MY_SPAWN co_cilk(t0, t1,
                         x0 + i * chunk, ds, x1, -ds,
                         y0, dy0, y1, dy1,
                         z0, dz0, z1, dz1);
        MY_SYNC;
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x0, ds,
                         y0, dy0, y1, dy1,
                         z0, dz0, z1, dz1);
        for (i = 1; i < NPIECES; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0 + i * chunk, -ds, x0 + i * chunk, ds,
                             y0, dy0, y1, dy1,
                             z0, dz0, z1, dz1);
        MY_SPAWN co_cilk(t0, t1,
                         x1, -ds, x1, dx1,
                         y0, dy0, y1, dy1,
                         z0, dz0, z1, dz1);
    }
    else if (dy >= dy_threshold && dy >= dz && dt >= 1 && dy >= 2 * ds * dt * NPIECES)
    {
        int chunk = dy / NPIECES;

        for (i = 0; i < NPIECES - 1; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0, dx0, x1, dx1,
                             y0 + i * chunk, ds, y0 + (i + 1) * chunk, -ds,
                             z0, dz0, z1, dz1);
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y0 + i * chunk, ds, y1, -ds,
                         z0, dz0, z1, dz1);
        MY_SYNC;
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y0, dy0, y0, ds,
                         z0, dz0, z1, dz1);
        for (i = 1; i < NPIECES; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0, dx0, x1, dx1,
                             y0 + i * chunk, -ds, y0 + i * chunk, ds,
                             z0, dz0, z1, dz1);
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y1, -ds, y1, dy1,
                         z0, dz0, z1, dz1);
    }
    else if (dz >= dz_threshold && dt >= 1 && dz >= 2 * ds * dt * NPIECES)
    {
        int chunk = dz / NPIECES;

        for (i = 0; i < NPIECES - 1; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0, dx0, x1, dx1,
                             y0, dy0, y1, dy1,
                             z0 + i * chunk, ds, z0 + (i + 1) * chunk, -ds);
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y0, dy0, y1, dy1,
                         z0 + i * chunk, ds, z1, -ds);
        MY_SYNC;
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y0, dy0, y1, dy1,
                         z0, dz0, z0, ds);
        for (i = 1; i < NPIECES; ++i)
            MY_SPAWN co_cilk(t0, t1,
                             x0, dx0, x1, dx1,
                             y0, dy0, y1, dy1,
                             z0 + i * chunk, -ds, z0 + i * chunk, ds);
        MY_SPAWN co_cilk(t0, t1,
                         x0, dx0, x1, dx1,
                         y0, dy0, y1, dy1,
                         z1, -ds, z1, dz1);
    }
    else if (dt > dt_threshold)
    {
        int halfdt = dt / 2;
        co_cilk(t0, t0 + halfdt,
                x0, dx0, x1, dx1,
                y0, dy0, y1, dy1,
                z0, dz0, z1, dz1);
        co_cilk(t0 + halfdt, t1,
                x0 + dx0 * halfdt, dx0, x1 + dx1 * halfdt, dx1,
                y0 + dy0 * halfdt, dy0, y1 + dy1 * halfdt, dy1,
                z0 + dz0 * halfdt, dz0, z1 + dz1 * halfdt, dz1);
    }
    else
    {
        co_basecase(t0, t1,
                    x0, dx0, x1, dx1,
                    y0, dy0, y1, dy1,
                    z0, dz0, z1, dz1);
    }
}
}

static void init_variables()
{
    Nxy = Nx * Ny;

    coef[4] = -1.0f / 560.0f;
    coef[3] = 8.0f / 315;
    coef[2] = -0.2f;
    coef[1] = 1.6f;
    coef[0] = -1435.0f / 504 * 3;

    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x)
            {
                /* set initial values */
                float r = abs((float)(x - Nx / 2 + y - Ny / 2 + z - Nz / 2) / 30);
                r = max(1 - r, 0.0f) + 1;

                aref(0, x, y, z) = 0.0f;
                aref(1, x, y, z) = 0.0f;
                vsqref(x, y, z) = 0.0f;
            }
}

void print_summary(char *header, double interval)
{
    /* print timing information */
    long total = (long)Nx * Ny * Nz;

    printf("++++++++++++++++++++ %s ++++++++++++++++++++++\n", header);
    printf("first non-zero numbers\n");
    for (int i = 0; i < total; i++)
    {
        if (A[T % 2][i] != 0)
        {
            printf("%d: %f\n", i, A[T % 2][i]);
            break;
        }
    }

    long mul = (long)(Nx - 8) * (Ny - 8) * (Nz - 8) * T;
    double perf = mul / (interval * 1e6);
    printf("time: %f\n", interval);

    //The kernel has 26 additions and 7 muliplications
    printf("Perf: %f Mcells/sec (%f M-FAdd/s, %f M-FMul/s)\n",
           perf,
           perf * 26,
           perf * 7);
    printf("Perf per worker: %f Mcells/sec (%f M-FAdd/s, %f M-FMul/s)\n\n",
           perf / num_worker_threads,
           perf * 26 / num_worker_threads,
           perf * 7 / num_worker_threads);
    //printf("count = %d\n\n", count);
}

void print_y()
{
    FILE *fout = fopen("y_points.txt", "w");
    int z = Nz / 2;
    int x = Nx / 2;
    for (int y = 0; y < Ny; y++)
    {
        fprintf(fout, "%f\n", aref(T, x, y, z));
    }
    fclose(fout);
    printf("Done writing output\n");
}

#ifdef CHECK_RESULT
void copy_A_to_B()
{
    long total = Nx * Ny * Nz;
    B = new float[total];
    for (int i = 0; i < total; i++)
    {
        B[i] = A[T & 1][i];
    }
}

void verify_A_and_B()
{
    long total = Nx * Ny * Nz;
    for (int i = 0; i < total; i++)
    {
        if (B[i] != A[T & 1][i])
        {
            printf("%.12f != %.12f at index %d\n", A[T & 1][i], B[i], i);
            return;
        }
    }
    printf("All checked out, A = B\n");
}
#endif

void Dotest()
{
    //initialization
    A = new float *[2];
    A[0] = new float[Nx * Ny * Nz];
    A[1] = new float[Nx * Ny * Nz];
    vsq = new float[Nx * Ny * Nz];
    double start, stop;

    ///////////////////////////////////////////////
    init_variables();
    start = getseconds();

#pragma lara marker timing {

#ifdef CACHE_OBLIVIOUS
    co_cilk(0, T,
            ds, 0, Nx - ds, 0,
            ds, 0, Ny - ds, 0,
            ds, 0, Nz - ds, 0);
#else
    loop_cilk(0, T,
              ds, Nx - ds,
              ds, Ny - ds,
              ds, Nz - ds);
#endif
}

stop = getseconds();
print_summary((char *)"Performance Results", stop - start);

#ifdef CHECK_RESULT
copy_A_to_B();
verify_A_and_B();
delete[] B;
#endif

delete[] A;
delete[] vsq;
}

static int GetNumCpuThreads()
{
    FILE *fp;
    char res[128];
    fp = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
    fread(res, 1, sizeof(res) - 1, fp);
    fclose(fp);
    const int numCores = (int)strtol(res, 0, 0);
    return numCores;
}

// Tune the number of workers used
#pragma isat tuning name(thd_count) scope(initCilk_begin, initCilk_end) measure(start_timing, end_timing) variable(nworkers_specified, range(1, $NUM_CPU_THREADS + 1, 1))

#pragma lara marker initCilk {

static void InitCilk()
{
    const int numThreads = GetNumCpuThreads();
    printf("Number of CPU threads available = %d\n", numThreads);

    // If CILK_NPROC environment variable is set, use its value, otherwise, use the number of CPU cores
    char *x = getenv("CILK_NPROC");
    const int nworkers_specified = x ? (int)strtol(x, 0, 0) : numThreads;

    printf("Set the number of workers used by Cilk to %d\n", nworkers_specified);
    // set the number of workers used in cilk
    char buf[100];
    sprintf(buf, "%d", nworkers_specified);
    const int status = __cilkrts_set_param((void *)"nworkers", (void *)buf);
    assert(status == 0);

    num_worker_threads = __cilkrts_get_nworkers();
    printf("Actual number of workers used by Cilk = %d\n", num_worker_threads);
    assert(nworkers_specified == num_worker_threads);
}
}

int main(int argc, char *argv[])
{
    if (argc > 3)
    {
        Nx = atoi(argv[1]);
        Ny = atoi(argv[2]);
        Nz = atoi(argv[3]);
    }

    if (argc > 4)
        T = atoi(argv[4]);

    printf("Order-%d 3D-Stencil (%d points) with space %dx%dx%d and time %d\n",
           ds, ds * 2 * 3 + 1, Nx, Ny, Nz, T);

    InitCilk();

#ifdef CACHE_OBLIVIOUS
    printf("Use cache-oblivious vesion\n");
#else
    printf("Use loop-based version\n");
#endif

#ifdef PARALLEL
    printf("Parallel execution\n");
#else
    printf("Serial execution\n");
#endif

#ifdef VECTORIZE
    printf("Vectorized\n");
#else
    printf("Not vectorized\n");
#endif

    Dotest();

    return 0;
}
