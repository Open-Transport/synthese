
/** Road class implementation.
	@file Road.cpp

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

#include "Road.h"
#include "Address.h"
#include "RoadChunk.h"
#include "PermanentService.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<env::Road>::KEY("Road");
	}

	namespace env
	{

		Road::Road (
			RegistryKeyType id
			, std::string name
			, const City* city
			, RoadType type
		)	: Registrable(id)
			, AddressablePlace (name, city)
			, _type (type)
		{
			// Creation of the permanent service
			addService(new PermanentService(id, this), false);
		}



		Road::~Road()
		{
		}

		
		
		const Road::RoadType& 
		Road::getType () const
		{
			return _type;
		}


		void 
		Road::setType (const RoadType& type)
		{
			_type = type;
		}



		const Address* 
		Road::findClosestAddressBefore (double metricOffset) const
		{
			const Address* address = 0;
			for (Addresses::const_iterator it = getAddresses ().begin ();
			 it != getAddresses ().end (); ++it)
			{
			if ( ((*it)->getMetricOffset () <= metricOffset) &&
				 ( (address == 0) || ((*it)->getMetricOffset () > address->getMetricOffset ()) ) )
			{
				address = (*it);
			}
			}
			return address;
		}


		const Address* 
		Road::findClosestAddressAfter (double metricOffset) const
		{
			const Address* address = 0;
			for (Addresses::const_iterator it = getAddresses ().begin ();
			 it != getAddresses ().end (); ++it)
			{
			if ( ((*it)->getMetricOffset () >= metricOffset) &&
				 ( (address == 0) || ((*it)->getMetricOffset () < address->getMetricOffset ()) ) )
			{
				address = (*it);
			}
			}
			return address;
		}



		bool 
		Road::isRoad () const
		{
			return true;
		}



		bool 
		Road::isLine () const
		{
			return false;
		}



		bool Road::isPedestrianMode() const
		{
			return true;
		}



		void Road::getImmediateVertices(
			VertexAccessMap& result,
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			SearchAddresses returnAddresses,
			SearchPhysicalStops returnPhysicalStops,
			const Vertex* origin /*= 0 */
		) const {
			for (Addresses::const_iterator it = _addresses.begin ();
				it != _addresses.end (); ++it
			){
				(*it)->getAddressablePlace()->getImmediateVertices(
					result, accessDirection, accessParameters
					, returnAddresses, returnPhysicalStops
					, origin
				);
			}
		}
	}
}
