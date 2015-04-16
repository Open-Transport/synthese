
/** ForcedDestinationsArrivalDepartureTableGenerator class header.
	@file ForcedDestinationsArrivalDepartureTableGenerator.h

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

#ifndef SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H
#define SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H

#include "ArrivalDepartureTableGenerator.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		//////////////////////////////////////////////////////////////////////////
		/// Departure table generator : chronological order with priority to
		///	display at least one departure per available destination.
		/// @ingroup m54
		/// @author Hugues Romain
		class ForcedDestinationsArrivalDepartureTableGenerator
		:	public ArrivalDepartureTableGenerator
		{
		public:
			typedef std::map<boost::optional<util::RegistryKeyType>, const pt::StopArea*> ForcedDestinationsSet;

		private:

			ForcedDestinationsSet	_forcedDestinations;	//!< Destinations à afficher absolument
			const boost::posix_time::time_duration				_persistanceDuration;	//!< Durée pendant laquelle une destination est forcée


		public:
			ForcedDestinationsArrivalDepartureTableGenerator(
				const PhysicalStops&,
				const DeparturesTableDirection&,
				const EndFilter&,
				const LineFilter&,
				const DisplayedPlacesList&,
				const ForbiddenPlacesList&,
				const boost::posix_time::ptime& startDateTime,
				const boost::posix_time::ptime& endDateTime,
				size_t maxSize,
				const ForcedDestinationsSet&,
				boost::posix_time::time_duration persistanceDuration,
				bool allowCanceled
			);


			const ArrivalDepartureList& generate();
		};

	}
}

#endif
