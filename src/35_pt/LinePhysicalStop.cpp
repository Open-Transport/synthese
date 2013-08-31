
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
#include "StopPoint.hpp"
#include "JourneyPatternCopy.hpp"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace pt
	{
		LinePhysicalStop::LinePhysicalStop(
			util::RegistryKeyType id,
			JourneyPattern* line,
			std::size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			StopPoint* stop
		):	Registrable(id),
			LineStop(id, line, rankInPath, isDeparture, isArrival, metricOffset, stop)
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

			// Links from stop to the linestop
			if(getIsArrival())
			{
				stop.addArrivalEdge((Edge*) this);
			}
			if(getIsDeparture())
			{
				stop.addDepartureEdge((Edge*) this);
			}
		}



		StopPoint* LinePhysicalStop::getPhysicalStop() const
		{
			return static_cast<StopPoint*>(getFromVertex());
		}



		void LinePhysicalStop::clearPhysicalStopLinks()
		{
			StopPoint* stop(getPhysicalStop());
			if(stop == NULL)
			{
				return;
			}

			// Collecting all line stops to unlink including journey pattern copies
			typedef vector<pair<JourneyPattern*, LinePhysicalStop*> > ToClean;
			ToClean toClean;
			toClean.push_back(make_pair(getLine(), this));
			BOOST_FOREACH(JourneyPatternCopy* copy, getLine()->getSubLines())
			{
				toClean.push_back(
					make_pair(
						copy,
						const_cast<LinePhysicalStop*>(static_cast<const LinePhysicalStop*>(
							copy->getEdge(getRankInPath()))
						)
				)	);
			}

			BOOST_FOREACH(const ToClean::value_type& it, toClean)
			{
				// Removing edge from journey pattern
				it.first->removeEdge(*it.second);

				// Removing edge from stop point
				if(it.second->getIsArrival())
				{
					stop->removeArrivalEdge(it.second);
				}
				if(it.second->getIsDeparture())
				{
					stop->removeDepartureEdge(it.second);
				}

				if(it.second != this)
				{
					delete it.second;
				}
			}
		}
}	}
