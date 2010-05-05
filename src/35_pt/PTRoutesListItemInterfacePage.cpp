
/** PTRoutesListItemInterfacePage class implementation.
	@file PTRoutesListItemInterfacePage.cpp
	@author Hugues Romain
	@date 2010

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

#include "PTRoutesListItemInterfacePage.hpp"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "City.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace transportwebsite;

	namespace pt
	{
		const std::string PTRoutesListItemInterfacePage::DATA_NAME("name");
		const std::string PTRoutesListItemInterfacePage::DATA_LENGTH("length");
		const std::string PTRoutesListItemInterfacePage::DATA_STOPS_NUMBER("stops_number");
		const std::string PTRoutesListItemInterfacePage::DATA_DIRECTION("direction");
		const std::string PTRoutesListItemInterfacePage::DATA_ORIGIN_CITY_NAME("origin_city_name");
		const std::string PTRoutesListItemInterfacePage::DATA_ORIGIN_STOP_NAME("origin_stop_name");
		const std::string PTRoutesListItemInterfacePage::DATA_DESTINATION_CITY_NAME("destination_city_name");
		const std::string PTRoutesListItemInterfacePage::DATA_DESTINATION_STOP_NAME("destination_stop_name");
		const std::string PTRoutesListItemInterfacePage::DATA_RANK("rank");
		const std::string PTRoutesListItemInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");

		void PTRoutesListItemInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const transportwebsite::WebPage> page,
			const server::Request& request,
			const pt::Line& object,
			std::size_t rank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey()); //0
			pm.insert(DATA_NAME, object.getName()); //1
			pm.insert(DATA_LENGTH, object.getLastEdge() ?  object.getLastEdge()->getMetricOffset() : double(0)); //2
			pm.insert(DATA_STOPS_NUMBER, object.getEdges().size()); //3
			pm.insert(DATA_DIRECTION, object.getDirection()); //4
			pm.insert(DATA_ORIGIN_CITY_NAME, dynamic_cast<const LineStop*>(object.getEdge(0)) ? dynamic_cast<const LineStop*>(object.getEdge(0))->getPhysicalStop()->getConnectionPlace()->getCity()->getName() : string()); //5
			pm.insert(DATA_ORIGIN_STOP_NAME, dynamic_cast<const LineStop*>(object.getEdge(0)) ? dynamic_cast<const LineStop*>(object.getEdge(0))->getPhysicalStop()->getConnectionPlace()->getName() : string()); //6
			pm.insert(DATA_DESTINATION_CITY_NAME, dynamic_cast<const LineStop*>(object.getLastEdge()) ? dynamic_cast<const LineStop*>(object.getLastEdge())->getPhysicalStop()->getConnectionPlace()->getCity()->getName() : string()); //7
			pm.insert(DATA_DESTINATION_STOP_NAME, dynamic_cast<const LineStop*>(object.getLastEdge()) ? dynamic_cast<const LineStop*>(object.getLastEdge())->getPhysicalStop()->getConnectionPlace()->getName() : string()); //8
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 2);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}
