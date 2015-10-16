
//////////////////////////////////////////////////////////////////////////
/// ServiceAddAction class implementation.
/// @file ServiceAddAction.cpp
/// @author Hugues
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ServiceAddAction.h"

#include "ActionException.h"
#include "CalendarLink.hpp"
#include "CalendarLinkTableSync.hpp"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "DBTransaction.hpp"
#include "Destination.hpp"
#include "Fetcher.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace db;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceAddAction>::FACTORY_KEY("ServiceAddAction");
	}

	namespace pt
	{
		const string ServiceAddAction::PARAMETER_END_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "ed";
		const string ServiceAddAction::PARAMETER_IS_CONTINUOUS = Action_PARAMETER_PREFIX + "ic";
		const string ServiceAddAction::PARAMETER_LINE_ID = Action_PARAMETER_PREFIX + "li";
		const string ServiceAddAction::PARAMETER_NUMBER = Action_PARAMETER_PREFIX + "nu";
		const string ServiceAddAction::PARAMETER_PERIOD = Action_PARAMETER_PREFIX + "pe";
		const string ServiceAddAction::PARAMETER_START_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "sd";
		const string ServiceAddAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";



		ServiceAddAction::ServiceAddAction():
			_period(minutes(0)),
			_startDepartureTime(not_a_date_time),
			_endDepartureTime(not_a_date_time)
		{

		}



		ParametersMap ServiceAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(PARAMETER_LINE_ID, _line->getKey());
			}
			map.insert(PARAMETER_IS_CONTINUOUS, _isContinuous);
			map.insert(PARAMETER_NUMBER, _number);
			map.insert(PARAMETER_PERIOD, _period.total_seconds() / 60);
			return map;
		}



		void ServiceAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_isContinuous = map.get<bool>(PARAMETER_IS_CONTINUOUS);
			_number = map.getDefault<string>(PARAMETER_NUMBER);
			if(map.getDefault<string>(PARAMETER_START_DEPARTURE_TIME).empty())
			{
				throw ActionException("A start time must be provided");
			}
			try
			{
				_startDepartureTime = duration_from_string(map.get<string>(PARAMETER_START_DEPARTURE_TIME));
			}
			catch(bad_lexical_cast)
			{
				throw ActionException("Bad start time");
			}
			if(!map.getDefault<string>(PARAMETER_END_DEPARTURE_TIME).empty())
			{
				try
				{
					_endDepartureTime = duration_from_string(map.get<string>(PARAMETER_END_DEPARTURE_TIME));
				}
				catch(bad_lexical_cast)
				{
				}
			}

			if(!_endDepartureTime.is_not_a_date_time())
			{
				if (_endDepartureTime < _startDepartureTime)
				{
					throw ActionException("End departure time must be after the start departure time");
				}
				if(_endDepartureTime - _startDepartureTime >= hours(24))
				{
					throw ActionException("End departure time must not be more than one day after the start departure time");
				}
			}

			// Period
			_period = minutes(map.getDefault<int>(PARAMETER_PERIOD, 0));
			if(_period.total_seconds() < 0)
			{
				throw ActionException("Period must be positive");
			}

			// Journey pattern
			try
			{
				_line = JourneyPatternTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_ID), Env::GetOfficialEnv());
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such line");
			}

			// Template
			if(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0))
			{
				try
				{
					_template = Fetcher<SchedulesBasedService>::Fetch(
						map.get<RegistryKeyType>(PARAMETER_TEMPLATE_ID),
						Env::GetOfficialEnv()
					);
				}
				catch(ObjectNotFoundException<SchedulesBasedService>&)
				{
					throw ActionException("No such service template");
				}
			}
		}



		void ServiceAddAction::run(
			Request& request
		){
			boost::shared_ptr<const SchedulesBasedService> timetemplate(_template);
			if(!timetemplate.get()) // Attempt to load an other service to read the schedules
			{
				ScheduledServiceTableSync::SearchResult services(ScheduledServiceTableSync::Search(*_env, _line->getKey(), optional<RegistryKeyType>(), optional<RegistryKeyType>(), optional<string>(), false, 0, 1));
				if(!services.empty())
				{
					timetemplate = static_pointer_cast<const SchedulesBasedService>(services.front());
				}
				else
				{
					ContinuousServiceTableSync::SearchResult services(ContinuousServiceTableSync::Search(*_env, _line->getKey(), optional<RegistryKeyType>(), 0, 1));
					if(!services.empty())
					{
						timetemplate = static_pointer_cast<const SchedulesBasedService>(services.front());
					}
				}
			}

			DBTransaction transaction;

			if(_isContinuous)
			{
				ContinuousService object;
				object.setServiceNumber(_number);
				object.setPath(_line.get());
				object.setMaxWaitingTime(_period);
				object.setRange(_endDepartureTime - _startDepartureTime);
				if(_template.get())
				{
					object.copyDates(*_template);
				}

				if(timetemplate.get())
				{
					object.setSchedulesFromOther(*timetemplate, _startDepartureTime - timetemplate->getDepartureSchedule(false, 0));
				}
				else
				{
					object.generateIncrementalSchedules(_startDepartureTime);
				}

				ContinuousServiceTableSync::Save(&object, transaction);

				//::AddCreationEntry(object, request.getUser().get());

				request.setActionCreatedId(object.getKey());
			}
			else
			{
				ScheduledService object;
				object.setServiceNumber(_number);
				object.setPath(_line.get());

				if(timetemplate.get())
				{
					object.setSchedulesFromOther(*timetemplate, _startDepartureTime - timetemplate->getDepartureSchedule(false, 0));
				}
				else
				{
					object.generateIncrementalSchedules(_startDepartureTime);
				}

				if(_template.get())
				{
					object.copyDates(*_template);
				}

				ScheduledServiceTableSync::Save(&object, transaction);

				// Copy of calendar template elements
				if(_template.get() && !_template->getCalendarLinks().empty())
				{
					BOOST_FOREACH(CalendarLink* calendarLink, _template->getCalendarLinks())
					{
						CalendarLink newLink;
						newLink.setCalendar(&object);
						newLink.setCalendarTemplate(calendarLink->getCalendarTemplate());
						newLink.setCalendarTemplate2(calendarLink->getCalendarTemplate2());
						newLink.setStartDate(calendarLink->getStartDate());
						newLink.setEndDate(calendarLink->getEndDate());
						object.addCalendarLink(newLink,true);
						CalendarLinkTableSync::Save(&newLink, transaction);
					}
				}

				request.setActionCreatedId(object.getKey());

				if(_period.total_seconds() > 0 && !_endDepartureTime.is_not_a_date_time())
				{
					int number(0);
					int step(1);
					try
					{
						number = lexical_cast<int>(_number);
						step = number - lexical_cast<int>(_template->getServiceNumber());
					}
					catch (bad_lexical_cast)
					{
					}

					for(time_duration departureTime(_startDepartureTime + _period); departureTime <= _endDepartureTime; departureTime += _period)
					{
						if(number > 0)
						{
							number += step;
						}
						ScheduledService object2;
						object2.setServiceNumber(number > 0 ? lexical_cast<string>(number) : string());
						object2.setPath(_line.get());
						object2.setSchedulesFromOther(object, departureTime - object.getDepartureSchedule(false, 0));
						object2.copyDates(object);

						ScheduledServiceTableSync::Save(&object2, transaction);

						// Copy of calendar template elements
						if(_template.get() && !_template->getCalendarLinks().empty())
						{
							BOOST_FOREACH(CalendarLink* calendarLink, _template->getCalendarLinks())
							{
								CalendarLink newLink;
								newLink.setCalendar(&object2);
								newLink.setCalendarTemplate(calendarLink->getCalendarTemplate());
								newLink.setCalendarTemplate2(calendarLink->getCalendarTemplate2());
								newLink.setStartDate(calendarLink->getStartDate());
								newLink.setEndDate(calendarLink->getEndDate());
								object2.addCalendarLink(newLink,true);
								CalendarLinkTableSync::Save(&newLink, transaction);
							}
						}
					}
				}
			}

			transaction.run();
		}



		bool ServiceAddAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}



		void ServiceAddAction::setIsContinuous( bool value )
		{
			_isContinuous = value;
		}



		void ServiceAddAction::setLine( boost::shared_ptr<pt::JourneyPattern> value )
		{
			_line = value;
		}
}	}
