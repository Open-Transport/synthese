
/** RoadChunk class implementation.
	@file RoadChunk.cpp

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

#include "RoadChunk.h"
#include "Road.h"
#include "Address.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace graph;
	
	namespace util
	{
		template<> const string Registry<road::RoadChunk>::KEY("RoadChunk");
	}

	namespace road
	{
		RoadChunk::RoadChunk(
			util::RegistryKeyType id,
			Address* fromAddress,
			int rankInRoad,
			Road* street
		):	util::Registrable(id),
			Edge(
				street,
				rankInRoad,
				fromAddress
			)
		{
			if(fromAddress) setFromAddress(fromAddress);
		}

		
		
		RoadChunk::~RoadChunk ()
		{
		}



		void RoadChunk::setRoad(const Road* road)
		{
			_parentPath = static_cast<const Path*>(road);
		}
		
		

		bool RoadChunk::isDepartureAllowed() const
		{
			return true;
		}
		
		
		
		bool RoadChunk::isArrivalAllowed() const
		{
			return true;
		}
		


		Address* RoadChunk::getFromAddress() const
		{
			return static_cast<Address*>(_fromVertex);
		}



		void RoadChunk::setFromAddress(Address* fromAddress )
		{
			_fromVertex = static_cast<Vertex*>(fromAddress);

			// Links from stop to the linestop
			fromAddress->addArrivalEdge(static_cast<Edge*>(this));
			fromAddress->addDepartureEdge(static_cast<Edge*>(this));

			markServiceIndexUpdateNeeded(false);
		}



		const Road* RoadChunk::getRoad() const
		{
			return static_cast<const Road*>(_parentPath);
		}
	}
}
