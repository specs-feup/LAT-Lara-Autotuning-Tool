#include "Board.h"
#include "Timer.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>
using namespace std;

Board::Board( int x_size, int y_size )
:
    X_SIZE( x_size ), Y_SIZE( y_size ), XY_SIZE( x_size * y_size )
{
    // initialize rnd generator with constant value
    // for reproducable results.
    srand( 1234567 );
}

void Board::seed()
{
    for ( int y = 0; y < Y_SIZE; ++y ) {
        for ( int x = 0; x < X_SIZE; ++x ) {
            set_field( x, y, rand() % 2 );
        }
    }
}

void Board::print() const
{
    for ( int y = 0; y < Y_SIZE; ++y ) {
        for ( int x = 0; x < X_SIZE; ++x ) {
            cout << ( ( get_field( x, y ) == 1 ) ? "X" : "." );
        }
        cout << endl;
    }
}

void Board::debug( int count )
{
    char key='_';
    int nr=0;
    while(key!='q' && nr<count) {
        cout << "Generation " << nr << endl;
        this->print();
        if(key!='c') {
            cin >> key;
        }
        this->next_gen();
        nr++;
    }
}

void Board::run( int count )
{
    Timer timer;
    timer.start();
    for(int i=0; i<count; i++) {
        this->next_gen();
    }
    timer.stop();
    cout << "Generations: " << count << endl;     
    cout << timer.getTimeInMS() << " ms" << endl;
}
