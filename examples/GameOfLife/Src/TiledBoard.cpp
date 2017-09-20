#include "TiledBoard.h"
#include "sse.h"
#include <assert.h>

// define this if you want to get the gauss-seidel variant of the algo
//#define GAUSS_SEIDEL

TiledBoard::TiledBoard( int x_size, int y_size, int tsx, int tsy, const char * logfilename )
: Board( x_size, y_size ),
  m_tiles(),
  m_numTilesX( ( x_size + tsx - 1 ) / tsx ),
  m_numTilesY( ( y_size + tsy - 1 ) / tsy ),
  m_tileSizeX( tsx ),
  m_tileSizeY( tsy )
#ifdef DO_LOG
  , m_logger( logfilename )
#endif
{
    m_tiles.reserve( m_numTilesX * m_numTilesY );
    bool lastYFull  = ((y_size % tsy) == 0);
    bool lastXFull  = ((x_size % tsx) == 0);
    int lastFilledY = (lastYFull) ? m_numTilesY : m_numTilesY-1;
    int lastFilledX = (lastXFull) ? m_numTilesX : m_numTilesX-1;

    int xrem = x_size % m_tileSizeX;
    for( int j = 0; j < lastFilledY; ++j ) {
        for( int i = 0; i < lastFilledX; ++i ){
            m_tiles.push_back( new Tile( m_tileSizeX, m_tileSizeY ) );
        }
        if( xrem > 0 ){
            m_tiles.push_back( new Tile( xrem, m_tileSizeY ) );   
        }
    }
    int yrem = y_size % m_tileSizeY;
    if( yrem > 0 ) {
        for( int i = 0; i < lastFilledX; ++i ) {
            m_tiles.push_back( new Tile( m_tileSizeX, yrem ) );
        }
        if( xrem > 0 ){
            m_tiles.push_back( new Tile( xrem, yrem ) );  
        }
    }
}

TiledBoard::~TiledBoard()
{
    for( TileVec::iterator i = m_tiles.begin(); i != m_tiles.end(); ++i ) {
        delete *i;
    }
}

void TiledBoard::next_gen()
{
    int n = m_tiles.size();
    // first update the border/halo information in each tile
    for( int i = 0; i < n; ++i ) {
#ifdef DO_LOG
        this->log( i, INVESTIGATE );
#endif

        int west = getWest( i );
        const uch* border_west = west >= 0 ? m_tiles[west]->getEast() : NULL;

        int ne = getNE( i );
        uch corner_ne = ne >= 0 ? m_tiles[ne]->getSW() : 0;

        int north = getNorth( i );
        const uch* border_north = north >= 0 ? m_tiles[north]->getSouth() : NULL;

        int nw = getNW( i );
        uch corner_nw = nw >= 0 ? m_tiles[nw]->getSE() : 0;

        int east = getEast( i );
        const uch* border_east = east >= 0 ? m_tiles[east]->getWest() : NULL;

        int sw = getSW( i );
        uch corner_sw = sw >= 0 ? m_tiles[sw]->getNE() : 0;

        int south = getSouth( i );
        const uch* border_south = south >= 0 ? m_tiles[south]->getNorth() : NULL;

        int se = getSE( i );
        uch corner_se = se >= 0 ? m_tiles[se]->getNW() : NULL;

        m_tiles[i]->updateBorders( border_north, border_east, border_south, border_west, corner_nw, corner_ne, corner_se, corner_sw );
#ifndef GAUSS_SEIDEL
    }
    // now do the computation
    for( int i = 0; i < n; ++i ) {
#endif
        m_tiles[i]->nextGen();
        
#ifdef DO_LOG
        this->log( i, DEAD );
#endif
    }
}

unsigned char TiledBoard::get_field( int x, int y ) const
{
    int tileX = x/m_tileSizeX;
    int tileY = y/m_tileSizeY;
    int cellX = x%m_tileSizeX;
    int cellY = y%m_tileSizeY;
    return m_tiles[tileX+tileY*m_numTilesX]->get_field( cellX, cellY );
}

void TiledBoard::set_field( int x, int y, unsigned char v )
{
    int tileX = x/m_tileSizeX;
    int tileY = y/m_tileSizeY;
    int cellX = x%m_tileSizeX;
    int cellY = y%m_tileSizeY;
    m_tiles[tileX+tileY*m_numTilesX]->set_field( cellX, cellY, v );
}

TiledBoard::Tile::Tile( int xs, int ys )
: m_currGen( NULL ),
  m_nextGen( NULL ),
  m_ebmask( (uch*)_mm_malloc(VEC_SIZE, 16) ),
  m_west( NULL ),
  m_east( NULL ),
  m_xsize( 0 ),
  m_ysize( 0 ),
  m_xsizeExt( 0 )
{
    if( xs && ys ) resize( xs, ys );
}

