////////////////////////////////////////////////////////////////////////////////
/// AlarmTestOnDisplayScreenFunction class implementation.
///	@file AlarmTestOnDisplayScreenFunction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "AlarmTestOnDisplayScreenFunction.h"

#include "LineStop.h"
#include "MessagesModule.h"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "ArrivalDepartureTableRight.h"
#include "Alarm.h"
#include "AlarmTableSync.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableTypes.h"
#include "StopArea.hpp"
#include "ServicePointer.h"
#include "JourneyPattern.hpp"
#include "LinePhysicalStop.hpp"
#include "City.h"
#include "CommercialLine.h"
#include "GraphConstants.h"
#include "SentScenario.h"
#include "StopPoint.hpp"
#include "PermanentService.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace server;
	using namespace messages;
	using namespace security;
	using namespace graph;
	using namespace geography;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Function, departure_boards::AlarmTestOnDisplayScreenFunction>::FACTORY_KEY("AlarmTestOnDisplayScreenFunction");
	}

	namespace departure_boards
	{
		const string AlarmTestOnDisplayScreenFunction::PARAMETER_DISPLAY_TYPE_ID("dt");


		AlarmTestOnDisplayScreenFunction::AlarmTestOnDisplayScreenFunction()
			: util::FactorableTemplate<server::Function,AlarmTestOnDisplayScreenFunction>()
		{
			setEnv(boost::shared_ptr<Env>(new Env));
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
			// Message
			setAlarmId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));

			// Display type
			try
			{
				_type = DisplayTypeTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DISPLAY_TYPE_ID), *_env);
			}
			catch (...)
			{
				throw RequestException("No such display type");
			}
		}



		util::ParametersMap AlarmTestOnDisplayScreenFunction::run( std::ostream& stream, const Request& request ) const
		{
			DisplayScreen fakeScreen;
			ArrivalDepartureList displayedObject;

			City city;
			city.set<Name>("CITY");
			StopArea place;
			place.setName("TEST");
			place.setCity(&city);
			StopPoint ps(0, string(), &place);
			place.addPhysicalStop(ps);
			CommercialLine cline;
			cline.setShortName("00");
			JourneyPattern line;
			line.setCommercialLine(&cline);
			PermanentService s(0, &line, minutes(5));
			LineStop lineStop;
			lineStop.set<Line>(line);
			lineStop.set<LineNode>(ps);
			ptime now(second_clock::local_time());
			ptime d(now);

			for (size_t i(0); i<_type->get<RowsNumber>(); ++i)
			{
				ServicePointer sp(true, false, USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET, s, d);
				sp.setDepartureInformations(**lineStop.getGeneratedLineStops().begin(), d, d, ps);
				sp.setArrivalInformations(**lineStop.getGeneratedLineStops().begin(), d, d, ps);
				ActualDisplayedArrivalsList destinations;

				destinations.push_back(IntermediateStop(&place, sp));
				destinations.push_back(IntermediateStop(&place, sp));

				displayedObject.insert(
					make_pair(
						sp,
						destinations
				)	);
				d += minutes(1);
			}

			SentScenario scenario;
			scenario.setIsEnabled(true);

			boost::shared_ptr<Alarm> alarm(new Alarm(1));
			alarm->setScenario(&scenario);
			alarm->setShortMessage(_alarm->getShortMessage());
			alarm->setLongMessage(_alarm->getLongMessage());
			alarm->setLevel(_alarm->getLevel());

			Env::GetOfficialEnv().getEditableRegistry<Alarm>().add(alarm);
			MessagesModule::UpdateActivatedMessages();

			if(_type->get<DisplayMainPage>())
			{
				DisplayScreenContentFunction f;
				f._displayDepartureBoard(
					stream,
					request,
					Env::GetOfficialEnv().getSPtr(_type->get<DisplayMainPage>().get_ptr()),
					Env::GetOfficialEnv().getSPtr(_type->get<DisplayRowPage>().get_ptr()),
					Env::GetOfficialEnv().getSPtr(_type->get<DisplayDestinationPage>().get_ptr()),
					Env::GetOfficialEnv().getSPtr(_type->get<DisplayTransferDestinationPage>().get_ptr()),
					now,
					displayedObject,
					fakeScreen
				);
			}

			Env::GetOfficialEnv().getEditableRegistry<Alarm>().remove(alarm->getKey());
			MessagesModule::UpdateActivatedMessages();

			return util::ParametersMap();
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
			const Webpage* page(
				_type->get<DisplayMainPage>().get_ptr()
			);
			return (page == NULL) ? "text/plain" : page->getMimeType();
		}
	}
}
