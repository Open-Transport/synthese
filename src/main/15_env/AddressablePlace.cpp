
/** AddressablePlace class implementation.
	@file AddressablePlace.cpp

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

#include "15_env/AddressablePlace.h"
#include "15_env/Address.h"

#include <assert.h>

namespace synthese
{
	namespace env
	{

		AddressablePlace::AddressablePlace (const std::string& name,
							const City* city)
			: Place (name, city)
		{
		}



		AddressablePlace::~AddressablePlace ()
		{

		}



		const Addresses& AddressablePlace::getAddresses () const
		{
			return _addresses;
		}
		    


		void 
		AddressablePlace::addAddress (const Address* address)
		{
			_addresses.push_back (address);
		}






		const AddressablePlace::ConnectionType 
		AddressablePlace::getConnectionType () const
		{
			return CONNECTION_TYPE_FORBIDDEN;
		}





		void
		AddressablePlace::getImmediateVertices (VertexAccessMap& result, 
							const AccessDirection& accessDirection,
							const AccessParameters& accessParameters,
							SearchAddresses returnAddresses,
							SearchPhysicalStops returnPhysicalStops
							, const Vertex* origin
		) const {
			if (returnAddresses == SEARCH_ADDRESSES)
			{
				for (Addresses::const_iterator it = _addresses.begin ();
					 it != _addresses.end (); ++it)
				{
					if (origin == (*it)) continue;
					result.insert ((*it), getVertexAccess (accessDirection,
									   accessParameters,
									   (*it), origin));
				}
			}
		}

	}
}
