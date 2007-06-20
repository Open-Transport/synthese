
/** AdvancedSelectTableSync class header.
	@file AdvancedSelectTableSync.h

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

#ifndef SYNTHESE_AdvancedSelectTableSync_H__
#define SYNTHESE_AdvancedSelectTableSync_H__

#include <vector>
#include <string>

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>

#include "12_security/Types.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class PhysicalStop;
		class CommercialLine;
	}

	namespace departurestable
	{
		class BroadcastPoint;

		typedef struct {
			boost::shared_ptr<env::ConnectionPlace> place;
			std::string	cityName;
			int broadCastPointsNumber;
			} ConnectionPlaceWithBroadcastPoint;
		
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
			const security::RightsOfSameClassMap& rights 
			, bool totalControl = true
			, security::RightLevel neededLevel = security::FORBIDDEN
			, std::string cityName = std::string()
			, std::string placeName = std::string()
			, BroadcastPointsPresence bpPresence = WITH_OR_WITHOUT_ANY_BROADCASTPOINT
			, uid lineId = UNKNOWN_VALUE
			, int number = UNKNOWN_VALUE
			, int first = 0
			, bool orderByCity = true
			, bool orderByName = false
			, bool orderByNumber = false
			, bool raisingOrder = true
		);

		typedef struct { boost::shared_ptr<const env::PhysicalStop> stop; boost::shared_ptr<BroadcastPoint> bp; } PhysicalStopAndBroadcastPoint;


		/** Line searcher.
			@result map founded searched physical stops from the live data objects with the corresponding broadcast point if exists (NULL else) The broadcast points are temporary object and must be deleted after use. 
		*/
		std::vector<boost::shared_ptr<const env::CommercialLine> > getCommercialLineWithBroadcastPoints(int number=UNKNOWN_VALUE, int first=0);
	}
}

#endif // SYNTHESE_AdvancedSelectTableSync_H__
