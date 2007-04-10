
/** HandicappedComplyer class implementation.
	@file HandicappedComplyer.cpp

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

#include "HandicappedComplyer.h"



namespace synthese
{
namespace env
{



HandicappedComplyer::HandicappedComplyer (const HandicappedComplyer* parent, 
			    const HandicappedCompliance* handicappedCompliance)
    : _parent (parent)
    , _handicappedCompliance (handicappedCompliance)
{
}


HandicappedComplyer::~HandicappedComplyer ()
{
}





const HandicappedCompliance* 
HandicappedComplyer::getHandicappedCompliance () const
{
    if (_handicappedCompliance != 0) return _handicappedCompliance;
    return (_parent != 0) ? _parent->getHandicappedCompliance () : 0;
}



void 
HandicappedComplyer::setHandicappedCompliance (const HandicappedCompliance* handicappedCompliance)
{
    _handicappedCompliance = handicappedCompliance;
}

void HandicappedComplyer::setParent(HandicappedComplyer* parent )
{
	_parent = parent;
}



}
}

