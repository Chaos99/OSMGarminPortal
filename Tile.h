#ifndef TILE_H
#define TILE_H

#include "gdal/ogrsf_frmts.h"


class Tile : public OGREnvelope
{
    public:
        Tile(unsigned int name, double s, double w, double n, double e);
        const unsigned int getName();
    protected:
    private:
        const unsigned int name;
};

#endif // TILE_H
