
/** RoadChunk class header.
	@file RoadChunk.h

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

#ifndef SYNTHESE_ENV_ROADCHUNK_H
#define SYNTHESE_ENV_ROADCHUNK_H

#include "Edge.h"

#include <utility>

namespace geos
{
	namespace geom
	{
		class Point;
}	}

namespace synthese
{
	namespace road
	{
		class Road;
		class Crossing;

		//////////////////////////////////////////////////////////////////////////
		/// Association class between road and crossings.
		///	@ingroup m35
		/// @author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		///	A road chunk is always delimited by two adresses, with no other
		///	address in between.
		///	These delimiting addresses can correspond to :
		///		- a crossing address
		///		- a physical stop address
		///		- a public place address
		class RoadChunk:
			public graph::Edge
		{
		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id identifier (default 0)
			/// @param fromCrossing crossing where the chunk begins (default NULL)
			/// @param rankInRoad rank of the chunk in the road path (default unknown)
			/// @param road road which the chunk belongs to (default NULL)
			/// @param metricOffset distance between the the chunk beginning and the road beginning (default unknown)
			RoadChunk(
				util::RegistryKeyType id = 0,
				Crossing* fromCrossing = NULL,
				int rankInRoad = UNKNOWN_VALUE,
				Road* road = NULL,
				double metricOffset = UNKNOWN_VALUE
			);

			virtual ~RoadChunk ();

		public:
			//! @name Getters
			//@{
				Crossing* getFromCrossing() const;
			//@}



			//! @name Setters
			//@{
				void setFromCrossing(Crossing* value);
				void setRoad(Road* road);
			//@}



			//! @name Query methods.
			//@{
				virtual bool isDepartureAllowed() const;
				virtual bool isArrivalAllowed() const;

				Road* getRoad() const;

				//////////////////////////////////////////////////////////////////////////
				/// Extracts the point corresponding to an offset from the beginning of the chunk.
				/// If the offset is greater than the length of the chunk, then the method
				///	returns the last point of the chunk
				/// @param metricOffset
				/// @return the point corresponding to an offset from the beginning of the chunk.
				boost::shared_ptr<geos::geom::Point> getPointFromOffset(
					MetricOffset metricOffset
				) const;
			//@}
		};
}	}

#endif
