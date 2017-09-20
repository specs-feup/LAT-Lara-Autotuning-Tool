// A game of life study
// @original author: Ralf Ratering, Frank Schlimbach

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Board.h"
#ifdef TILED_OMP
#include "TiledOMPBoard.h"
#endif

using namespace std;

void run_benchmark(Board *board, int count)
{
    // set seed
    board->seed();

// Decide if to print every generation or run a benchmark
#if defined(DEBUG) || defined(_DEBUG)
    board->debug(count);
#else
    board->run(count);
//    board->print();
#endif
    delete board;
}

int main(int argc, char *argv[])
{
    int xsize = 30;
    int ysize = 20;
    int count = 100;

    if (argc == 4)
    {
        xsize = atoi(argv[1]);
        ysize = atoi(argv[2]);
        count = atoi(argv[3]);
    }
    cout << "Starting game of life...\n";
    cout << "Dimensions: [" << xsize << ", " << ysize << "]\n";
    cout << "Generations: " << count << endl;

#ifdef TILED_OMP
    cout << "Using OpenMP tiled algorithm...\n";

#pragma lat marker M1 {

    const int x_blksize = 64;
    const int y_blksize = 64;
    const int num_threads = 8;

    omp_set_num_threads(num_threads);

#pragma isat marker M2 {
    run_benchmark(new TiledOMPBoard(xsize, ysize, x_blksize, y_blksize, "tiledomp.log"), count);
}
}

#endif
}
