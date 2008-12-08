
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
#include "ConnectionPlace.h"

namespace synthese
{
	using namespace geometry;

	namespace env
	{
		Vertex::Vertex (const AddressablePlace* place,
				double x,
				double y) 
			: Point2D (x, y) // By default geolocation is unknown.
			, _addressablePlace (place)
		{
		    
		}



		Vertex::~Vertex ()
		{
		}



		const ConnectionPlace* 
		Vertex::getConnectionPlace () const
		{
			return dynamic_cast<const ConnectionPlace*> (_addressablePlace);
		}



		const AddressablePlace* 
		Vertex::getPlace () const
		{
			return _addressablePlace;
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
			_departureEdges.insert (edge);    
		}



		void 
		Vertex::addArrivalEdge ( const Edge* edge )
		{
			_arrivalEdges.insert (edge);    
		}

		void Vertex::setPlace( const AddressablePlace* place )
		{
			_addressablePlace = place;
		}
	}
}
