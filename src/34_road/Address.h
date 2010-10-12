
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

#include "UtilConstants.h"
#include "RoadChunk.h"
#include "Place.h"

#include <vector>
#include <set>
#include <boost/optional.hpp>
#include <geos/geom/Point.h>

namespace synthese
{
	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// Point on a side of a street.
		/// @ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// The address is defined by a RoadChunk and a metric offset.
		/// @image html uml_address.png
		///
		/// It can correspond to a registered number on the street, or not
		/// (optional _houseNumber attribute)
		class Address:
			public WithGeometry<geos::geom::Point>
		{
		public:


		private:
			RoadChunk* _roadChunk;
			double	_metricOffset;
			boost::optional<RoadChunk::HouseNumber> _houseNumber;


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Empty constructor.
			Address();



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from values.
			/// @param roadChunk road chunk where the address points to
			/// @param metricOffset offset after the beginning of the chunk (and not the road)
			/// @param houseNumber registered number of the point in the street
			Address(
				RoadChunk& roadChunk,
				RoadChunk::MetricOffset metricOffset,
				boost::optional<RoadChunk::HouseNumber> houseNumber = boost::optional<RoadChunk::HouseNumber>()
			);



			~Address();


			//! @name Getters
			//@{
				RoadChunk* getRoadChunk() const { return _roadChunk; }
				double getMetricOffset() const { return _metricOffset; }
				boost::optional<RoadChunk::HouseNumber> getHouseNumber() const { return _houseNumber; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the vertices directly reachable from the house.
				///	Two vertices are returned if the graph type is road : 
				///	<ul>
				///	<li>the beginning of the two chunk linked with the house</li>
				///	<li>the beginning ot the revered chunk from the preceding one</li>
				///	</ul>
				///	Approach durations are computed from the accessParameters.
				//////////////////////////////////////////////////////////////////////////
				/// @retval result the two vertices
				/// @param accessParameters parameters of access
				/// @param whatToSearch graph of the returned vertices
				void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;

			//@}
		};
	}
}

#endif
