
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


#include "VertexAccessMap.h"

#include "Vertex.h"
#include "Edge.h"
#include "Line.h"
#include "ConnectionPlace.h"

#include <assert.h>
#include <set>
#include <limits>

using namespace std;

namespace synthese
{
	namespace env
	{


		VertexAccessMap::VertexAccessMap ()
			: _isobarycenterUpToDate (false)
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




		void 
		VertexAccessMap::insert (const Vertex* vertex, 
					 const VertexAccess& vertexAccess)
		{
			_map.insert (std::make_pair (vertex, vertexAccess));
			_isobarycenterUpToDate = false;
			_isobarycenterMaxSquareDistanceUpToDate = false;
			if (vertexAccess.approachTime < _minApproachTime)
			{
				_minApproachTime = vertexAccess.approachTime;
			}

			// Updating the paths which needs fine stepping set
			if (!vertex->isConnectionAllowed())
			{
				// Departure vertices
				for (set<const Edge*>::const_iterator itEdge(vertex->getDepartureEdges().begin());
					itEdge != vertex->getDepartureEdges().end (); ++itEdge)
				{
					_pathOnWhichFineSteppingForDeparture.insert((*itEdge)->getParentPath());
				}

				// Arrival vertices
				for (set<const Edge*>::const_iterator itEdge(vertex->getArrivalEdges().begin());
					itEdge != vertex->getArrivalEdges().end (); ++itEdge)
				{
					_pathOnWhichFineSteppingForArrival.insert((*itEdge)->getParentPath());
				}
			}

		}





		const Point& 
		VertexAccessMap::getIsobarycenter () const
		{
			if (_isobarycenterUpToDate == false)
			{
				double sumx (0.0);
				double sumy (0.0);
				double points(0);
				
				for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
					 it != _map.end (); ++it)
				{
					if (it->first->getX() > 0 && it->first->getY() > 0)
					{
						sumx += it->first->getX();
						sumy += it->first->getY();
						++points;
					}
				}
				
				if (points)
				{
					_isobarycenter.setX (sumx / points);
					_isobarycenter.setY (sumy / points);
				}
				else
				{
					_isobarycenter.setX(UNKNOWN_VALUE);
					_isobarycenter.setY(UNKNOWN_VALUE);
				}
				
				_isobarycenterUpToDate = true;
			}
			return _isobarycenter;
		    
		}


		const SquareDistance&
		VertexAccessMap::getIsobarycenterMaxSquareDistance () const
		{
			if (_isobarycenterMaxSquareDistanceUpToDate == false)
			{
				_isobarycenterMaxSquareDistance.setSquareDistance (0);
				if (!_isobarycenter.isUnknown())
				{
					for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
						it != _map.end (); ++it)
					{
						if (it->first->getX() > 0 && it->first->getY() > 0)
						{
							SquareDistance sqd (*(it->first), _isobarycenter);
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




		const std::map<const Vertex*, VertexAccess>& 
		VertexAccessMap::getMap () const
		{
			return _map;
		}



		void 
		VertexAccessMap::merge (const VertexAccessMap& vam,
					MergeAddresses mergeAddresses,
					MergePhysicalStops mergePhysicalStops)
		{
			for (std::map<const Vertex*, VertexAccess>::const_iterator itps = vam.getMap ().begin ();
			 itps != vam.getMap ().end (); ++itps)
			{
			if ( (mergeAddresses == MERGE_ADDRESSES) && 
				 (itps->first->isAddress () == true) )
			{
				insert (itps->first, itps->second);
			}
			if ( (mergePhysicalStops == MERGE_PHYSICALSTOPS) && 
				 (itps->first->isAddress () == false) )
			{
				insert (itps->first, itps->second);
			}
			}

		}




	}
}

