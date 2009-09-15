
/** DeparturesTableTransferDestinationInterfacePage class implementation.
	@file DeparturesTableTransferDestinationInterfacePage.cpp
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

#include "DeparturesTableTransferDestinationInterfacePage.h"
#include "ServiceUse.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "Edge.h"
#include "Line.h"
#include "Vertex.h"
#include "RollingStock.h"
#include "CommercialLine.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace graph;
	using namespace env;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, departurestable::DeparturesTableTransferDestinationInterfacePage>::FACTORY_KEY("departures_table_transfer_destination");
	}

	namespace departurestable
	{
		DeparturesTableTransferDestinationInterfacePage::DeparturesTableTransferDestinationInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, DeparturesTableTransferDestinationInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void DeparturesTableTransferDestinationInterfacePage::display(
			std::ostream& stream,
			const ServiceUse& object,
			size_t localTransferRank,
			size_t totalTransferRank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			const Line* line(dynamic_cast<const Line*>(object.getEdge()->getParentPath()));
			const PublicTransportStopZoneConnectionPlace* place(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(object.getArrivalEdge()->getFromVertex()->getHub()));
		
			pv.push_back(line->getRollingStock() ? lexical_cast<string>(line->getRollingStock()->getKey()) : string()); //0
			pv.push_back(lexical_cast<string>(line->getCommercialLine()->getKey())); //1
			pv.push_back(line->getCommercialLine()->getShortName()); //2
			pv.push_back(lexical_cast<string>(line->getCommercialLine()->getStyle())); //3
			pv.push_back(lexical_cast<string>(line->getCommercialLine()->getImage())); //4
			pv.push_back(lexical_cast<string>(place->getKey())); //5
			pv.push_back(place->getCity() ? place->getCity()->getName() : string());
			pv.push_back(place->getName()); //7
			pv.push_back(place->getName26());
			pv.push_back(place->getName13()); //9
			pv.push_back(object.getDepartureDateTime().toString());
			pv.push_back(object.getArrivalDateTime().toString()); //11
			pv.push_back(lexical_cast<string>(localTransferRank)); //12
			pv.push_back(lexical_cast<string>(totalTransferRank)); //13

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
