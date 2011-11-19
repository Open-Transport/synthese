
/** DrawableLineIndex class header.
	@file DrawableLineIndex.h

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

#ifndef SYNTHESE_CARTO_DRAWABLELINEINDEX_H
#define SYNTHESE_CARTO_DRAWABLELINEINDEX_H

#include <map>
#include <vector>
#include <set>

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

class DrawableLine;

/** .
	@ingroup m39
*/
class DrawableLineIndex
{

private:

    typedef std::map<double, std::set<DrawableLine*>* > Index1D;
    typedef std::map<double, Index1D* > Index2D;

    mutable Index2D _index;
    double _scaleX;
    double _scaleY;

    mutable std::vector<geos::geom::Coordinate> _fuzzyPoints;

protected:

public:

    DrawableLineIndex();
    ~DrawableLineIndex();

    const std::set<DrawableLine*>&
	find (const geos::geom::Coordinate& point) const;


	void setScaleX (double scaleX);
	void setScaleY (double scaleY);

    void add (const geos::geom::Coordinate& point,
	      DrawableLine* line) const;

    geos::geom::Coordinate getFuzzyPoint (const geos::geom::Coordinate& point) const;



private:

    std::set<DrawableLine*>&
	doFind (const geos::geom::Coordinate& point) const;


};

}
}

#endif

