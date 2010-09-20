
/** House class implementation.
	@file House.cpp

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

#include "House.hpp"
#include "Road.h"
#include "RoadPlace.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace,road::House>::FACTORY_KEY("House");
	}

	namespace road
	{
		House::House(
			RoadChunk& chunk,
			RoadChunk::HouseNumber houseNumber,
			bool numberAtBeginning,
			std::string separator
		):	Address(
				chunk,
				chunk.getMetricOffset() + chunk.getHouseNumberMetricOffset(houseNumber),
				houseNumber
			)
		{
			setName(string(
					(numberAtBeginning ? lexical_cast<string>(houseNumber) : chunk.getRoad()->getRoadPlace()->getName()) +
					separator +
					(numberAtBeginning ? chunk.getRoad()->getRoadPlace()->getName() : lexical_cast<string>(houseNumber))
			)	);
			setCity(chunk.getRoad()->getRoadPlace()->getCity());
		}



		House::House()
		{

		}


		boost::shared_ptr<geos::geom::Point> House::getPoint() const
		{
			return this->getGeometry();
		}



		void House::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const GraphTypes& whatToSearch
		) const	{
			return Address::getVertexAccessMap(result, accessParameters, whatToSearch);
		}



		std::string House::getNameForAllPlacesMatcher( std::string text /*= std::string() */ ) const
		{
			return text;
		}
	}
}
