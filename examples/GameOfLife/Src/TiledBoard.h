#pragma once
#ifndef _TiledBOARD_H_
#define _TiledBOARD_H_

#include <stddef.h>
#include <vector>
#include "Board.h"
#ifdef DO_LOG
# include "log.h"
#endif

typedef unsigned char uch;
namespace worklets { class serializer; }

class TiledBoard : public Board
{
public:
    class Tile;
    TiledBoard( int x_size, int y_size, int tsx, int tsy, const char * logfilename = NULL );
    virtual ~TiledBoard();
    virtual void next_gen();    
    virtual unsigned char get_field( int x, int y ) const;
    virtual void set_field( int x, int y, unsigned char v );
    /*virtual*/ int getWest( int tileId ) const;
    /*virtual*/ int getNorth( int tileId ) const;
    /*virtual*/ int getEast( int tileId ) const;
    /*virtual*/ int getSouth( int tileId ) const;
    /*virtual*/ int getNW( int tileId ) const;
    /*virtual*/ int getNE( int tileId ) const;
    /*virtual*/ int getSE( int tileId ) const;
    /*virtual*/ int getSW( int tileId ) const;
#ifdef DO_LOG
    void log( int tile, LogState s );
#endif

    class Tile
    {
    public:
        Tile( int xsize = 0, int ysize = 0 );
        /*virtual*/ ~Tile();
        /*virtual*/ void resize( int xsize, int ysize );
        // virtual void copyIn( const uch * board, int board_xsize, int board_ysize, int x, int y, int xsize, int ysize );
        // virtual void copyOut( uch * board, int board_xsize, int board_ysize );
        /*virtual*/ void updateBorders( const uch * border_north, const uch * border_east, const uch * border_south, const uch * border_west,
                                        uch corner_nw, uch corner_ne, uch corner_se, uch corner_sw );
        /*virtual*/ void nextGen();
        /*virtual*/ void nextGen( const Tile * input );
        /*virtual*/ uch get_field( int x, int y ) const;
        /*virtual*/ void set_field( int x, int y, uch v );
        /*virtual*/ const uch * getWest();
        /*virtual*/ const uch * getNorth() const;
        /*virtual*/ const uch * getEast();
        /*virtual*/ const uch * getSouth() const;
        /*virtual*/ uch getNW() const;
        /*virtual*/ uch getNE() const;
        /*virtual*/ uch getSE() const;
        /*virtual*/ uch getSW() const;
        friend void serialize( worklets::serializer& buf, Tile& obj );
        //        friend void TiledBoard::Tile::nextGen( const TiledBoard::Tile * input );

    protected:
        /*virtual*/ void nextGen( uch * currGen, int szx, int szy ) const;
        /*virtual*/ void buffExchange();
        uch * m_currGen; // stores current generation of cells
        uch * m_nextGen; // used in nextGen to temp store next geh
        uch * m_ebmask;  // right border mask
        uch * m_west;    // buffer that stores the western border (copy of data in currGEn)
        uch * m_east;    // buffer that stores the eastern border (copy of data in currGEn)
        int   m_xsize;   // size of tile in x-direction
        int   m_ysize;   // size of tile in y-direction
        int   m_xsizeExt;// x-size including borders
    };

protected:
    typedef std::vector< Tile * > TileVec;
    TileVec m_tiles;
    int     m_numTilesX;
    int     m_numTilesY;
    int     m_tileSizeX;
    int     m_tileSizeY;
#ifdef DO_LOG
    Logger  m_logger;
#endif
};

inline int TiledBoard::getWest( int tileId ) const
{
    return tileId % m_numTilesX ? tileId-1 : -1;
}

inline int TiledBoard::getSouth( int tileId ) const
{
    return tileId >= m_numTilesX ? tileId-m_numTilesX : -1;
}

inline int TiledBoard::getEast( int tileId ) const
{
    return tileId % m_numTilesX < m_numTilesX - 1 ? tileId+1 : -1;
}

inline int TiledBoard::getNorth( int tileId ) const
{
    return tileId < m_tiles.size() - m_numTilesX ? tileId+m_numTilesX : -1;
}

inline int TiledBoard::getSW( int tileId ) const
{
    return tileId >= m_numTilesX && tileId % m_numTilesX  ? tileId-1-m_numTilesX : -1;
}

inline int TiledBoard::getSE( int tileId ) const
{
    return tileId >= m_numTilesX && tileId % m_numTilesX < m_numTilesX - 1 ? tileId-m_numTilesX+1 : -1;
}

inline int TiledBoard::getNE( int tileId ) const
{
    return tileId < m_tiles.size() - m_numTilesX && tileId % m_numTilesX < m_numTilesX - 1 ? tileId+m_numTilesX+1 : -1;
}

inline int TiledBoard::getNW( int tileId ) const
{
    return tileId < m_tiles.size() - m_numTilesX && tileId % m_numTilesX ? tileId+m_numTilesX-1 : -1;
}

inline uch TiledBoard::Tile::get_field( int x, int y ) const
{
    return m_currGen[(1+y)*m_xsizeExt+x+1];
}

inline void TiledBoard::Tile::set_field( int x, int y, uch v )
{
    m_currGen[(1+y)*m_xsizeExt+x+1] = v;
}

inline const uch * TiledBoard::Tile::getWest()
{
    if( m_west == NULL ) {
        m_west = new uch[m_ysize];
    }
    for( int i = 1; i <= m_ysize; ++i ) {
        m_west[i-1] = m_currGen[i*m_xsizeExt];
    }
    return m_west;
}

inline const uch * TiledBoard::Tile::getSouth() const
{
    return &m_currGen[m_xsizeExt+1];
}

inline const uch * TiledBoard::Tile::getEast()
{
    if( m_east == NULL ) {
        m_east = new uch[m_ysize];
    }
    for( int i = 1; i <= m_ysize; ++i ) {
        m_east[i-1] = m_currGen[i*m_xsizeExt+m_xsize+1];
    }
    return m_east;
}

inline const uch * TiledBoard::Tile::getNorth() const
{
    return &m_currGen[m_ysize*m_xsizeExt+1];
}

inline uch TiledBoard::Tile::getSW() const
{
    return m_currGen[m_xsizeExt+1];
}

inline uch TiledBoard::Tile::getSE() const
{
    return m_currGen[m_xsizeExt+m_xsize];
}

inline uch TiledBoard::Tile::getNE() const
{
    return m_currGen[m_ysize*m_xsizeExt+m_xsize];
}

inline uch TiledBoard::Tile::getNW() const
{
    return m_currGen[m_ysize*m_xsizeExt+1];
}

#endif //_TiledBOARD_H_
