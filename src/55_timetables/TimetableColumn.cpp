
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
	using namespace calendar;
	using namespace graph;
	using namespace time;

	namespace timetables
	{
		TimetableColumn::TimetableColumn(
			const TimetableGenerator& timetablegenerator,
			const env::NonPermanentService& service
		):	_warning(NULL)
			, _line(static_cast<const Line*>(service.getPath()))
			, _calendar(service)
		{
			_calendar &= timetablegenerator.getBaseCalendar();
			const TimetableGenerator::Rows& rows(timetablegenerator.getRows());
			const Path::Edges& edges(service.getPath()->getEdges());
			Path::Edges::const_iterator itEdge(edges.begin());

			
			for(TimetableGenerator::Rows::const_iterator itRow(rows.begin()); itRow != rows.end(); ++itRow)
			{
				Path::Edges::const_iterator itEdge2;
				for (itEdge2 = itEdge; itEdge2 != edges.end(); ++itEdge2)
				{
					if(	dynamic_cast<const PublicTransportStopZoneConnectionPlace*>((*itEdge2)->getFromVertex()->getHub())->getKey() == itRow->getPlace()->getKey()
						&&(	(*itEdge2)->isDeparture() == itRow->getIsDeparture()
							|| (*itEdge2)->isArrival() == itRow->getIsArrival()
						)
					){
						_content.push_back(
							make_pair(
								dynamic_cast<const PhysicalStop*>((*itEdge2)->getFromVertex()),
								((*itEdge2)->isDeparture() && itRow->getIsDeparture()) ?
									service.getDepartureBeginScheduleToIndex(false, itEdge2 - edges.begin()) :
									service.getArrivalBeginScheduleToIndex(false, itEdge2 - edges.begin())
						)	);
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
					_content.push_back(make_pair<const PhysicalStop*, Schedule>(NULL, Schedule()));
				}
			}
		}



		int TimetableColumn::operator<=( const TimetableColumn& other) const
		{
			assert(_content.size() == other._content.size());

			size_t rowsNumber(_content.size());
			size_t i;
			size_t j;

			// Tentative par ligne commune
			for (i=0; i<rowsNumber; ++i)
			{
				if (!_content[i].second.getHour().isUnknown() && !other._content[i].second.getHour().isUnknown())
				{
					if (_content[i].second < other._content[i].second)
						return true;
					if (other._content[i].second < _content[i].second)
						return false;
				}
			}

			// Tentative par succession 1
			for (i=0; i< rowsNumber; ++i)
			{
				if (!_content[i].second.getHour().isUnknown())
				{
					for(j = i+1; j< rowsNumber; ++j)
					{
						if(	!other._content[j].second.getHour().isUnknown() &&
							other._content[j].second < _content[i].second
						){
							return false;
						}
						
						if (i != 0)
						{
							for(size_t k = i-1; k>0; --k)
							{
								if(	!other._content[k].second.getHour().isUnknown() &&
									_content[i].second < other._content[k].second
								)	return true;
							}
						}
					}
				}
			}

			// Premiere heure
			for (i=0; i< rowsNumber; ++i)
				if (!_content[i].second.getHour().isUnknown())
					break;
			for (j=0; j< rowsNumber; ++j)
				if (!other._content[j].second.getHour().isUnknown())
					break;
			return _content[i].second < other._content[j].second;
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
			assert(op._content.size() == _content.size());

			return _line->getCommercialLine() == op._line->getCommercialLine()
				&& _content == op._content;
		}



		void TimetableColumn::setWarning(const TimetableWarning* it )
		{
			_warning = it;
		}



		const Calendar& TimetableColumn::getCalendar() const
		{
			return _calendar;
		}



		const TimetableColumn::Content& TimetableColumn::getContent() const
		{
			return _content;
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



		const TimetableWarning* TimetableColumn::getWarning() const
		{
			return _warning;
		}
	}
}
