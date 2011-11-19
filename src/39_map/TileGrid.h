
/** TileGrid class header.
	@file TileGrid.h

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

#ifndef SYNTHESE_CARTO_TILEGRID_H
#define SYNTHESE_CARTO_TILEGRID_H

#include <vector>

namespace geos
{
	namespace geom
	{
		class Coordinate;
	}
}

namespace synthese
{


namespace map
{




/** This class is aimed at maintaining a grid
   of non painted tiles.
   The grid is generated here and then dumped in the postscript file
   for interrogation when drawing non overlapping objects.
	@ingroup m39
*/
class TileGrid
{
private:

    double _step;
    double _tileWidth;
    double _tileHeight;

    std::vector<std::vector<bool> > _tiles;

protected:

public:

    TileGrid (double width,
		double height,
		double tileWidth,
		double tileHeight);

    ~TileGrid ();

    bool isMarked (int tileX, int tileY) const;


    void markTilesForPoint (const geos::geom::Coordinate& p);

    void markTilesForLine (const geos::geom::Coordinate& from,
			   const geos::geom::Coordinate& to);

    void markTilesForRectangle (const geos::geom::Coordinate& from,
				const geos::geom::Coordinate& to,
				bool filled = true);



};

}
}

#endif

