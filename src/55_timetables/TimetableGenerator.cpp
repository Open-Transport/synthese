
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
#include "Vertex.h"
#include "NonPermanentService.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Env.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace util;
	using namespace graph;

	namespace timetables
	{
		TimetableGenerator::TimetableGenerator(
			const Env& env
		):	_withContinuousServices(true),
			_env(env)
			, _maxColumnsNumber(UNKNOWN_VALUE)
		{

		}




		const TimetableGenerator::Warnings& TimetableGenerator::getWarnings() const
		{
			return _warnings;
		}


		void TimetableGenerator::build()
		{
			assert(_columns.empty());

			// Loop on each line of the database
			BOOST_FOREACH(shared_ptr<const Line> linePtr, _env.getRegistry<Line>())
			{
				// Line selection
				const Line& line(*linePtr);
				if (!_isLineSelected(line))
					continue;

				// Loop on each service
				BOOST_FOREACH(const Service* servicePtr, line.getServices())
				{
					// Permanent service filter
					const NonPermanentService* service(dynamic_cast<const NonPermanentService*>(servicePtr));
					if (service == NULL)
						continue;

					// Calendar filter
					if(	!(_baseCalendar || *service)
						|| !_withContinuousServices && service->isContinuous()
					)	continue;

					// Column creation
					TimetableColumn col(*this, *service);

					// Column storage or merge
					_insert(col);
				}
			}

			_buildWarnings();
		}



		const TimetableGenerator::Rows& TimetableGenerator::getRows() const
		{
			return _rows;
		}



		void TimetableGenerator::_insert( const TimetableColumn& col )
		{
			Columns::iterator itCol;
			for (itCol = _columns.begin(); itCol != _columns.end(); ++itCol)
			{
				if (*itCol == col)
				{
					itCol->merge(col);
					return;
				}

				if (col <= *itCol)
				{
					_columns.insert(itCol, col);
					return;
				}
			}
			_columns.push_back(col);
		}



		void TimetableGenerator::_buildWarnings()
		{
			assert(_warnings.empty());

			int nextNumber(1);
			for(Columns::iterator itCol(_columns.begin()); itCol != _columns.end(); ++itCol)
			{
				Warnings::const_iterator itWarn;
				for(itWarn = _warnings.begin(); itWarn != _warnings.end() && !(itCol->getCalendar() == itWarn->getCalendar()); ++itWarn);
				
				if (itWarn == _warnings.end())
					itWarn = _warnings.insert(_warnings.end(), TimetableWarning(itCol->getCalendar(), nextNumber++));
				
				itCol->setWarning(itWarn);
			}
		}



		bool TimetableGenerator::_isLineSelected( const env::Line& line ) const
		{
			if (!line.getUseInTimetables())
				return false;

			bool lineIsSelected(false);
			bool passageOk(false);
			Path::Edges::const_iterator itEdge;
			const Path::Edges& edges(line.getEdges());


			// A0: Line selection upon calendar
			if (!line.getAllDays() && !(_baseCalendar || line))
				return false;


			// A1: Line selection : there must be at least a departure stop of the line in the departures rows
			Rows::const_iterator itRow;
			for (itRow = _rows.begin(); itRow != _rows.end(); ++itRow)
			{
				if (!itRow->getIsDeparture())
					continue;

				for (itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
				{
					if((*itEdge)->isDeparture() && (*itEdge)->getPlace() == itRow->getPlace())
					{
						lineIsSelected = true;
						if (itRow->getIsArrival() || itRow->getCompulsory() == PassageSuffisant)
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
			lineIsSelected = false;
			const LineStop* departureLinestop(static_cast<const LineStop*>(*itEdge));

			for (++itRow; itRow != _rows.end(); ++itRow)
			{
				if(	(	itRow->getIsArrival()
					&&(	passageOk
						|| itRow->getCompulsory() == PassageSuffisant
					)
				)||(itRow->getIsDeparture()
					&& itRow->getIsArrival()
					)
				){
					for(const Edge* arrivalLinestop(departureLinestop->getFollowingArrivalForFineSteppingOnly());
						arrivalLinestop != NULL;
						arrivalLinestop = arrivalLinestop->getFollowingArrivalForFineSteppingOnly()
					){
						if(	arrivalLinestop->getFromVertex()->getPlace() == itRow->getPlace()
						){
							lineIsSelected = true;
							break;
						}
					}
					if (lineIsSelected)
						break;
				}
			}

			return lineIsSelected;
		}



		void TimetableGenerator::setRows( const Rows& rows)
		{
			_rows = rows;
		}



		std::vector<time::Schedule> TimetableGenerator::getSchedulesByRow( Rows::const_iterator row ) const
		{
			int delta(row - _rows.begin());
			vector<Schedule> result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
			{
				vector<Schedule> content(it->getContent());
				result.push_back(*(content.begin() + delta));
			}
			return result;
		}



		void TimetableGenerator::setBaseCalendar(
			const Calendar& value
		){
			_baseCalendar = value;
		}



		std::vector<const env::Line*> TimetableGenerator::getLines() const
		{
			vector<const Line*> result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getLine());
			return result;
		}



		std::vector<tTypeOD> TimetableGenerator::getOriginTypes() const
		{
			vector<tTypeOD> result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getOriginType());
			return result;
		}



		std::vector<tTypeOD> TimetableGenerator::getDestinationTypes() const
		{
			vector<tTypeOD> result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getDestinationType());
			return result;
		}



		std::vector<std::vector<TimetableWarning>::const_iterator> TimetableGenerator::getColumnsWarnings() const
		{
			std::vector<std::vector<TimetableWarning>::const_iterator> result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getWarning());
			return result;
		}
	}
}
