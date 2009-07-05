
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

#ifndef SYNTHESE_env_AdvancedSelectTableSync_h__
#define SYNTHESE_env_AdvancedSelectTableSync_h__

#include "UId.h"
#include "Date.h"
#include "Registry.h"

#include <map>
#include <utility>

#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}
	
	namespace env
	{
		typedef std::map<std::pair<time::Date, int>, int> RunHours;

		RunHours getCommercialLineRunHours(
			util::Env& env,
			util::RegistryKeyType id,
			const boost::optional<time::Date>& startDate,
			const boost::optional<time::Date>& endDate
		);

		bool isPlaceServedByCommercialLine(uid Line, uid place);
		bool isPlaceServedByNetwork(uid networkId, uid place);
		
		/** Reads in database the rank of the last departure stop of a route.
			@param line The ID of the route
			@return int the rank of the last departure line-stop
			@author Hugues Romain
			@date 2007
			@throw Exception if there is no line stop corresponding to the specified route id. Possible reasons :
				- the route does not exist
				- no line-stop are registered for the specified route (database corruption)
				- no departure line-stop are registered for the specified route (database corruption)
		*/
		int getRankOfLastDepartureLineStop(uid line);
	}
}

#endif // SYNTHESE_env_AdvancedSelectTableSync_h__