TiledBoard::Tile::~Tile()
{
    if( m_currGen ) _mm_free( m_currGen );
    if( m_nextGen ) _mm_free( m_nextGen );
    if( m_ebmask )  _mm_free( m_ebmask );
    delete [] m_west;
    delete [] m_east;
}

void TiledBoard::Tile::resize( int xsize, int ysize )
{
    if( xsize != m_xsize || ysize != m_ysize || m_currGen == NULL ) {
        if( m_currGen ) _mm_free( m_currGen );
        if( m_nextGen ) _mm_free( m_nextGen );
        delete [] m_west;
        delete [] m_east;

        m_xsize = xsize;
        m_ysize = ysize;
        m_xsizeExt = m_xsize + 1 + VEC_SIZE-((1+m_xsize)%VEC_SIZE);
        int sz = (m_xsizeExt)*(ysize+2);
        m_currGen = (unsigned char*)_mm_malloc(sz, 64);
        m_nextGen = (unsigned char*)_mm_malloc(sz, 64);
        memset( m_currGen, 0, sz );
        memset( m_nextGen, 0, sz );
        const int NONBLOCKED_SIZE=VEC_SIZE-(m_xsizeExt-m_xsize-1);
        for(int i=0; i<VEC_SIZE; i++) {
            m_ebmask[i]=(i<NONBLOCKED_SIZE)?ON:OFF;
        }
    }
}

//void TiledBoard::Tile::copyIn( const uch * board, int board_xsize, int board_ysize, int x, int y, int xsize, int ysize )
//{
//    resize( xsize, ysize );
//
//    for( int i = y; i < y+ysize; ++i ) {
//        assert( i < board_ysize );
//        memcpy( &m_currGen[(i-y)*m_xsizeExt], &board[i*board_xsize+x], xsize );
//    }
//}
//
//void TiledBoard::Tile::copyOut( uch * board, int board_xsize, int board_ysize, int x, int y )
//{
//    for( int i = y; i < y+m_ysize; ++i ) {
//        assert( i < board_ysize );
//        memcpy( &board[i*board_xsize+x], &m_currGen[(i-y)*m_xsizeExt], m_xsize );
//    }
//}


void TiledBoard::Tile::updateBorders( const uch * border_north, const uch * border_east, const uch * border_south, const uch * border_west,
                                      uch corner_nw, uch corner_ne, uch corner_se, uch corner_sw )
{
    // init borders
    m_currGen[0]                            = corner_sw;
    m_currGen[m_xsize+1]                    = corner_se;
    m_currGen[m_ysize*m_xsizeExt+m_xsize+1] = corner_ne;
    m_currGen[m_ysize*m_xsizeExt]           = corner_nw;
    if( border_south ) memcpy( &m_currGen[1],                    border_south, m_xsize );
    if( border_north ) memcpy( &m_currGen[m_ysize*m_xsizeExt+1], border_north, m_xsize );
    if( border_west ) for( int i = 1; i <= m_ysize; ++i ) m_currGen[i*m_xsizeExt]           = border_west[i-1];
    if( border_east ) for( int i = 1; i <= m_ysize; ++i ) m_currGen[i*m_xsizeExt+m_xsize+1] = border_east[i-1];
}

void TiledBoard::Tile::nextGen( uch * nextGen, int szx, int szy ) const
{
    assert( szx == m_xsize && szy == m_ysize );
    memset( nextGen, 0, m_xsizeExt*(m_ysize+2) );

    // ok, now let's compute
    sse_next_gen( m_currGen, nextGen, m_xsizeExt*(m_ysize+2),
                  1, m_xsize+1, m_xsizeExt, 1, m_ysize+1, 
                  m_ebmask );
}

void TiledBoard::Tile::nextGen()
{
    if( m_nextGen == NULL ) {
        int sz = (m_xsizeExt)*(m_ysize+2);
        m_nextGen = (unsigned char*)_mm_malloc(sz, 64);
        memset( m_nextGen, 0, sz );
    }
    nextGen( m_nextGen, m_xsize, m_ysize );
    buffExchange();
}

void TiledBoard::Tile::nextGen( const TiledBoard::Tile * input )
{
    if( m_nextGen == NULL ) {
        m_xsizeExt = input->m_xsizeExt;
        m_ysize =  input->m_ysize;
        m_xsize =  input->m_xsize;
        int sz = (m_xsizeExt)*(m_ysize+2);
        m_nextGen = (unsigned char*)_mm_malloc(sz, 64);
        memset( m_nextGen, 0, sz );
    }
    input->nextGen( m_nextGen, m_xsize, m_ysize );
    buffExchange();
}

void TiledBoard::Tile::buffExchange()
{
    // exchange buffers
	unsigned char *temp( m_nextGen );
	m_nextGen = m_currGen;
	m_currGen = temp;
}


#ifdef DO_LOG
void TiledBoard::log( int tile, LogState s )
{
    m_logger.log( tile / m_numTilesX, tile % m_numTilesX, s );
}
#endif
