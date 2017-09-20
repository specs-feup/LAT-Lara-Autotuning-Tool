#pragma once
#ifndef _TiledOMPBOARD_H_
#define _TiledOMPBOARD_H_

#include "TiledBoard.h"
#include "omp.h"

typedef unsigned char uch;

class TiledOMPBoard : public TiledBoard
{
public:
    TiledOMPBoard( int x_size, int y_size, int tsx, int tsy, const char * logfilename = NULL );
    virtual void next_gen();
};

#endif //_TiledOMPBOARD_H_
