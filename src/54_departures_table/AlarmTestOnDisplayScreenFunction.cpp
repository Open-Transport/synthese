////////////////////////////////////////////////////////////////////////////////
/// AlarmTestOnDisplayScreenFunction class implementation.
///	@file AlarmTestOnDisplayScreenFunction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "Request.h"
#include "ArrivalDepartureTableRight.h"
#include "SentAlarm.h"
#include "AlarmTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableTypes.h"
#include "DeparturesTableInterfacePage.h"
#include "Interface.h"
#include "InterfacePage.h"
#include "InterfacePageException.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ServicePointer.h"
#include "Line.h"
#include "LineStop.h"
#include "City.h"
#include "CommercialLine.h"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "GraphConstants.h"
#include "InterfacePageTableSync.h"
#include "SentScenario.h"
#include "PhysicalStop.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace messages;
	using namespace env;
	using namespace interfaces;
	using namespace security;
	using namespace graph;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Function, departurestable::AlarmTestOnDisplayScreenFunction>::FACTORY_KEY("AlarmTestOnDisplayScreenFunction");
	}

	namespace departurestable
	{
		const string AlarmTestOnDisplayScreenFunction::PARAMETER_DISPLAY_TYPE_ID("dt");
		

		AlarmTestOnDisplayScreenFunction::AlarmTestOnDisplayScreenFunction()
			: util::FactorableTemplate<server::Function,AlarmTestOnDisplayScreenFunction>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}

		ParametersMap AlarmTestOnDisplayScreenFunction::_getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get())
				map.insert(Request::PARAMETER_OBJECT_ID, _alarm->getKey());
			return map;
		}

		void AlarmTestOnDisplayScreenFunction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			setAlarmId(id);

			id = map.getUid(PARAMETER_DISPLAY_TYPE_ID, true, FACTORY_KEY);
			try
			{
				_type = DisplayTypeTableSync::Get(id, *_env);
				if (!_type->getDisplayInterface())
					throw RequestException("The specified type has no display interface");
				InterfacePageTableSync::Search(
					*_env,
					_type->getDisplayInterface()->getKey(),
					optional<int>(),
					optional<int>(),
					UP_DOWN_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw RequestException("No such display type");
			}
		}

		void AlarmTestOnDisplayScreenFunction::run( std::ostream& stream, const Request& request ) const
		{
			try
			{
				ArrivalDepartureListWithAlarm displayedObject;

				City city;
				city.setName("CITY");
				PublicTransportStopZoneConnectionPlace place;
				place.setName("TEST");
				place.setCity(&city);
				PhysicalStop ps(UNKNOWN_VALUE, string(), &place);
				CommercialLine cline;
				cline.setShortName("00");
				Line line;
				line.setCommercialLine(&cline);
				LineStop lineStop;
				lineStop.setLine(&line);
				lineStop.setPhysicalStop(&ps);
				ptime d(second_clock::local_time());

				for (int i(0); i<_type->getRowNumber(); ++i)
				{
					ServicePointer sp(false, DEPARTURE_TO_ARRIVAL, USER_PEDESTRIAN, &lineStop);
					sp.setActualTime(d);
					sp.setTheoreticalTime(d);
					ActualDisplayedArrivalsList destinations;
					destinations.push_back(IntermediateStop(&place));
					destinations.push_back(IntermediateStop(&place));

					displayedObject.map.insert(
						make_pair(
							sp,
							destinations		
					)	);
					d += days(1);
				}

				SentScenario scenario;
				scenario.setIsEnabled(true);

				SentAlarm alarm;
				alarm.setShortMessage(_alarm->getShortMessage());
				alarm.setLongMessage(_alarm->getLongMessage());
				alarm.setLevel(_alarm->getLevel());
				alarm.setScenario(&scenario);

				displayedObject.alarm = &alarm;
				const DeparturesTableInterfacePage* page(
					_type->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
				);
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
					, _type->getMaxStopsNumber(),
					0,
					true
					, &place
					, displayedObject
				);
			}
			catch (InterfacePageException& e)
			{
			}
		}



		void AlarmTestOnDisplayScreenFunction::setAlarmId(RegistryKeyType id)
		{
			try
			{
				_alarm = AlarmTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw RequestException("No such alarm");
			}
		}



		bool AlarmTestOnDisplayScreenFunction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}

		std::string AlarmTestOnDisplayScreenFunction::getOutputMimeType() const
		{
			const DeparturesTableInterfacePage* page(
				_type->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
				);
			return (page == NULL) ? "text/plain" : page->getMimeType();
		}
	}
}
