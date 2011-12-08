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

#include "UseRule.h"

#include <limits>

using namespace std;

namespace synthese
{
	namespace graph
	{
		UseRule::UseRule(
		){
		}



		UseRule::~UseRule()
		{
		}



		bool UseRule::IsReservationPossible( const UseRule::ReservationAvailabilityType& value )
		{
			return value == RESERVATION_OPTIONAL_POSSIBLE || value == RESERVATION_COMPULSORY_POSSIBLE || value == RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;
		}
	}
}
