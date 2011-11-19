
/** Place class implementation.
	@file Place.cpp

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

#include "Place.h"
#include "VertexAccessMap.h"
#include <stdarg.h>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace geography
	{
		Place::Place(
		)
		{
		}


		Place::~Place ()
		{

		}

		bool Place::includes( const Place* place ) const
		{
			return place == this;
		}



		graph::VertexAccessMap Place::getVertexAccessMap(
			const AccessParameters& accessParameters,
			GraphTypes::value_type whatToSearch,
			...
		) const {
			VertexAccessMap result;
			GraphTypes whatToSearchSet;
			GraphTypes::value_type col(whatToSearch);
			va_list marker;
			for(va_start(marker, whatToSearch); col; col = va_arg(marker, GraphTypes::value_type))
			{
				whatToSearchSet.insert(col);
			}
			va_end(marker);
			getVertexAccessMap(result, accessParameters, whatToSearchSet);
			return result;
		}
	}
}
