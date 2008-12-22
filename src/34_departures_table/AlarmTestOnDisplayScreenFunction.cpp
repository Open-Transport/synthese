
/** AlarmTestOnDisplayScreenFunction class implementation.
	@file AlarmTestOnDisplayScreenFunction.cpp
	@author Hugues Romain
	@date 2008

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

#include "01_util/Conversion.h"

#include "30_server/RequestException.h"
#include "30_server/RequestMissingParameterException.h"
#include "30_server/QueryString.h"

#include "17_messages/Alarm.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/AlarmTableSync.h"

#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/Types.h"
#include "34_departures_table/DeparturesTableInterfacePage.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"

#include "04_time/DateTime.h"

#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/ServicePointer.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"
#include "15_env/City.h"
#include "15_env/CommercialLine.h"

#include "AlarmTestOnDisplayScreenFunction.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace messages;
	using namespace env;
	using namespace interfaces;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<Function, departurestable::AlarmTestOnDisplayScreenFunction>::FACTORY_KEY("AlarmTestOnDisplayScreenFunction");
	}

	namespace departurestable
	{
		const string AlarmTestOnDisplayScreenFunction::PARAMETER_DISPLAY_TYPE_ID("dt");
		

		ParametersMap AlarmTestOnDisplayScreenFunction::_getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get())
				map.insert(QueryString::PARAMETER_OBJECT_ID, _alarm->getKey());
			return map;
		}

		void AlarmTestOnDisplayScreenFunction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			setAlarmId(id);

			id = map.getUid(PARAMETER_DISPLAY_TYPE_ID, true, FACTORY_KEY);
			try
			{
				_type = DisplayTypeTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw RequestException("No such display type");
			}
		}

		void AlarmTestOnDisplayScreenFunction::_run( std::ostream& stream ) const
		{
			try
			{
				ArrivalDepartureListWithAlarm displayedObject;

				auto_ptr<City> city(new City);
				city->setName("CITY");
				auto_ptr<PublicTransportStopZoneConnectionPlace> place(new PublicTransportStopZoneConnectionPlace);
				place->setName("TEST");
				place->setCity(city.get());
				vector<const PublicTransportStopZoneConnectionPlace*> places;
				places.push_back(place.get());
				places.push_back(place.get());
				auto_ptr<CommercialLine> cline(new CommercialLine);
				cline->setShortName("00");
				auto_ptr<Line> line(new Line);
				line->setCommercialLine(cline.get());
				auto_ptr<LineStop> lineStop(new LineStop);
				lineStop->setLine(line.get());
				DateTime d(TIME_CURRENT);
				

				for (int i(0); i<_type->getRowNumber(); ++i)
				{
					ServicePointer sp(DEPARTURE_TO_ARRIVAL,lineStop.get());
					sp.setActualTime(d);
					DeparturesTableElement dte(sp, false);
					displayedObject.map.insert(make_pair(dte, places));
					d += 1;
				}

				auto_ptr<SingleSentAlarm> alarm(new SingleSentAlarm);
				alarm->setShortMessage(_alarm->getShortMessage());
				alarm->setLongMessage(_alarm->getLongMessage());
				alarm->setLevel(_alarm->getLevel());
				alarm->setIsEnabled(true);

				displayedObject.alarm = alarm.get();
				if (_type->getDisplayInterface() == NULL) return;
				const DeparturesTableInterfacePage* page(_type->getDisplayInterface()->getPage<DeparturesTableInterfacePage>());
				if (page == NULL) return;

				VariablesMap variables;

				page->display(
					stream
					, variables
					, "TEST"
					, 0
					, false
					, false
					, false
					, _type->getMaxStopsNumber()
					, place.get()
					, displayedObject
				);
			}
			catch (InterfacePageException& e)
			{
			}
		}

		void AlarmTestOnDisplayScreenFunction::setAlarmId(uid id)
		{
			try
			{
				_alarm = AlarmTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw RequestException("No such alarm");
			}
		}
	}
}
