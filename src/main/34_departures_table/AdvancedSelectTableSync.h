
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

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
	}

	namespace departurestable
	{
		typedef struct { env::ConnectionPlace* place; int broadCastPointsNumber; } ConnectionPlaceWithBroadcastPoint;

		/** Connection place searcher.
			@result vector of the founded searched connection places from the live data objects (do not delete the objects after use). 
		*/
		std::vector<ConnectionPlaceWithBroadcastPoint> searchConnectionPlacesWithBroadcastPoints(std::string cityName = "", std::string placeName = "", int _bpNumbers = UNKNOWN_VALUE, uid lineId = UNKNOWN_VALUE, int number=UNKNOWN_VALUE, int first=0);
	}
}

#endif // SYNTHESE_AdvancedSelectTableSync_H__
