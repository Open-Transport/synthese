
/** TimetableGenerator class implementation.
	@file TimetableGenerator.cpp

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

#include "TimetableGenerator.h"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "SchedulesBasedService.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Env.h"
#include "CalendarModule.h"
#include "SubLine.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace calendar;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace timetables
	{
		TimetableGenerator::TimetableGenerator(
			const Env& env
		):	_withContinuousServices(true),
			_env(env)
			, _maxColumnsNumber(UNKNOWN_VALUE)
		{
		}



		TimetableResult TimetableGenerator::build()
		{
			TimetableResult result;

			if(!_rows.empty())
			{
				// Loop on each line of the database
				BOOST_FOREACH(Registry<Line>::value_type it, _env.getRegistry<Line>())
				{
					// Line selection
					const Line& line(*it.second);
					if (!_isLineSelected(line))
						continue;

					_scanServices(result, line);

					BOOST_FOREACH(const Line::SubLines::value_type& subline, line.getSubLines())
					{
						_scanServices(result, *subline);
					}
				}

				_buildWarnings(result);

			}
			return result;
		}



		void TimetableGenerator::_scanServices(TimetableResult& result, const pt::Line& line )
		{
			// Loop on each service
			BOOST_FOREACH(const Service* servicePtr, line.getServices())
			{
				// Permanent service filter
				const SchedulesBasedService* service(dynamic_cast<const SchedulesBasedService*>(servicePtr));
				if (service == NULL)
					continue;

				// Calendar filter
				if(	!(_baseCalendar.hasAtLeastOneCommonDateWith(*service))
					|| !_withContinuousServices && service->isContinuous()
				)	continue;

				// Column creation
				TimetableColumn col(*this, *service);

				// Column storage or merge
				_insert(result, col);
			}
		}



		void TimetableGenerator::_insert(TimetableResult& result, const TimetableColumn& col )
		{
			TimetableResult::Columns::iterator itCol;
			for (itCol = result.getColumns().begin(); itCol != result.getColumns().end(); ++itCol)
			{
				if (*itCol == col)
				{
					itCol->merge(col);
					return;
				}

				if (col <= *itCol)
				{
					result.getColumns().insert(itCol, col);
					return;
				}
			}
			result.getColumns().push_back(col);
		}



		void TimetableGenerator::_buildWarnings(TimetableResult& result)
		{
			int nextNumber(1);
			for(TimetableResult::Columns::iterator itCol(result.getColumns().begin()); itCol != result.getColumns().end(); ++itCol)
			{
				if(itCol->getCalendar() == _baseCalendar) continue;

				const TimetableWarning* warn(NULL);
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& itWarn, result.getWarnings())
				{
					if(itWarn.second.getCalendar() == itCol->getCalendar())
					{
						warn = &itWarn.second;
						break;
					}
				}
				
				if (warn == NULL)
				{
					warn = &result.getWarnings().insert(
						make_pair(
							nextNumber,
							TimetableWarning(
								itCol->getCalendar(),
								nextNumber,
								CalendarModule::GetBestCalendarTitle(itCol->getCalendar(), _baseCalendar)
					)	)	).first->second;
					++nextNumber;
				}
				
				itCol->setWarning(warn);
			}
		}



		bool TimetableGenerator::_isLineSelected( const pt::Line& line ) const
		{
			if (!line.getUseInTimetables())
				return false;

			bool lineIsSelected(false);
			bool passageOk(false);
			Path::Edges::const_iterator itEdge;
			const Path::Edges& edges(line.getEdges());

			// Line is authorized
			if(!_authorizedLines.empty() && _authorizedLines.find(line.getCommercialLine()) == _authorizedLines.end())
			{
				return false;
			}

			// A0: Line selection upon calendar
			if (!line.getAllDays() && !_baseCalendar.hasAtLeastOneCommonDateWith(line))
				return false;


			// A1: Line selection : there must be at least a departure stop of the line in the departures rows
			Rows::const_iterator itRow;
			for (itRow = _rows.begin(); itRow != _rows.end(); ++itRow)
			{
				if (!itRow->getIsDeparture())
					continue;

				for (itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
				{
					if(	(*itEdge)->isDeparture() &&
						dynamic_cast<const PublicTransportStopZoneConnectionPlace*>((*itEdge)->getHub())->getKey() == itRow->getPlace()->getKey() &&
						(	_authorizedPhysicalStops.empty() ||
							_authorizedPhysicalStops.find(dynamic_cast<const PhysicalStop*>((*itEdge)->getFromVertex())) != _authorizedPhysicalStops.end()
						)	
					){
						lineIsSelected = true;
						if (itRow->getIsArrival() || itRow->getCompulsory() == TimetableRow::PassageSuffisant)
							passageOk = true;
						break;
					}
				}
				if (lineIsSelected)
					break;
			}
			if (!lineIsSelected)
				return false;


			// A2: Line selection : there must be at least an arrival stop of the line in the arrival rows, after the departure
			// this test is ignored if the timetable is defined only by a departure stop
			if(_rows.size() > 1)
			{
				lineIsSelected = false;
				const LineStop* departureLinestop(static_cast<const LineStop*>(*itEdge));

				for (++itRow; itRow != _rows.end(); ++itRow)
				{
					if(	(	itRow->getIsArrival() &&
							(	passageOk ||
								itRow->getCompulsory() == TimetableRow::PassageSuffisant
						)	) ||
						(	itRow->getIsDeparture() &&
							itRow->getIsArrival()
						)
					){
						for(const Edge* arrivalLinestop(departureLinestop->getFollowingArrivalForFineSteppingOnly());
							arrivalLinestop != NULL;
							arrivalLinestop = arrivalLinestop->getFollowingArrivalForFineSteppingOnly()
						){
							if(	dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(arrivalLinestop->getFromVertex()->getHub())->getKey() == itRow->getPlace()->getKey()
							){
								lineIsSelected = true;
								break;
							}
						}
						if (lineIsSelected)
							break;
					}
				}
			}

			return lineIsSelected;
		}
	}
}
