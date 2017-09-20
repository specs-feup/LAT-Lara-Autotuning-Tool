#ifndef BOARD_H_
#define BOARD_H_

// Base class for game of life study implementations
// @author: Ralf Ratering
class Board
{
public:
    Board( int x_size, int y_size );
    virtual ~Board() {}

    virtual void seed();
    virtual void next_gen() = 0;
    virtual void print() const;
    
    virtual unsigned char get_field( int x, int y ) const = 0;
    virtual void set_field( int x, int y, unsigned char v ) = 0;
    int getXSize() const { return X_SIZE; }
    int getYSize() const { return Y_SIZE; }

    virtual void debug( int count );
    virtual void run( int count );

protected:
    const int X_SIZE;
    const int Y_SIZE;
    const int XY_SIZE;
};

#endif
