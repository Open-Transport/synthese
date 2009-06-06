//
// C++ Implementation: UseRule
//
// Description: 
//
//
// Author: Hugues Romain (RCS) <hugues.romain@reseaux-conseil.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ForbiddenUseRule.h"
#include "DateTime.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace time;

	namespace graph
	{
		shared_ptr<const ForbiddenUseRule> ForbiddenUseRule::INSTANCE(new ForbiddenUseRule);

		time::DateTime ForbiddenUseRule::getReservationDeadLine( const time::DateTime& originTime, const time::DateTime& departureTime ) const
		{
			return DateTime(TIME_MIN);
		}

		UseRule::AccessCapacity ForbiddenUseRule::getAccessCapacity() const
		{
			return AccessCapacity(0);
		}

		time::DateTime ForbiddenUseRule::getReservationOpeningTime(
			const ServicePointer& servicePointer
		) const	{
			return DateTime(TIME_MAX);
		}

		UseRule::RunPossibilityType ForbiddenUseRule::isRunPossible(
			const ServiceUse& serviceUse
		) const	{
			return RUN_NOT_POSSIBLE;
		}

		UseRule::RunPossibilityType ForbiddenUseRule::isRunPossible(
			const ServicePointer& servicePointer
		) const	{
			return RUN_NOT_POSSIBLE;
		}

		UseRule::ReservationAvailabilityType ForbiddenUseRule::getReservationAvailability(
			const ServiceUse& serviceUse
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		UseRule::ReservationAvailabilityType ForbiddenUseRule::getReservationAvailability(
			const ServicePointer& servicePointer
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		bool ForbiddenUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			return false;
		}
	}
}
