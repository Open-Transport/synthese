
/** StandardArrivalDepartureTableGenerator class implementation.
	@file StandardArrivalDepartureTableGenerator.cpp

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

#include "15_env/Edge.h"
#include "15_env/LineStop.h"
#include "15_env/PhysicalStop.h"

#include "34_departures_table/StandardArrivalDepartureTableGenerator.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace departurestable
	{

		StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
			const DeparturesTableModule::PhysicalStopsList& physicalStops
			, const DeparturesTableModule::Direction& direction
			, const DeparturesTableModule::EndFilter& endfilter
			, const DeparturesTableModule::LineFilter& lineFilter
			, const DeparturesTableModule::DisplayedPlacesList& displayedPlacesList
			, const DeparturesTableModule::ForbiddenPlacesList& forbiddenPlaces
			, const DateTime& startTime
			, const DateTime& endDateTime
			, size_t maxSize
		) : ArrivalDepartureTableGenerator(physicalStops, direction, endfilter, lineFilter
										, displayedPlacesList, forbiddenPlaces, startTime, endDateTime, maxSize)
		{}

		const DeparturesTableModule::ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
		{
			for (DeparturesTableModule::PhysicalStopsList::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
			{
				for (set<const Edge*>::const_iterator eit = (*it)->getDepartureEdges().begin(); eit != (*it)->getDepartureEdges().end(); ++eit)
				{
					const LineStop* ls = (const LineStop*) (*eit);

					if (!_allowedLineStop(ls))
						continue;

					// Loop on services
					DateTime departureDateTime = _startDateTime;
					int serviceNumber = -2;
					while ((serviceNumber = ls->getNextService(departureDateTime, _endDateTime, _startDateTime, ++serviceNumber)) != UNKNOWN_VALUE)
					{
						_insert(ls, serviceNumber, departureDateTime);
					}		
				}
			}
			return _result;

		}
	}
}
