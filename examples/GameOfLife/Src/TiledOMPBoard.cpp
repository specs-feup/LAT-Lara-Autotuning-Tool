#include "TiledOMPBoard.h"
#include "sse.h"

TiledOMPBoard::TiledOMPBoard(int x_size, int y_size, int tsx, int tsy, const char *logfilename)
    : TiledBoard(x_size, y_size, tsx, tsy, logfilename)
{
}

void TiledOMPBoard::next_gen()
{
    int n = m_tiles.size();

// first update the border/halo information in each tile

#pragma lara marker M3 {

#pragma lat_tompb parallel for
    for (int i = 0; i < n; ++i)
    {
#ifdef DO_LOG
        this->log(i, INVESTIGATE);
#endif

        //printf("1st for-loop: num_threads = %d\n", omp_get_num_threads());

        int west = getWest(i);
        const uch *border_west = west >= 0 ? m_tiles[west]->getEast() : NULL;

        int ne = getNE(i);
        uch corner_ne = ne >= 0 ? m_tiles[ne]->getSW() : 0;

        int north = getNorth(i);
        const uch *border_north = north >= 0 ? m_tiles[north]->getSouth() : NULL;

        int nw = getNW(i);
        uch corner_nw = nw >= 0 ? m_tiles[nw]->getSE() : 0;

        int east = getEast(i);
        const uch *border_east = east >= 0 ? m_tiles[east]->getWest() : NULL;

        int sw = getSW(i);
        uch corner_sw = sw >= 0 ? m_tiles[sw]->getNE() : 0;

        int south = getSouth(i);
        const uch *border_south = south >= 0 ? m_tiles[south]->getNorth() : NULL;

        int se = getSE(i);
        uch corner_se = se >= 0 ? m_tiles[se]->getNW() : NULL;
        m_tiles[i]->updateBorders(border_north, border_east, border_south, border_west, corner_nw, corner_ne, corner_se, corner_sw);
    }
// now do the computation
#pragma lat_tompb parallel for
    for (int i = 0; i < n; ++i)
    {
        //printf("2nd for-loop: num_threads = %d\n", omp_get_num_threads());

        m_tiles[i]->nextGen();
#ifdef DO_LOG
        this->log(i, DEAD);
#endif
    }
}

