
/** ConnectionPlace class implementation.
	@file ConnectionPlace.cpp

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

#include "ConnectionPlace.h"

#include "VertexAccessMap.h"

#include "SquareDistance.h"

#include <assert.h>
#include <limits>

#include "01_util/Constants.h"

using namespace std;

namespace synthese
{
	using namespace geometry;
	using namespace util;

	namespace env
	{
		const int ConnectionPlace::FORBIDDEN_TRANSFER_DELAY(99);

		ConnectionPlace::ConnectionPlace(
			RegistryKeyType key,
			const std::string& name,
			const City* city,
			ConnectionType type
		):	Registrable(key),
			AddressablePlace(name, city)
			, _connectionType(type)
		{
		}


		ConnectionPlace::~ConnectionPlace ()
		{
		}
		 


		VertexAccess ConnectionPlace::getVertexAccess(
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			const Vertex* destination,
			const Vertex* origin
		) const	{
			VertexAccess access(accessDirection);

			if (origin != 0)
			{
				access.approachDistance = 0;
				if (accessDirection == ARRIVAL_TO_DEPARTURE)
				{
					access.approachTime = getTransferDelay (origin, destination);
				} 
				else
				{
					access.approachTime = getTransferDelay (destination, origin);
				}
			}
			else
			{
				access.approachDistance = 0;
				access.approachTime = 0;
			}

			return access;
		}



		ConnectionPlace::ConnectionType ConnectionPlace::getConnectionType() const
		{
			return _connectionType;
		}
	}
}
