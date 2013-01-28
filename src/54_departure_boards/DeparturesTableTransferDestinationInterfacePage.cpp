
/** DeparturesTableTransferDestinationInterfacePage class implementation.
	@file DeparturesTableTransferDestinationInterfacePage.cpp
	@author Hugues
	@date 2009

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

#include "DeparturesTableTransferDestinationInterfacePage.h"
#include "ServicePointer.h"
#include "StopArea.hpp"
#include "City.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "Vertex.h"
#include "RollingStock.hpp"
#include "CommercialLine.h"
#include "Service.h"

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
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, departure_boards::DeparturesTableTransferDestinationInterfacePage>::FACTORY_KEY("departures_table_transfer_destination");
	}

	namespace departure_boards
	{
		DeparturesTableTransferDestinationInterfacePage::DeparturesTableTransferDestinationInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, DeparturesTableTransferDestinationInterfacePage>()
		{
		}



		void DeparturesTableTransferDestinationInterfacePage::display(
			std::ostream& stream,
			const ServicePointer& object,
			size_t localTransferRank,
			size_t totalTransferRank,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(object.getService()->getPath()));
			const StopArea* place(dynamic_cast<const StopArea*>(object.getArrivalEdge()->getFromVertex()->getHub()));

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
			{
				stringstream s;
				s << setw(2) << setfill('0') << object.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << object.getDepartureDateTime().time_of_day().minutes();
				pv.push_back(s.str()); // 10
			}
			{
				stringstream s;
				s << setw(2) << setfill('0') << object.getArrivalDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << object.getArrivalDateTime().time_of_day().minutes();
				pv.push_back(s.str()); // 11
			}
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
