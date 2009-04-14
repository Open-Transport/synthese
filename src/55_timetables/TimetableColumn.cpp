
/** TimetableColumn class implementation.
	@file TimetableColumn.cpp

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

#include "TimetableColumn.h"

#include "Schedule.h"

#include "TimetableRow.h"
#include "TimetableGenerator.h"

#include "NonPermanentService.h"
#include "Path.h"
#include "Line.h"
#include "Edge.h"
#include "Vertex.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace graph;

	namespace timetables
	{
		TimetableColumn::TimetableColumn(
			const TimetableGenerator& timetablegenerator,
			const env::NonPermanentService& service
		):	_warning(timetablegenerator.getWarnings().end())
			, _line(static_cast<const Line*>(service.getPath()))
			, _calendar(service)
		{
			const TimetableGenerator::Rows& rows(timetablegenerator.getRows());
			const Path::Edges& edges(service.getPath()->getEdges());
			Path::Edges::const_iterator itEdge(edges.begin());

			
			for(TimetableGenerator::Rows::const_iterator itRow(rows.begin()); itRow != rows.end(); ++itRow)
			{
				Path::Edges::const_iterator itEdge2;
				for (itEdge2 = itEdge; itEdge2 != edges.end(); ++itEdge2)
				{
					if(	(*itEdge2)->getFromVertex()->getHub() == itRow->getPlace()
						&&(	(*itEdge2)->isDeparture() == itRow->getIsDeparture()
							|| (*itEdge2)->isArrival() == itRow->getIsArrival()
						)
					){
						_timeContent.push_back(
							((*itEdge2)->isDeparture() == itRow->getIsDeparture())
							? service.getDepartureBeginScheduleToIndex(itEdge2 - edges.begin())
							: service.getArrivalBeginScheduleToIndex(itEdge2 - edges.begin())
						);
						if (itEdge2 == edges.begin())
							_originType = Terminus;
						if (itEdge2 == edges.end() - 1)
							_destinationType = Terminus;
						itEdge = itEdge2 + 1;
						break;
					}
				}
				if (itEdge2 == edges.end())
				{
					_timeContent.push_back(Schedule());
				}
			}
		}



		int TimetableColumn::operator<=( const TimetableColumn& other) const
		{
			assert(_timeContent.size() == other._timeContent.size());

			int rowsNumber(_timeContent.size());
			int i;
			int j;

			// Tentative par ligne commune
			for (i=0; i<rowsNumber; ++i)
			{
				if (!_timeContent[i].getHour().isUnknown() && !other._timeContent[i].getHour().isUnknown())
				{
					if (_timeContent[i] < other._timeContent[i])
						return true;
					if (other._timeContent[i] < _timeContent[i])
						return false;
				}
			}

			// Tentative par succession 1
			for (int i=0; i< rowsNumber; ++i)
			{
				if (!_timeContent[i].getHour().isUnknown())
				{
					for(j = i+1; j< rowsNumber; ++j)
					{
						if(	!other._timeContent[j].getHour().isUnknown()
						&& other._timeContent[j] < _timeContent[i]
						)	return false;
						
						if (i != 0)
						{
							for(int k = i-1; k>0; --k)
							{
								if(	!other._timeContent[k].getHour().isUnknown()
								&&	_timeContent[i] < other._timeContent[k]
								)	return true;
							}
						}
					}
				}
			}

			// Premiere heure
			for (i=0; i< rowsNumber; ++i)
				if (!_timeContent[i].getHour().isUnknown())
					break;
			for (j=0; j< rowsNumber; ++j)
				if (!other._timeContent[j].getHour().isUnknown())
					break;
			return _timeContent[i] < other._timeContent[j];
		}



		void TimetableColumn::merge( const TimetableColumn& col )
		{
			assert(col == *this);

			if( _line->getOrigin()->getHub() != col._line->getOrigin()->getHub()
			){
				_originType = Indetermine;
			}
			if(	_line->getDestination()->getHub() != col._line->getDestination()->getHub()
			){
				_destinationType = Indetermine;
			}
			
			_calendar |= col._calendar;
		}



		bool TimetableColumn::operator==( const TimetableColumn& op ) const
		{
			assert(op._timeContent.size() == _timeContent.size());

			return _line->getCommercialLine() == op._line->getCommercialLine()
				&& _timeContent == op._timeContent;
		}



		void TimetableColumn::setWarning( vector<TimetableWarning>::const_iterator it )
		{
			_warning = it;
		}



		const Calendar& TimetableColumn::getCalendar() const
		{
			return _calendar;
		}



		const vector<time::Schedule>& TimetableColumn::getContent() const
		{
			return _timeContent;
		}



		const env::Line* TimetableColumn::getLine() const
		{
			return _line;
		}



		synthese::timetables::tTypeOD TimetableColumn::getOriginType() const
		{
			return _originType;
		}



		synthese::timetables::tTypeOD TimetableColumn::getDestinationType() const
		{
			return _destinationType;
		}



		std::vector<TimetableWarning>::const_iterator TimetableColumn::getWarning() const
		{
			return _warning;
		}
	}
}
