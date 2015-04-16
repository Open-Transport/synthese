
/** TimetableServiceRowInterfacePage class implementation.
	@file TimetableServiceRowInterfacePage.cpp
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

#include "TimetableServiceRowInterfacePage.h"
#include "TimetableColumn.h"
#include "StopPoint.hpp"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "RollingStock.hpp"
#include "TimetableWarning.h"
#include "Service.h"
#include "StopArea.hpp"
#include "StaticFunctionRequest.h"
#include "Webpage.h"
#include "City.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace timetables;
	using namespace graph;
	using namespace pt;
	using namespace cms;
	using namespace server;
	using namespace vehicle;

	namespace timetables
	{
		const string TimetableServiceRowInterfacePage::DATA_DEPARTURE_HOUR("departure_hour");
		const string TimetableServiceRowInterfacePage::DATA_DEPARTURE_MINUTES("departure_minutes");
		const string TimetableServiceRowInterfacePage::DATA_LAST_DEPARTURE_HOUR("last_departure_hour");
		const string TimetableServiceRowInterfacePage::DATA_NOTE_NUMBER("note_number");
		const string TimetableServiceRowInterfacePage::DATA_NOTE_TEXT("note_text");
		const string TimetableServiceRowInterfacePage::DATA_CELLS_CONTENT("cells_content");
		const string TimetableServiceRowInterfacePage::DATA_DEPARTURE_QUAY("departure_quay");
		const string TimetableServiceRowInterfacePage::DATA_LINE_SHORT_NAME("line_short_name");
		const string TimetableServiceRowInterfacePage::DATA_LINE_STYLE("line_style");
		const string TimetableServiceRowInterfacePage::DATA_LINE_PICTURE("line_picture");
		const string TimetableServiceRowInterfacePage::DATA_ROLLING_STOCK_ID("rolling_stock_id");
		const string TimetableServiceRowInterfacePage::DATA_RANK("rank");
		const string TimetableServiceRowInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");
		const string TimetableServiceRowInterfacePage::DATA_FOLLOWING_SERVICES_WITH_SAME_DEPARTURE_HOUR("following_services_with_same_departure_hour");
		const string TimetableServiceRowInterfacePage::DATA_LINE_DESTINATION_CITY_ID("line_destination_city_id");
		const string TimetableServiceRowInterfacePage::DATA_LINE_DESTINATION_CITY_NAME("line_destination_city_name");
		const string TimetableServiceRowInterfacePage::DATA_LINE_DESTINATION_PLACE_ID("line_destination_place_id");
		const string TimetableServiceRowInterfacePage::DATA_LINE_DESTINATION_PLACE_NAME("line_destination_place_name");
		const string TimetableServiceRowInterfacePage::DATA_TEXT_LINE_DESTINATION("text_line_destination");
		const string TimetableServiceRowInterfacePage::DATA_LINE_ID("line_id");
		const string TimetableServiceRowInterfacePage::DATA_ROUTE_ID("route_id");

		const string TimetableServiceRowInterfacePage::DATA_HOURS("hours");
		const string TimetableServiceRowInterfacePage::DATA_MINUTES("minutes");
		const string TimetableServiceRowInterfacePage::DATA_CITY_NAME("city_name");
		const string TimetableServiceRowInterfacePage::DATA_STOP_NAME("stop_name");
		const string TimetableServiceRowInterfacePage::DATA_STOP_ALIAS("stop_alias");


		void TimetableServiceRowInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			boost::shared_ptr<const Webpage> cellPage,
			const server::Request& request,
			const TimetableColumn& object,
			const time_duration& lastSchedule,
			size_t rank,
			size_t followingServicesWithSameHour
		){
			ParametersMap pm(request.getFunction()->getTemplateParameters());

			pm.insert(DATA_DEPARTURE_HOUR, Service::GetTimeOfDay(object.getContent().begin()->second).hours()); //0
			pm.insert(DATA_DEPARTURE_MINUTES, Service::GetTimeOfDay(object.getContent().begin()->second).minutes()); //1
			pm.insert(DATA_LAST_DEPARTURE_HOUR, Service::GetTimeOfDay(lastSchedule).hours()); //2
			if(object.getWarning().get())
			{
				pm.insert(DATA_NOTE_NUMBER, object.getWarning()->getNumber()); //3
				pm.insert(DATA_NOTE_TEXT, object.getWarning()->getText()); //4
			}

			// 5 : Cells content
			if(cellPage.get())
			{
				stringstream content;
				size_t rank(0);
				BOOST_FOREACH(const TimetableColumn::Content::value_type& cell, object.getContent())
				{
					DisplayCell(
						content,
						cellPage,
						request,
						cell,
						object.getLine()->getRollingStock(),
						rank++
					);
				}
				pm.insert(DATA_CELLS_CONTENT, content.str());
			}

			if(object.getContent().begin()->first)
			{
				pm.insert(DATA_DEPARTURE_QUAY, object.getContent().begin()->first->getName()); //6
			}
			pm.insert(DATA_LINE_SHORT_NAME, object.getLine()->getCommercialLine()->getShortName()); //7
			pm.insert(DATA_LINE_STYLE, object.getLine()->getCommercialLine()->getStyle()); //8
			pm.insert(DATA_LINE_PICTURE, object.getLine()->getCommercialLine()->getImage()); //9
			if(object.getLine()->getRollingStock())
			{
				pm.insert(DATA_ROLLING_STOCK_ID, object.getLine()->getRollingStock()->getKey()); //10
			}
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 2);
			pm.insert(DATA_FOLLOWING_SERVICES_WITH_SAME_DEPARTURE_HOUR, followingServicesWithSameHour); //12

			const StopArea* lastPlace(object.getLine()->getDestination()->getConnectionPlace());
			pm.insert(DATA_LINE_DESTINATION_CITY_ID, lastPlace->getCity()->getKey()); //13
			pm.insert(DATA_LINE_DESTINATION_CITY_NAME, lastPlace->getCity()->getName()); //14
			pm.insert(DATA_LINE_DESTINATION_PLACE_ID, lastPlace->getKey()); //15
			pm.insert(DATA_LINE_DESTINATION_PLACE_NAME, lastPlace->getName()); //16

			pm.insert(DATA_TEXT_LINE_DESTINATION, object.getLine()->getDirection()); //17

			pm.insert(DATA_LINE_ID, object.getLine()->getCommercialLine()->getKey()); //18
			pm.insert(DATA_ROUTE_ID, object.getLine()->getKey()); //19

			page->display(stream, request, pm);
		}



		void TimetableServiceRowInterfacePage::DisplayCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const TimetableColumn::Content::value_type& object,
			const RollingStock* rollingStock,
			size_t rank
		){
			ParametersMap pm(request.getFunction()->getTemplateParameters());

			if(object.first)
			{
				pm.insert(DATA_HOURS, Service::GetTimeOfDay(object.second).hours()); //0
				pm.insert(DATA_MINUTES, Service::GetTimeOfDay(object.second).minutes()); //1
				pm.insert(DATA_STOP_NAME, object.first->getConnectionPlace()->getName()); //2
				pm.insert(DATA_CITY_NAME, object.first->getConnectionPlace()->getCity()->getName()); //3
				pm.insert(DATA_STOP_ALIAS, object.first->getConnectionPlace()->getTimetableName()); //4
			}
			pm.insert(DATA_RANK, rank); //5
			if(rollingStock)
			{
				pm.insert(DATA_ROLLING_STOCK_ID, rollingStock->getKey()); //6
			}

			page->display(stream, request, pm);
		}
}	}
