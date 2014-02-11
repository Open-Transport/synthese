
/** DRTArea class implementation.
	@file DRTArea.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DRTArea.hpp"
#include "PTModule.h"

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	CLASS_DEFINITION(DRTArea, "t071_drt_areas", 71)
	FIELD_DEFINITION_OF_OBJECT(DRTArea, "drt_area_id", "drt_area_ids")

	FIELD_DEFINITION_OF_TYPE(Stops, "stops", SQL_TEXT)

	namespace pt
	{
		DRTArea::DRTArea(
			const util::RegistryKeyType id,
			std::string name,
			Stops::Type stops
		):	Registrable(id),
			Object<DRTArea, DRTAreaSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(Name, name),
					FIELD_VALUE_CONSTRUCTOR(Stops, stops)
			)	),
			Vertex(NULL, boost::shared_ptr<geos::geom::Point>())
		{}



		graph::GraphIdType DRTArea::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		std::string DRTArea::getRuleUserName() const
		{
			return "Zone de TAD";
		}



		bool DRTArea::contains( const StopArea& stopArea ) const
		{
			return get<Stops>().find(&const_cast<StopArea&>(stopArea)) != get<Stops>().end();
		}
}	}
