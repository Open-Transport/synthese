#include "TileGrid.h"

#include "Geometry.h"
#include "15_env/Point.h"

#include <cmath>


using synthese::env::Point;



namespace synthese
{
namespace carto
{


TileGrid::TileGrid (double width, 
		    double height,
		    double tileWidth,
		    double tileHeight)
    : _step (std::min (tileWidth, tileHeight) / 10)
//    : _step (std::min (tileWidth, tileHeight))
    , _tileWidth (tileWidth)
    , _tileHeight (tileHeight)
{
    int nbTilesX = (int) (width / tileWidth + 1);
    int nbTilesY = (int) (height /tileHeight + 1);

    // Initialize tiles unmarked
    for (int i=0; i<nbTilesX; ++i)
    {
	std::vector<bool> column;
	for (int j=0; j<nbTilesY; ++j)
	{
	    column.push_back (false);
	}
	_tiles.push_back (column);
    }
}



TileGrid::~TileGrid ()
{
}


	
void 
TileGrid::markTilesForPoint (const synthese::env::Point& p)
{
    int tileX = (int) (p.getX () / _tileWidth);
    int tileY = (int) (p.getY () / _tileHeight);
    _tiles[tileX][tileY] = true;
}



void 
TileGrid::markTilesForLine (const synthese::env::Point& from, 
			    const synthese::env::Point& to)
{
    markTilesForPoint (from);
    markTilesForPoint (to);

    // Special case if from.getX () == to.getX ()
    if (from.getX () == to.getX ())
    {
	Point next (from);
	while (1)
	{
	    next.setY (next.getY () + _tileHeight);
	    if (next.getY () > to.getY ()) break;
	    markTilesForPoint (next);
	}
	return;
    }

    std::pair<double, double> ab =
	calculateAffineEquation (from, to);
    double a = ab.first;

    // Walk through the intersections with vertical lines grid
    int startTileX = (int) (from.getX () / _tileWidth + 1);
    int endTileX = (int) (to.getX () / _tileWidth);
    if (startTileX > endTileX)
    {
	int tmp = startTileX;
	startTileX = endTileX;
	endTileX = startTileX;
    }

    for (int i=startTileX; i<=endTileX; ++i)
    {
	Point p (i*_tileWidth, 
		 from.getY () + a * ((i*_tileWidth) - from.getX ()));
	markTilesForPoint (p);
    }


    a = 1 / a;
    // Walk through the intersections with horizontal lines grid
    int startTileY = (int) (from.getY () / _tileHeight + 1);
    int endTileY = (int) (to.getY () / _tileHeight);
    if (startTileY > endTileY)
    {
	int tmp = startTileY;
	startTileY = endTileY;
	endTileY = startTileY;
    }

    for (int i=startTileY; i<=endTileY; ++i)
    {
	Point p (from.getX () + a * (i*_tileHeight - from.getY()), 
		 i*_tileHeight);
	markTilesForPoint (p);
    }
}



void 
TileGrid::markTilesForRectangle (const synthese::env::Point& from, 
				 const synthese::env::Point& to, 
				 bool filled)
{

}


bool 
TileGrid::isMarked (int tileX, int tileY) const
{
    return _tiles[tileX][tileY];
}
	







}
}

