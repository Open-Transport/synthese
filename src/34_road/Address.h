
/** Address class header.
	@file Address.h

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

#ifndef SYNTHESE_ENV_ADDRESS_H
#define SYNTHESE_ENV_ADDRESS_H

#include "01_util/Constants.h"
#include "GeoPoint.h"

#include <vector>
#include <set>

namespace geos
{
	namespace geom
	{
		class Coordinate;
	}
}

namespace synthese
{
	namespace road
	{
		class RoadChunk;
		class Road;

		//////////////////////////////////////////////////////////////////////////
		/// Address.
		/// @ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// An address corresponds to a point on a side of a street.
		/// @image html uml_address.png
		///
		/// The address is defined by a RoadChunk and a metric offset.
		class Address:
			public geography::GeoPoint
		{
		public:


		private:
			RoadChunk* _roadChunk;
			double	_metricOffset;


		public:

			//////////////////////////////////////////////////////////////////////////
			/// Direct constructor.
			/// @param roadChunk road chunk where the address points to
			/// @param metricOffset offset after the beginning of the chunk (and not the road)
			Address(
				RoadChunk* roadChunk = NULL,
				double metricOffset = UNKNOWN_VALUE
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor by road and offset
			/// @param road road
			/// @param metricOffset offset after the beginning of the road
			/// @param coordinate coordinate of the address
			Address(
				const Road* road,
				double metricOffset,
				const geos::geom::Coordinate& coordinate
			);

			~Address();


			//! @name Getters
			//@{
				RoadChunk* getRoadChunk() const { return _roadChunk; }
				double getMetricOffset() const { return _metricOffset; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Query methods
			//@{
			//@}
		};
	}
}

#endif
