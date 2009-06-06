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

#include "AllowedUseRule.h"
#include "DateTime.h"
#include "AccessParameters.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace time;

	namespace graph
	{
		shared_ptr<const AllowedUseRule> AllowedUseRule::INSTANCE(new AllowedUseRule);

		time::DateTime AllowedUseRule::getReservationDeadLine(
			const time::DateTime& originTime,
			const time::DateTime& departureTime
		) const	{
			return departureTime;
		}

		UseRule::AccessCapacity AllowedUseRule::getAccessCapacity() const
		{
			return AccessCapacity();
		}

		time::DateTime AllowedUseRule::getReservationOpeningTime( const ServicePointer& servicePointer ) const
		{
			return DateTime(TIME_MIN);
		}

		UseRule::RunPossibilityType AllowedUseRule::isRunPossible(
			const ServicePointer& servicePointer
		) const	{
			return RUN_POSSIBLE;
		}

		UseRule::RunPossibilityType AllowedUseRule::isRunPossible(
			const ServiceUse& serviceUse
		) const	{
			return RUN_POSSIBLE;
		}

		UseRule::ReservationAvailabilityType AllowedUseRule::getReservationAvailability(
			const ServiceUse& serviceUse
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		UseRule::ReservationAvailabilityType AllowedUseRule::getReservationAvailability(
			const ServicePointer& servicePointer
		) const	{
			return RESERVATION_FORBIDDEN;
		}

		bool AllowedUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			if(accessParameters.getDRTOnly()) return false;
			return true;
		}
	}
}
