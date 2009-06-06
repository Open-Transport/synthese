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

#include "UseRule.h"
#include "DateTime.h"

#include <limits>

using namespace std;

namespace synthese
{
	using namespace time;
	
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
			return value == RESERVATION_OPTIONAL_POSSIBLE || value == RESERVATION_COMPULSORY_POSSIBLE;
		}
	}
}
