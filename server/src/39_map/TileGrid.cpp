
/** TileGrid class implementation.
	@file TileGrid.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TileGrid.h"
#include "Geometry.h"

#include <cmath>
#include <algorithm>
#include <geos/geom/Coordinate.h>

using namespace geos::geom;

namespace synthese
{


namespace map
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
TileGrid::markTilesForPoint (const Coordinate& p)
{
    int tileX = (int) (p.x / _tileWidth);
    int tileY = (int) (p.y / _tileHeight);
    _tiles[tileX][tileY] = true;
}



void
TileGrid::markTilesForLine (const Coordinate& from,
			    const Coordinate& to)
{
    markTilesForPoint (from);
    markTilesForPoint (to);

    // Special case if from.x == to.x
    if (from.x == to.x)
    {
	Coordinate next (from);
	while (1)
	{
	    next = Coordinate(next.x, next.y + _tileHeight);
	    if (next.y > to.y) break;
	    markTilesForPoint (next);
	}
	return;
    }

    std::pair<double, double> ab =
	calculateAffineEquation (from, to);
    double a = ab.first;

    // Walk through the intersections with vertical lines grid
    int startTileX = (int) (from.x / _tileWidth + 1);
    int endTileX = (int) (to.x / _tileWidth);
    if (startTileX > endTileX)
    {
	startTileX = endTileX;
	endTileX = startTileX;
    }

    for (int i=startTileX; i<=endTileX; ++i)
    {
	Coordinate p (i*_tileWidth,
		 from.y + a * ((i*_tileWidth) - from.x));
	markTilesForPoint (p);
    }


    a = 1 / a;
    // Walk through the intersections with horizontal lines grid
    int startTileY = (int) (from.y / _tileHeight + 1);
    int endTileY = (int) (to.y / _tileHeight);
    if (startTileY > endTileY)
    {
	startTileY = endTileY;
	endTileY = startTileY;
    }

    for (int i=startTileY; i<=endTileY; ++i)
    {
	Coordinate p (from.x + a * (i*_tileHeight - from.y),
		 i*_tileHeight);
	markTilesForPoint (p);
    }
}



void
TileGrid::markTilesForRectangle (const Coordinate& from,
				 const Coordinate& to,
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
