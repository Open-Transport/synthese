
/** TrafficForecast class implementation.
	@file TrafficForecast.cpp

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

#include "TrafficForecast.hpp"

namespace synthese
{
	using namespace analysis;

	CLASS_DEFINITION(TrafficForecast, "t111_traffic_forecasts", 112)
	FIELD_DEFINITION_OF_OBJECT(TrafficForecast, "traffic_forecast_id", "traffic_forecasts_ids")

	FIELD_DEFINITION_OF_TYPE(Departure, "departure_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Arrival, "arrival_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Traffic, "traffic", SQL_INTEGER)

	namespace analysis
	{
		TrafficForecast::TrafficForecast(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<TrafficForecast, TrafficForecastSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Departure),
					FIELD_DEFAULT_CONSTRUCTOR(Arrival),
					FIELD_DEFAULT_CONSTRUCTOR(OpeningTime),
					FIELD_VALUE_CONSTRUCTOR(Traffic, 0)
			)	)
		{}



		void TrafficForecast::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void TrafficForecast::unlink()
		{

		}
}	}

