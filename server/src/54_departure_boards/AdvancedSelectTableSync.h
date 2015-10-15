
/** AdvancedSelectTableSync class header.
	@file 54_departure_boards/AdvancedSelectTableSync.h

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

#ifndef SYNTHESE_AdvancedSelectTableSync_H__
#define SYNTHESE_AdvancedSelectTableSync_H__

#include <vector>
#include <string>

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include "SecurityTypes.hpp"
#include "UtilConstants.h"
#include "UtilTypes.h"

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace pt
	{
		class StopPoint;
		class CommercialLine;
	}

	namespace geography
	{
		class NamedPlace;
	}

	namespace departure_boards
	{
		class BroadcastPoint;

		struct ConnectionPlaceWithBroadcastPoint
		{
			boost::shared_ptr<geography::NamedPlace> place;
			std::string	cityName;
			int broadCastPointsNumber;
			int cpuNumber;
		};

		typedef enum {
			AT_LEAST_ONE_BROADCASTPOINT,
			NO_BROADCASTPOINT,
			WITH_OR_WITHOUT_ANY_BROADCASTPOINT
		} BroadcastPointsPresence;

		/** Connection place searcher.
			@param broadCastPointsNumber Filter on broadcast points number : 0 = no broadcast points, 1 = at least one broadcast point
			@result vector of the founded searched connection places from the live data objects (do not delete the objects after use).
		*/
		std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchConnectionPlacesWithBroadcastPoints(
			util::Env& env,
			const security::RightsOfSameClassMap& rights
			, bool totalControl = true
			, security::RightLevel neededLevel = security::FORBIDDEN
			, std::string cityName = std::string()
			, std::string placeName = std::string()
			, BroadcastPointsPresence bpPresence = WITH_OR_WITHOUT_ANY_BROADCASTPOINT
			, boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>()
			, boost::optional<std::size_t> number = boost::optional<std::size_t>()
			, int first = 0
			, bool orderByCity = true
			, bool orderByName = false
			, bool orderByNumber = false
			, bool raisingOrder = true
		);

		typedef struct { boost::shared_ptr<const pt::StopPoint> stop; boost::shared_ptr<BroadcastPoint> bp; } PhysicalStopAndBroadcastPoint;


		/** JourneyPattern searcher.
			@result found physical stops from the live data objects with the corresponding broadcast point if exists (NULL else) The broadcast points are temporary object and must be deleted after use.
		*/
		std::vector<boost::shared_ptr<const pt::CommercialLine> > getCommercialLineWithBroadcastPoints(
			util::Env& env,
			boost::optional<std::size_t> number = boost::optional<std::size_t>(),
			int first = 0
		);
	}
}

#endif // SYNTHESE_AdvancedSelectTableSync_H__
