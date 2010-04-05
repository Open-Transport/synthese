
/** DeparturesTableDestinationInterfacepage class implementation.
	@file DeparturesTableDestinationInterfacepage.cpp
	@author Hugues
	@date 2009

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

#include "DeparturesTableDestinationInterfacepage.h"
#include "ServiceUse.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "Edge.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, departurestable::DeparturesTableDestinationInterfacepage>::FACTORY_KEY("departures_table_destination");
	}

	namespace departurestable
	{
		DeparturesTableDestinationInterfacepage::DeparturesTableDestinationInterfacepage()
			: FactorableTemplate<interfaces::InterfacePage, DeparturesTableDestinationInterfacepage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void DeparturesTableDestinationInterfacepage::display(
			std::ostream& stream,
			const ServiceUse& object,
			bool lastDisplayedStopWasInTheSameCity,
			bool isTheEndStation,
			const std::string& transfersString,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			const PublicTransportStopZoneConnectionPlace* place(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(object.getSecondEdge()->getHub()));

			ParametersVector pv;
			pv.push_back(lexical_cast<string>(place->getKey())); //0
			pv.push_back(place->getCity()->getName()); //1
			pv.push_back(place->getName()); //2
			pv.push_back(place->getName26()); //3
			pv.push_back(place->getName13()); //4
			pv.push_back(lexical_cast<string>(lastDisplayedStopWasInTheSameCity)); //5
			pv.push_back(to_simple_string(object.getSecondActualDateTime())); //6
			pv.push_back(lexical_cast<string>(isTheEndStation)); //7
			pv.push_back(transfersString); //8

			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
		}
	}
}
