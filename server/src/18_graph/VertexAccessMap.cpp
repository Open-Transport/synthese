
/** VertexAccessMap class implementation.
	@file VertexAccessMap.cpp

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


#include "VertexAccessMap.h"

#include "Vertex.h"
#include "Edge.h"
#include "Hub.h"

#include <geos/geom/Point.h>
#include <geos/geom/Envelope.h>
#include <boost/foreach.hpp>
#include <assert.h>
#include <set>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	namespace graph
	{
		VertexAccessMap::VertexAccessMap ()
		{
		}


		VertexAccessMap::~VertexAccessMap ()
		{
		}



		const VertexAccess&
		VertexAccessMap::getVertexAccess (const Vertex* vertex) const
		{
			assert (contains (vertex));
			return _map.find (vertex)->second;
		}



		bool
		VertexAccessMap::needFineSteppingForDeparture (const Path* path) const
		{
			return _pathOnWhichFineSteppingForDeparture.find(path) != _pathOnWhichFineSteppingForDeparture.end();
		}



		bool
		VertexAccessMap::needFineSteppingForArrival (const Path* path) const
		{
			return _pathOnWhichFineSteppingForArrival.find(path) != _pathOnWhichFineSteppingForArrival.end();
		}



		bool
		VertexAccessMap::contains (const Vertex* vertex) const
		{
			return (_map.find (vertex) != _map.end ());
		}



		void VertexAccessMap::insert(
			const Vertex* vertex,
			const VertexAccess& vertexAccess
		){
			VamMap::iterator it(_map.find(vertex));

			if (it == _map.end())
			{
				// Insertion of a new vertex
				_map.insert (std::make_pair (vertex, vertexAccess));
				_isobarycentre.reset();

				// Updating the paths which needs fine stepping set
				if (!vertex->getHub()->isUsefulTransfer(vertex->getGraphType()))
				{
					// Departure vertices
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, vertex->getDepartureEdges())
					{
						_pathOnWhichFineSteppingForDeparture.insert(edge.first);
					}

					// Arrival vertices
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, vertex->getArrivalEdges())
					{
						_pathOnWhichFineSteppingForArrival.insert(edge.first);
					}
				}
			}
			else
			{	// Update of the access conditions if more efficient
				if(	vertexAccess.approachTime < it->second.approachTime
				||	(vertexAccess.approachTime == it->second.approachTime
					&&	vertexAccess.approachDistance < it->second.approachDistance)
				)	it->second = vertexAccess;
			}

			if (_minApproachTime.is_not_a_date_time() || vertexAccess.approachTime < _minApproachTime)
			{
				_minApproachTime = vertexAccess.approachTime;
			}
		}



		const boost::shared_ptr<Point>& VertexAccessMap::getCentroid(
		) const	{
			if(!_isobarycentre.get())
			{
				Envelope e;
				BOOST_FOREACH(VamMap::value_type it, _map)
				{
					if(it.first->hasGeometry())
					{
						e.expandToInclude(*it.first->getGeometry()->getCoordinate());
					}
				}
				if(!e.isNull())
				{
					Coordinate c;
					e.centre(c);
					_isobarycentre.reset(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(c));
				}
			}

			return _isobarycentre;
		}



		posix_time::time_duration VertexAccessMap::getMinApproachTime() const
		{
			return _minApproachTime;
		}



		const VertexAccessMap::VamMap& VertexAccessMap::getMap () const
		{
			return _map;
		}



		bool VertexAccessMap::intersercts( const VertexAccessMap& other ) const
		{
			// Control if the departure place and the arrival place have a common point
			BOOST_FOREACH(const VamMap::value_type& it, _map)
			{
				if(other._map.find(it.first) != other._map.end()) return true;
			}
			return false;
		}



		Journey VertexAccessMap::getBestIntersection( const VertexAccessMap& other ) const
		{
			Journey result;
			time_duration bestDuration(not_a_date_time);

			BOOST_FOREACH(const VamMap::value_type& it, _map)
			{
				VamMap::const_iterator it2(other._map.find(it.first));
				if(it2 != other._map.end())
				{
					if(	bestDuration.is_not_a_date_time() ||
						it.second.approachTime + it2->second.approachTime < bestDuration
					){
						Journey secondJourney(it2->second.approachJourney);
						if(!secondJourney.empty())
						{
							if(!it.second.approachJourney.empty())
							{
								secondJourney.shift(
									it.second.approachJourney.getFirstArrivalTime() - secondJourney.getFirstDepartureTime()
								);
							}
						}
						result = it.second.approachJourney;
						result.append(secondJourney);
					}
				}
			}
			return result;
		}

		std::ostream& operator<< (std::ostream& os, const VertexAccessMap& vam)
		{
			for (VertexAccessMap::VamMap::const_iterator it = vam.getMap().begin(); it != vam.getMap().end(); ++it)
		    {
      			os << "Vertex " << it->first->getKey() << " : " << 
      			"approachTime = " << it->second.approachTime << ", " << 
      			"approachDistance = " <<  it->second.approachDistance << std::endl;
    		}	
    		return os;		
		}

	}
}
