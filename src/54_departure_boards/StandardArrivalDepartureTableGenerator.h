
/** StandardArrivalDepartureTableGenerator class header.
	@file StandardArrivalDepartureTableGenerator.h

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

#ifndef SYNTHESE_StandardArrivalDepartureTableGenerator_H
#define SYNTHESE_StandardArrivalDepartureTableGenerator_H

#include "ArrivalDepartureTableGenerator.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace departure_boards
	{
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m54
		class StandardArrivalDepartureTableGenerator:
			public ArrivalDepartureTableGenerator
		{
		private:
			bool _excludeStartingAreaIsDestination;

		public:
			StandardArrivalDepartureTableGenerator(
				const PhysicalStops&,
				const DeparturesTableDirection&,
				const EndFilter&,
				const LineFilter&,
				const DisplayedPlacesList&,
				const ForbiddenPlacesList&,
				const boost::posix_time::ptime& startDateTime,
				const boost::posix_time::ptime& endDateTime,
				bool allowCanceled,
				boost::optional<std::size_t> maxSize = boost::optional<std::size_t>(),
				bool endDateTimeConcernsTheorical = false,
				bool excludeStartingAreaIsDestination = false
			);

			const ArrivalDepartureList& generate();
		};

	}
}

#endif
