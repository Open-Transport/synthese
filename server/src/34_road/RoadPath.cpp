
/** RoadPath class implementation.
	@file RoadPath.cpp

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

#include "RoadPath.hpp"

#include "Edge.h"
#include "PermanentService.h"
#include "Road.h"
#include "RoadPlace.h"

using namespace std;

namespace synthese
{
	using namespace graph;

	namespace road
	{
		RoadPath::RoadPath(
			Road& road
		):	_road(&road)
		{
			// Creation of the permanent service
			PermanentService* service(new PermanentService(0, this));
			addService(*service, false);
			service->setPath(this);
		}



		RoadPath::~RoadPath()
		{
			BOOST_FOREACH(Edges::value_type edge, getEdges())
			{
				edge->setParentPath(NULL);
			}

			BOOST_FOREACH(ServiceSet::value_type service, getAllServices())
			{
				delete service;
			}
		}



/*		void Road::_insertRoadChunk(
			RoadChunkEdge& chunk, double length, std::size_t rankShift
		){
			assert(!_edges.empty());
			assert(chunk.getMetricOffset() <= (*_edges.begin())->getMetricOffset() + length);
			assert(chunk.getMetricOffset() <= (*_edges.begin())->getRankInPath() + rankShift);

			// Shifting the whole road to right
			for(Edges::const_iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				(*it)->setRankInPath((*it)->getRankInPath() + rankShift);
				(*it)->setMetricOffset((*it)->getMetricOffset() + length);
			}

			// Insertion of the chunk
			addEdge(chunk);
		}
*/


		RoadPlace* RoadPath::getRoadPlace(
		) const {
			return static_cast<RoadPlace*>(_pathGroup);
		}




		bool RoadPath::isPedestrianMode() const
		{
			return true;
		}



		bool RoadPath::isRoad() const
		{
			return true;
		}



		std::string RoadPath::getRuleUserName() const
		{
			if(!_road) return string();
			return "Route " + _road->getAnyRoadPlace()->getFullName();
		}



		bool RoadPath::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		void RoadPath::_setRoadPlace( RoadPlace& value )
		{
			// Break of existing link if exists
			if(_pathGroup)
			{
				value.removeRoad(*this);
			}

			// New links
			_pathGroup = &value;
			value.addRoad(*this);
		}
}	}

