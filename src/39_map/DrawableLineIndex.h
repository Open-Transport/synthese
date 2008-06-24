
/** DrawableLineIndex class header.
	@file DrawableLineIndex.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "06_geometry/Point2D.h"

#include <map>
#include <vector>
#include <set>

namespace synthese
{

namespace map
{

class DrawableLine;

/** .
	@ingroup m59
*/
class DrawableLineIndex
{

private:

    typedef std::map<double, std::set<DrawableLine*>* > Index1D;
    typedef std::map<double, Index1D* > Index2D;
	
    mutable Index2D _index;
    double _scaleX;
    double _scaleY;
    
    mutable std::vector<geometry::Point2D> _fuzzyPoints;

protected:

public:

    DrawableLineIndex();
    ~DrawableLineIndex();
	
    const std::set<DrawableLine*>&
	find (const geometry::Point2D& point) const;
    

	void setScaleX (double scaleX);
	void setScaleY (double scaleY);

    void add (const geometry::Point2D& point, 
	      DrawableLine* line) const;

    geometry::Point2D getFuzzyPoint (const geometry::Point2D& point) const;



private:

    std::set<DrawableLine*>&
	doFind (const geometry::Point2D& point) const;


};

}
}

#endif

