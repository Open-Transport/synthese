
/** ForcedDestinationsArrivalDepartureTableGenerator class header.
	@file ForcedDestinationsArrivalDepartureTableGenerator.h

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

#ifndef SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H
#define SYNTHESE_ForcedDestinationsArrivalDepartureTableGenerator_H

#include "ArrivalDepartureTableGenerator.h"
#include "UId.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{

		class ForcedDestinationsArrivalDepartureTableGenerator
		:	public ArrivalDepartureTableGenerator
		{
		public:
			typedef std::map<uid, const env::PublicTransportStopZoneConnectionPlace*> ForcedDestinationsSet;

		private:

			ForcedDestinationsSet	_forcedDestinations;	//!< Destinations à afficher absolument
			const int				_persistanceDuration;	//!< Durée pendant laquelle une destination est forcée


		public:
			ForcedDestinationsArrivalDepartureTableGenerator(
				const PhysicalStops&
				, const DeparturesTableDirection&
				, const EndFilter&
				, const LineFilter&
				, const DisplayedPlacesList&
				, const ForbiddenPlacesList&,
				const TransferDestinationsList&
				, const time::DateTime& startDateTime
				, const time::DateTime& endDateTime
				, size_t maxSize
				, const ForcedDestinationsSet&
				, int persistanceDuration
				);


			const ArrivalDepartureList& generate();
		};

	}
}

#endif
