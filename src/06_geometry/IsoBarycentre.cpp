
/** IsoBarycentre class implementation.
	@file IsoBarycentre.cpp

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

#include "IsoBarycentre.h"

namespace synthese
{
namespace geometry
{


IsoBarycentre::IsoBarycentre()
        : Point2D()
{
    clear();
}

void IsoBarycentre::clear()
{
    _points = 0;
    setXY();
}

void IsoBarycentre::add( const Point2D& point )
{
    if (point.isUnknown())
        return;

    setXY(
        (getX() * _points + point.getX()) / (_points + 1)
        , (getY() * _points + point.getY()) / (_points + 1)
    );

    ++_points;
}

void IsoBarycentre::add( const IsoBarycentre& barycentre )
{
    if (barycentre.isUnknown())
        return;

    setXY(
        (getX() * _points + barycentre.getX() * barycentre._points) / (_points + barycentre._points)
        , (getY() * _points + barycentre.getY() * barycentre._points) / (_points + barycentre._points)
    );

    _points += barycentre._points;
}
}
}
