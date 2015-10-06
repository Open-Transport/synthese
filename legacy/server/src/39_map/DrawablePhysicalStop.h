
/** DrawablePhysicalStop class header.
	@file DrawablePhysicalStop.h

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

#ifndef SYNTHESE_CARTO_DRAWABLEPHYSICALSTOP_H
#define SYNTHESE_CARTO_DRAWABLEPHYSICALSTOP_H

#include "Drawable.h"

#include <string>
#include <geos/geom/Coordinate.h>

namespace synthese
{

namespace pt
{
    class StopPoint;
}


namespace map
{


class Map;
class PostscriptCanvas;



/** @ingroup m39 */
class DrawablePhysicalStop
{

public:


private:

	const long long _physicalStopId;
    const std::string _name;
	geos::geom::Coordinate _point;

protected:

public:

    DrawablePhysicalStop (const synthese::pt::StopPoint* physicalStop);
    ~DrawablePhysicalStop ();

    //! @name Getters/Setters
    //@{
    int getPhysicalStopId () const;
    const std::string& getName () const;
	const geos::geom::Coordinate& getPoint () const;

    //@}

    //! @name Update methods.
    //@{
    void prepare (Map& map);
    //@}

};

}
}

#endif

