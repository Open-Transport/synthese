
/** AccessParameters class implementation.
	@file AccessParameters.cpp

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

#include "UseRules.h"
#include "AccessParameters.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace graph;
	
	namespace graph
	{
		AccessParameters::AccessParameters(
			UserClassCode userClass,
			bool drtOnly /*= boost::logic::indeterminate */
			, bool withoutDrt
			, double maxApproachDistance /*= 1000 */
			, double maxApproachTime /*= 23 */
			, double approachSpeed /*= 34 */
			, int maxTransportConnectionCount /*= 10  */
		)	: _maxApproachDistance(maxApproachDistance)
			, _maxApproachTime(maxApproachTime)
			, _approachSpeed(approachSpeed)
			, _maxTransportConnectionCount(maxTransportConnectionCount)
			, _drtOnly(drtOnly)
			, _withoutDrt(withoutDrt),
			_userClass(userClass)
		{
		}

		bool AccessParameters::isCompatibleWith(
			const UseRules* rules
		) const {
			if (rules == NULL) return true;
			
			const UseRule& rule(rules->getUseRule(_userClass));
			
			if(!rule.getAccess()) return false;
			
// 			if (_fare && complyer.getFare()->isCompliant() == logic::tribool(true) && complyer.getFare().get() != _fare)
// 				return false;

 			if (_drtOnly && rule.getReservationType() == UseRule::RESERVATION_FORBIDDEN) return false;

 			if (_withoutDrt && rule.getReservationType() != UseRule::RESERVATION_FORBIDDEN) return false;

			return true;
		}

		bool AccessParameters::isCompatibleWithApproach( double distance, double duration) const
		{
			return distance < _maxApproachDistance && duration < _maxApproachTime;
		}

		double AccessParameters::getApproachSpeed() const
		{
			return _approachSpeed;
		}
		
		
		
		UserClassCode AccessParameters::getUserClass() const
		{
			return _userClass;
		}
	}
}
