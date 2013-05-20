//
// C++ Implementation: UseRule
//
// Description:
//
//
// Author: Hugues Romain (RCS) <hromain@rcsmobility.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ForbiddenUseRule.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		boost::shared_ptr<const ForbiddenUseRule> ForbiddenUseRule::INSTANCE(new ForbiddenUseRule);

		ptime ForbiddenUseRule::getReservationDeadLine( const ptime& originTime, const ptime& departureTime ) const
		{
			return ptime(neg_infin);
		}

		UseRule::AccessCapacity ForbiddenUseRule::getAccessCapacity() const
		{
			return AccessCapacity(0);
		}

		ptime ForbiddenUseRule::getReservationOpeningTime(
			const ServicePointer& servicePointer
		) const	{
			return ptime(pos_infin);
		}


		UseRule::RunPossibilityType ForbiddenUseRule::isRunPossible(
			const ServicePointer& servicePointer,
			bool ignoreReservation
		) const	{
			return RUN_NOT_POSSIBLE;
		}



		UseRule::ReservationAvailabilityType ForbiddenUseRule::getReservationAvailability(
			const ServicePointer& servicePointer,
			bool ignoreReservationDeadline
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		bool ForbiddenUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			return false;
		}



		std::string ForbiddenUseRule::getUseRuleName() const
		{
			return "Accès interdit";
		}
	}
}
