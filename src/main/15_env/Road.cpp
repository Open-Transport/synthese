
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

#include "15_env/Road.h"
#include "15_env/Address.h"
#include "15_env/RoadChunk.h"
#include "15_env/PermanentService.h"

namespace synthese
{
	namespace env
	{

		Road::Road (
			uid id
			, std::string name
			, const City* city
			, RoadType type
		)	: synthese::util::Registrable<uid,Road> (id)
			, AddressablePlace (name, city)
			, _type (type)
		{
			// Creation of the permanent service
			addService(new PermanentService(this));
		}



		Road::~Road()
		{
		}


		bool 
		Road::hasReservationRule () const
		{
			return false;
		}



		const ReservationRule* 
		Road::getReservationRule () const
		{
			return 0;
		}



		const Axis* 
		Road::getAxis () const
		{
			return 0;
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
			for (std::vector<const Address*>::const_iterator it = getAddresses ().begin ();
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
			for (std::vector<const Address*>::const_iterator it = getAddresses ().begin ();
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



		uid
		Road::getId () const
		{
			return synthese::util::Registrable<uid,Road>::getKey();
		}

		bool Road::isPedestrianMode() const
		{
			return true;
		}



	}
}
