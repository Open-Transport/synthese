
/** AllowedUseRule class implementation.
	@file AllowedUseRule.cpp

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


#include "AllowedUseRule.h"
#include "AccessParameters.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		boost::shared_ptr<const AllowedUseRule> AllowedUseRule::INSTANCE(new AllowedUseRule);

		boost::posix_time::ptime AllowedUseRule::getReservationDeadLine(
			const boost::posix_time::ptime& originTime,
			const boost::posix_time::ptime& departureTime
		) const	{
			return departureTime;
		}

		UseRule::AccessCapacity AllowedUseRule::getAccessCapacity() const
		{
			return AccessCapacity();
		}

		boost::posix_time::ptime AllowedUseRule::getReservationOpeningTime( const ServicePointer& servicePointer ) const
		{
			return ptime(neg_infin);
		}

		UseRule::RunPossibilityType AllowedUseRule::isRunPossible(
			const ServicePointer& servicePointer,
			bool ignoreReservation
		) const	{
			return RUN_POSSIBLE;
		}

		UseRule::ReservationAvailabilityType AllowedUseRule::getReservationAvailability(
			const ServicePointer& serviceUse,
			bool ignoreReservationDeadline
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		bool AllowedUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			if(accessParameters.getDRTOnly()) return false;
			return true;
		}



		std::string AllowedUseRule::getUseRuleName() const
		{
			return "Autorisé sans condition";
		}
	}
}
