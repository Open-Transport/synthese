
/** RollingStock class implementation.
	@file RollingStock.cpp

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

#include "RollingStock.hpp"
#include "Registry.h"
#include "ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace util;
	using namespace vehicle;

	CLASS_DEFINITION(RollingStock, "t049_rolling_stock", 49)
	FIELD_DEFINITION_OF_TYPE(Article, "article", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(IndicatorLabel, "indicator_label", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TridentKey, "trident_key", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(IsTridentReference, "is_trident_reference", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(CO2Emissions, "CO2_emissions", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(EnergyConsumption, "energy_consumption", SQL_DOUBLE)

	namespace vehicle
	{
		const unsigned int RollingStock::CO2_EMISSIONS_DISTANCE_UNIT_IN_METERS = 1000; // 1km
		const unsigned int RollingStock::ENERGY_CONSUMPTION_DISTANCE_UNIT_IN_METERS = 100000;  // 100km

		RollingStock::RollingStock(
			util::RegistryKeyType key
		):	util::Registrable(key),
			Object<RollingStock, RollingStockSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Article),
					FIELD_DEFAULT_CONSTRUCTOR(IndicatorLabel),
					FIELD_DEFAULT_CONSTRUCTOR(TridentKey),
					FIELD_VALUE_CONSTRUCTOR(IsTridentReference, false),
					FIELD_VALUE_CONSTRUCTOR(CO2Emissions, 0),
					FIELD_VALUE_CONSTRUCTOR(EnergyConsumption, 0),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks)
			)	),
			graph::PathClass()
		{}



		RollingStock::~RollingStock()
		{}



		const string RollingStock::getGTFSKey() const
		{
			/* GTFS rolling stocks are:
			 *
			 * 0 - Tram, Streetcar, Light rail. Any light rail or street level system within a metropolitan area.
			 * 1 - Subway, Metro. Any underground rail system within a metropolitan area.
			 * 2 - Rail. Used for intercity or long-distance travel.
			 * 3 - Bus. Used for short- and long-distance bus routes.
			 * 4 - Ferry. Used for short- and long-distance boat service.
			 * 5 - Cable car. Used for street-level cable cars where the cable runs beneath the car.
			 * 6 - Gondola, Suspended cable car. Typically used for aerial cable cars where the car is suspended from the cable.
			 * 7 - Funicular. Any rail system designed for steep inclines.
			 *
			 */

			string gtfsRollStock = "3"; // default is BUS

			if(get<TridentKey>() == "Metro")
			{
				gtfsRollStock = "1";
			}
			else if(get<TridentKey>() == "Tramway")
			{
				gtfsRollStock = "0";
			}

			return gtfsRollStock;
		}

		PathClass::Identifier RollingStock::getIdentifier() const
		{
			return getKey();
		}
}	}
