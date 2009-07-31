
/** VertexAccessMap class implementation.
	@file VertexAccessMap.cpp

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


#define NOMINMAX
#include <limits>
#include "VertexAccessMap.h"

#include "Vertex.h"
#include "Edge.h"
#include "Hub.h"

#include <boost/foreach.hpp>
#include <assert.h>
#include <set>

using namespace std;

namespace synthese
{
	using namespace geometry;

	namespace graph
	{
		VertexAccessMap::VertexAccessMap ()
			: _isobarycentreToUpdate (false)
			, _isobarycenterMaxSquareDistanceUpToDate (false)
			, _minApproachTime (std::numeric_limits<int>::max ())
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
				_isobarycentreToUpdate = true;
				_isobarycenterMaxSquareDistanceUpToDate = false;

				// Updating the paths which needs fine stepping set
				if (!vertex->getHub()->isConnectionPossible())
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
				||	vertexAccess.approachTime == it->second.approachTime
					&&	vertexAccess.approachDistance < it->second.approachDistance
				)	it->second = vertexAccess;
			}

			if (vertexAccess.approachTime < _minApproachTime)
			{
				_minApproachTime = vertexAccess.approachTime;
			}
		}



		const IsoBarycentre& 
		VertexAccessMap::getIsobarycenter () const
		{
			if (_isobarycentreToUpdate)
			{
				_isobarycentre.clear();
				BOOST_FOREACH(VamMap::value_type it, _map)
				{
					_isobarycentre.add(*it.first);
				}
				_isobarycentreToUpdate = false;
			}
			return _isobarycentre;
		    
		}



		const SquareDistance& VertexAccessMap::getIsobarycenterMaxSquareDistance(
		) const	{
			if (_isobarycenterMaxSquareDistanceUpToDate == false)
			{
				_isobarycenterMaxSquareDistance.setSquareDistance (0);
				if (!_isobarycentre.isUnknown())
				{
					BOOST_FOREACH(const VamMap::value_type& it, _map)
					{
						if (it.first->getX() > 0 && it.first->getY() > 0)
						{
							SquareDistance sqd (*it.first, _isobarycentre);
							if (_isobarycenterMaxSquareDistance < sqd)
							{
								_isobarycenterMaxSquareDistance.setSquareDistance (sqd.getSquareDistance ());
							}
						}
					}
				}
				_isobarycenterMaxSquareDistanceUpToDate = true;
			}
			return _isobarycenterMaxSquareDistance;		    
		}



		int 
		VertexAccessMap::getMinApproachTime () const
		{
			return _minApproachTime;
		}



		const VertexAccessMap::VamMap& VertexAccessMap::getMap () const
		{
			return _map;
		}



		void VertexAccessMap::mergeWithFilter(
			const VertexAccessMap& vam,
			GraphIdType graphFilter
		){
			BOOST_FOREACH(const VamMap::value_type& itps, vam.getMap())
			{
				const Vertex* vertex(itps.first);

				if(vertex->getGraphType() == graphFilter)
				{
					insert(vertex, itps.second);
				}
			}
		}
	}
}
