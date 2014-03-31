
/** LinePhysicalStop class implementation.
	@file LinePhysicalStop.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "LinePhysicalStop.hpp"

#include "DRTArea.hpp"
#include "Hub.h"
#include "LineStop.h"
#include "PTModule.h"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace pt
	{
		LinePhysicalStop::LinePhysicalStop(
			JourneyPattern* line,
			std::size_t rankInPath,
			double metricOffset,
			StopPoint* stop,
			LineStop* lineStop
		):	Registrable(0),
			Edge(line, rankInPath, stop, metricOffset),
			_lineStop(lineStop)
		{
			if(stop)
			{
				setPhysicalStop(*stop);
			}
		}



		void LinePhysicalStop::setPhysicalStop(StopPoint& stop )
		{
			// Saving of the attribute
			setFromVertex(&stop);
		}



		StopPoint* LinePhysicalStop::getPhysicalStop() const
		{
			return static_cast<StopPoint*>(getFromVertex());
		}



		LinePhysicalStop::~LinePhysicalStop()
		{
		}



		bool LinePhysicalStop::getReservationNeeded() const
		{
			return
				_lineStop->get<ReservationNeeded>() ||
				dynamic_cast<DRTArea*>(&*_lineStop->get<LineNode>())
			;
		}



		void LinePhysicalStop::link()
		{
			// Link on stop
			if(_fromVertex)
			{
				if(isArrivalAllowed())
				{
					_fromVertex->addArrivalEdge(this);
				}
				if(isDepartureAllowed())
				{
					_fromVertex->addDepartureEdge(this);
				}

				if(_fromVertex->getHub())
				{
					_fromVertex->getHub()->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
				}
			}
			
			// Link on path
			if(_parentPath)
			{
				_parentPath->addEdge(*this);
			}
		}



		void LinePhysicalStop::unlink()
		{
			if(_parentPath)
			{
				_parentPath->removeEdge(*this);
			}

			if(_fromVertex)
			{
				_fromVertex->removeArrivalEdge(this);
				_fromVertex->removeDepartureEdge(this);

				if(_fromVertex->getHub())
				{
					_fromVertex->getHub()->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
				}
			}
		}



		bool LinePhysicalStop::getScheduleInput() const
		{
			return _lineStop->get<ScheduleInput>();
		}



		JourneyPattern* LinePhysicalStop::getJourneyPattern() const
		{
			return _lineStop->get<Line>() ? &*_lineStop->get<Line>() : NULL;
		}
}	}
