
/** Crossing class implementation.
	@file Crossing.cpp

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

#include "Crossing.h"
#include "RoadModule.h"
#include "Address.h"
#include "AddressablePlace.h"
#include "Road.h"
#include "Registry.h"
#include "VertexAccessMap.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<road::Crossing>::KEY("Crossing");
	}

	namespace road
	{
		Crossing::Crossing(
			util::RegistryKeyType key
		):	Hub(),
			Registrable(key)
		{
		}
		
		
		
		Crossing::~Crossing ()
		{
		
		}

		bool Crossing::isConnectionAllowed(
			const Vertex& fromVertex,
			const Vertex& toVertex
		) const	{
			return true;
		}


		const geometry::Point2D& Crossing::getPoint() const
		{
			return *_address;
		}


		int Crossing::getMinTransferDelay() const
		{
			return 0;
		}



		void Crossing::setAddress(Address* address)
		{
			_address = address;
		}
		
		
		
		Address* Crossing::getAddress() const
		{
			return _address;
		}

		bool Crossing::containsAnyVertex( graph::GraphIdType graphType ) const
		{
			return graphType == RoadModule::GRAPH_ID;
		}



		void Crossing::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessDirection& accessDirection,
			graph::GraphIdType whatToSearch,
			const graph::Vertex& origin
		) const	{
			if(whatToSearch == RoadModule::GRAPH_ID)
			{
				result.insert(
					_address,
					VertexAccess()
				);
			}
		}



		synthese::graph::HubScore Crossing::getScore() const
		{
			return 1;
		}



		time::MinutesDuration Crossing::getTransferDelay( const graph::Vertex& fromVertex , const graph::Vertex& toVertex ) const
		{
			return 0;
		}
	}
}
