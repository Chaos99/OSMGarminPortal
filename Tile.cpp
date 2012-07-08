#include "Tile.h"


/** @brief Tile constructor
  *
  * standard constructor, creates Tile with name and coordinates
  */
Tile::Tile(unsigned int name, double s, double w, double n, double e):
    OGREnvelope(),
    name(name)
{
    if (n > s)
    {
        MinY = s;
        MaxY = n;
    }
    else
    {
        MinY = n;
        MaxY = s;
    }

    if  ( e > w)
    {
        MinX = w;
        MaxX = e;
    }
    else
    {
        MinX = e;
        MaxX = w;
    }
}

/** @brief getName
  *
  * @todo: returns te name (=number) of the tile as given by splitter
  */
const unsigned int Tile::getName()
{
    return name;
}



