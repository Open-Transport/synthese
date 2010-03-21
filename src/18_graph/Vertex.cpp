
/** Vertex class implementation.
	@file Vertex.cpp

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

#include "Vertex.h"
#include "Edge.h"

using namespace std;

namespace synthese
{
	using namespace geometry;

	namespace graph
	{
		Vertex::Vertex(
			const Hub* hub,
			double x,
			double y) 
		:	Point2D (x, y), // By default geolocation is unknown.
			_hub(hub)
		{
		}



		Vertex::~Vertex ()
		{
		}



		const Hub* Vertex::getHub(
		) const {
			return _hub;
		}


		
		const Vertex::Edges& 
		Vertex::getDepartureEdges () const
		{
			return _departureEdges;
		}



		const Vertex::Edges& 
		Vertex::getArrivalEdges () const
		{
			return _arrivalEdges;
		}

		 

		void 
		Vertex::addDepartureEdge ( const Edge* edge )
		{
			assert(edge);
			assert(edge->getParentPath());

			_departureEdges.insert(make_pair(edge->getParentPath(), edge));
		}



		void 
		Vertex::addArrivalEdge ( const Edge* edge )
		{
			assert(edge);
			assert(edge->getParentPath());

			_arrivalEdges.insert(make_pair(edge->getParentPath(), edge));    
		}



		void Vertex::setHub(
			const Hub* hub
		){
			_hub = hub;
		}



		void Vertex::removeArrivalEdge( const Edge* edge )
		{
			assert(edge);
			assert(edge->getParentPath());

			pair<Edges::iterator, Edges::iterator> range(_arrivalEdges.equal_range(edge->getParentPath()));
			assert(range.first != _arrivalEdges.end() && range.first->first == edge->getParentPath());

			for(Edges::iterator it(range.first); it!= range.second; ++it)
			{
				if(it->second == edge)
				{
					_arrivalEdges.erase(it);
				}
			}
		}



		void Vertex::removeDepartureEdge( const Edge* edge )
		{
			assert(edge);
			assert(edge->getParentPath());

			pair<Edges::iterator, Edges::iterator> range(_departureEdges.equal_range(edge->getParentPath()));
			assert(range.first != _departureEdges.end() && range.first->first == edge->getParentPath());

			for(Edges::iterator it(range.first); it!= range.second; ++it)
			{
				if(it->second == edge)
				{
					_departureEdges.erase(it);
				}
			}
		}
	}
}
