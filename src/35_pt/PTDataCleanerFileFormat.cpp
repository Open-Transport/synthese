
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
#include "CalendarTemplate.h"

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
		const string PTDataCleanerFileFormat::PARAMETER_DATE("date");

		PTDataCleanerFileFormat::PTDataCleanerFileFormat(
			util::Env& env,
			const DataSource& dataSource
		):	Importer(env, dataSource)
		{}



		void PTDataCleanerFileFormat::_cleanCalendars() const
		{
			ImportableTableSync::ObjectBySource<JourneyPatternTableSync> journeyPatterns(_dataSource, _env);

			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::value_type& itPathSet, journeyPatterns.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<JourneyPatternTableSync>::Map::mapped_type::value_type& itPath, itPathSet.second)
				{
					BOOST_FOREACH(const ServiceSet::value_type& itService, itPath->getServices())
					{
						if(!dynamic_cast<NonPermanentService*>(itService))
						{
							continue;
						}

						NonPermanentService* service(static_cast<NonPermanentService*>(itService));
						if(_startDate)
						{
							date lastServiceDate(service->getLastActiveDate());
							if(lastServiceDate >= *_startDate)
							{
								Calendar mask(*_startDate, lastServiceDate);
								service->subDates(mask);
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
			return true;
		}



		bool PTDataCleanerFileFormat::afterParsing()
		{
			_selectObjectsToRemove();
			return true;
		}



		void PTDataCleanerFileFormat::_setFromParametersMap( const server::ParametersMap& map )
		{
			//TODO Implement
		}



		server::ParametersMap PTDataCleanerFileFormat::_getParametersMap() const
		{
			ParametersMap result;
			if(_calendarTemplate.get())
			{
				result.insert(PARAMETER_CALENDAR_ID, _calendarTemplate->getKey());
			}
			if(_startDate)
			{
				result.insert(PARAMETER_DATE, *_startDate);
			}
			return result;
		}



		void PTDataCleanerFileFormat::setCalendar( boost::shared_ptr<const calendar::CalendarTemplate> value )
		{
			_calendarTemplate = value;
		}



		void PTDataCleanerFileFormat::setStartDate( boost::optional<boost::gregorian::date> value )
		{
			_startDate = value;
		}
}	}
