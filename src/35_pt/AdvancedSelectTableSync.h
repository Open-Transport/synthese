
/** AdvancedSelectTableSync class header.
	@file 35_pt/AdvancedSelectTableSync.h

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

#ifndef SYNTHESE_env_AdvancedSelectTableSync_h__
#define SYNTHESE_env_AdvancedSelectTableSync_h__

#include "Registry.h"

#include <map>
#include <utility>

#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace pt
	{
		typedef std::map<std::pair<boost::gregorian::date, int>, int> RunHours;

		RunHours getCommercialLineRunHours(
			util::Env& env,
			util::RegistryKeyType id,
			const boost::optional<boost::gregorian::date>& startDate,
			const boost::optional<boost::gregorian::date>& endDate
		);

		bool isPlaceServedByCommercialLine(util::RegistryKeyType JourneyPattern, util::RegistryKeyType place);
		bool isPlaceServedByNetwork(util::RegistryKeyType networkId, util::RegistryKeyType place);

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
		int getRankOfLastDepartureLineStop(util::RegistryKeyType line);
	}
}

#endif // SYNTHESE_env_AdvancedSelectTableSync_h__
