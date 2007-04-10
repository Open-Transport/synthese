
/** BroadcastPoint class implementation.
	@file BroadcastPoint.cpp

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

#include <sstream>

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"

#include "34_departures_table/BroadcastPoint.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace env;

	namespace departurestable
	{
		BroadcastPoint::BroadcastPoint()
			: Registrable<uid, BroadcastPoint>()
		{

		}

		void BroadcastPoint::setName( const std::string& name )
		{
			_name = name;
		}

		void BroadcastPoint::setConnectionPlace(shared_ptr<const ConnectionPlace> place )
		{
			_place = place;
		}

		void BroadcastPoint::setPhysicalStop(shared_ptr<const PhysicalStop> physicalStop )
		{
			_physicalStop = physicalStop;
		}

		const std::string BroadcastPoint::getName() const
		{
			stringstream s;
			if (_physicalStop.get() && !_physicalStop->getName().empty())
				s << _physicalStop->getName();
			else if (!_name.empty())
				s << _name;
			else
				s << "(unnamed)";
			return s.str();
		}

		shared_ptr<const ConnectionPlace> BroadcastPoint::getConnectionPlace() const
		{
			return _place;
		}

		shared_ptr<const PhysicalStop> BroadcastPoint::getPhysicalStop() const
		{
			return _physicalStop;
		}

		std::string BroadcastPoint::getFullName() const
		{
			if (!_place.get())
				return "(non localisé)";
			
			stringstream s;
			s << _place->getFullName() << "/" << getName();

			return s.str(); 
		}
	}
}
