
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

#include "15_env/RoadChunk.h"
#include "15_env/Road.h"
#include "15_env/Address.h"

namespace synthese
{
	namespace env
	{
		RoadChunk::RoadChunk (
			uid id
			, Address* fromAddress
			, int rankInRoad
			, bool isDeparture
			, bool isArrival
		)	: synthese::util::Registrable<uid,RoadChunk> (id)
			, Edge (
				isDeparture
				, isArrival
				, (fromAddress == NULL) ? NULL : fromAddress->getRoad ()
				, rankInRoad
			), _fromAddress (fromAddress)
		{
		}

		  
		RoadChunk::~RoadChunk ()
		{
		}

		    




		const Vertex* 
		RoadChunk::getFromVertex () const
		{
			return _fromAddress;
		}




		double
		RoadChunk::getMetricOffset () const
		{
			return _fromAddress->getMetricOffset ();
		}

		void RoadChunk::setFromAddress(Address* fromAddress )
		{
			_fromAddress = fromAddress;

			// Links from stop to the linestop
			if (isArrival())
				_fromAddress->addArrivalEdge(static_cast<Edge*>(this));
			if (isDeparture())
				_fromAddress->addDepartureEdge(static_cast<Edge*>(this));

			markServiceIndexUpdateNeeded();
		}




	}
}
