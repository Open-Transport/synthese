
/** PhysicalStop class implementation.
	@file PhysicalStop.cpp

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

#include "PhysicalStop.h"
#include "Registry.h"
#include "PTModule.h"
#include "PublicTransportStopZoneConnectionPlace.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<env::PhysicalStop>::KEY("PhysicalStop");
	}

	namespace env 
	{

		PhysicalStop::PhysicalStop(
			RegistryKeyType id
			, string name
			, const PublicTransportStopZoneConnectionPlace* place
			, double x
			, double y
		)	: Registrable(id)
			, Vertex (place, x, y)
			, _name (name),
			_operatorCode()
		{
		}



		PhysicalStop::~PhysicalStop()
		{

		}


		const std::string& 
		PhysicalStop::getName () const
		{
			return _name;
		}



		void 
		PhysicalStop::setName (const std::string& name)
		{
			_name = name;
		}



		const std::string& PhysicalStop::getOperatorCode() const
		{
			return _operatorCode;
		}

		void PhysicalStop::setOperatorCode( const std::string& code )
		{
			_operatorCode = code;
		}


		const PublicTransportStopZoneConnectionPlace* PhysicalStop::getConnectionPlace() const
		{
			return static_cast<const PublicTransportStopZoneConnectionPlace*>(Vertex::getHub());
		}

		graph::GraphIdType PhysicalStop::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}
	}
}

