
/** PTDataCleanerFileFormat class implementation.
	@file PTDataCleanerFileFormat.cpp

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

#include "PTDataCleanerFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "LineStopTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "RequestException.h"

using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace impex;
	using namespace graph;
	using namespace util;
	using namespace server;
	
	namespace pt
	{
		const string PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID("calendar_id");
		const string PTDataCleanerFileFormat::PARAMETER_START_DATE("start_date");
		const string PTDataCleanerFileFormat::PARAMETER_END_DATE("end_date");
		const string PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA("clean_old_data");
		const string PTDataCleanerFileFormat::PARAMETER_FROM_TODAY("from_today");

		PTDataCleanerFileFormat::PTDataCleanerFileFormat(
			util::Env& env,
			const DataSource& dataSource
		):	Importer(env, dataSource),
			_cleanOldData(true),
			_fromToday(false)
		{}



		void PTDataCleanerFileFormat::_cleanCalendars() const
		{
			if(!_cleanOldData)
			{
				return;
			}

			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(_dataSource, _env);

			date now(gregorian::day_clock::local_day());
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					ScheduledServiceTableSync::Search(_env, itPath->getKey());
					ContinuousServiceTableSync::Search(_env, itPath->getKey());

					BOOST_FOREACH(const ServiceSet::value_type& itService, itPath->getServices())
					{
						if(!dynamic_cast<NonPermanentService*>(itService))
						{
							continue;
						}

						NonPermanentService* service(static_cast<NonPermanentService*>(itService));
						if(_fromToday)
						{
							if(now <= service->getLastActiveDate())
							{
								Calendar dates(now, service->getLastActiveDate());
								service->subDates(dates);
							}
						}
						else
						{
							service->subDates(_calendar);
						}
					}
				}
			}
		}




		void PTDataCleanerFileFormat::_selectObjectsToRemove() const
		{
			if(!_cleanOldData)
			{
				return;
			}

			// Scheduled services without any active date
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& itService, _env.getRegistry<ScheduledService>())
			{
				if(itService.second->getRoute()->hasLinkWithSource(_dataSource) && itService.second->empty())
				{
					_scheduledServicesToRemove.insert(itService.second);
				}
			}
			
			// Continuous services without any active date
			BOOST_FOREACH(const Registry<ContinuousService>::value_type& itCService, _env.getRegistry<ContinuousService>())
			{
				if(itCService.second->getRoute()->hasLinkWithSource(_dataSource) && itCService.second->empty())
				{
					_continuousServicesToRemove.insert(itCService.second);
				}
			}

			// Journey patterns without any service
			BOOST_FOREACH(const Registry<JourneyPattern>::value_type& itJourneyPattern, _env.getRegistry<JourneyPattern>())
			{
				if(itJourneyPattern.second->hasLinkWithSource(_dataSource) && itJourneyPattern.second->getServices().empty())
				{
					_journeyPatternsToRemove.insert(itJourneyPattern.second);
				}
			}

			// Scheduled services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(shared_ptr<ScheduledService> sservice, _scheduledServicesToRemove)
			{
				_env.getEditableRegistry<ScheduledService>().remove(sservice->getKey());
			}

			// Continuous services to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(shared_ptr<ContinuousService> cservice, _continuousServicesToRemove)
			{
				_env.getEditableRegistry<ContinuousService>().remove(cservice->getKey());
			}

			// Journey patterns to delete are removed from the environment to avoid useless saving
			BOOST_FOREACH(shared_ptr<JourneyPattern> journeyPattern, _journeyPatternsToRemove)
			{
				_env.getEditableRegistry<JourneyPattern>().remove(journeyPattern->getKey());
			}
		}




		void PTDataCleanerFileFormat::_addRemoveQueries( db::DBTransaction& transaction ) const
		{
			if(!_cleanOldData)
			{
				return;
			}

			BOOST_FOREACH(shared_ptr<ScheduledService> sservice, _scheduledServicesToRemove)
			{
				ScheduledServiceTableSync::RemoveRow(sservice->getKey(), transaction);
			}
			BOOST_FOREACH(shared_ptr<ContinuousService> cservice, _continuousServicesToRemove)
			{
				ContinuousServiceTableSync::RemoveRow(cservice->getKey(), transaction);
			}
			BOOST_FOREACH(shared_ptr<JourneyPattern> journeyPattern, _journeyPatternsToRemove)
			{
				BOOST_FOREACH(const Edge* edge, journeyPattern->getEdges())
				{
					LineStopTableSync::RemoveRow(edge->getKey(), transaction);
				}
				JourneyPatternTableSync::RemoveRow(journeyPattern->getKey(), transaction);
			}
		}



		bool PTDataCleanerFileFormat::beforeParsing()
		{
			_cleanCalendars();
			return !_cleanOldData || !_calendar.empty() || _fromToday;
		}



		bool PTDataCleanerFileFormat::afterParsing()
		{
			_selectObjectsToRemove();
			return true;
		}



		void PTDataCleanerFileFormat::_setFromParametersMap( const util::ParametersMap& map )
		{
			_cleanOldData = map.getDefault<bool>(PARAMETER_CLEAN_OLD_DATA, true);

			RegistryKeyType calendarId(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID, 0));
			if(calendarId) try
			{
				_calendarTemplate = CalendarTemplateTableSync::Get(calendarId, _env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw RequestException("No such calendar template");
			}

			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}

			_fromToday = map.getDefault<bool>(PARAMETER_FROM_TODAY, false);

			_calendar.clear();
			if(!_fromToday)
			{
				if(_calendarTemplate.get())
				{
					if(_startDate && _endDate)
					{
						_calendar = _calendarTemplate->getResult(Calendar(*_startDate, *_endDate));
					}
					else if(_calendarTemplate->isLimited())
					{
						_calendar = _calendarTemplate->getResult();
						if(_startDate)
						{
							_calendar &= Calendar(*_startDate, _calendar.getLastActiveDate());
						}
						if(_endDate)
						{
							_calendar &= Calendar(_calendar.getFirstActiveDate(), *_endDate);
						}
					}
				}
				else
				{
					if(_startDate && _endDate)
					{
						_calendar = Calendar(*_startDate, *_endDate);
					}
				}
			}
		}



		util::ParametersMap PTDataCleanerFileFormat::_getParametersMap() const
		{
			ParametersMap result;
			result.insert(PARAMETER_CLEAN_OLD_DATA, _cleanOldData);
			if(_calendarTemplate.get())
			{
				result.insert(PARAMETER_CALENDAR_ID, _calendarTemplate->getKey());
			}
			if(_startDate)
			{
				result.insert(PARAMETER_START_DATE, *_startDate);
			}
			if(_endDate)
			{
				result.insert(PARAMETER_END_DATE, *_endDate);
			}
			result.insert(PARAMETER_FROM_TODAY, _fromToday);
			return result;
		}
}	}
