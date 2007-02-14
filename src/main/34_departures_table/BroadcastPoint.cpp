
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

		void BroadcastPoint::setConnectionPlace( ConnectionPlace* place )
		{
			_place = place;
		}

		void BroadcastPoint::setPhysicalStop( PhysicalStop* physicalStop )
		{
			_physicalStop = physicalStop;
		}

		const std::string BroadcastPoint::getName() const
		{
			return _name;
		}

		ConnectionPlace* BroadcastPoint::getConnectionPlace() const
		{
			return _place;
		}

		PhysicalStop* BroadcastPoint::getPhysicalStop() const
		{
			return _physicalStop;
		}

		std::string BroadcastPoint::getFullName() const
		{
			if (_place == NULL)
				return "(not localized)";
			
			stringstream s;
			s << _place->getFullName() << "/";

			if ((_physicalStop != NULL) && _physicalStop->getName().size())
				s << _physicalStop->getName();
			else if (_name.size())
				s << _name;
			else
				s << "(unnamed)";

			return s.str(); 
		}
	}
}
