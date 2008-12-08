
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

#include "15_env/ReservationRule.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/BikeCompliance.h"
#include "15_env/Fare.h"
#include "15_env/Complyer.h"

#include "AccessParameters.h"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace env
	{
		AccessParameters::AccessParameters(
			bool bikeCompliance /*= boost::logic::indeterminate */
			, Fare*  fare /*= NULL */
			, bool handicappedCompliance /*= boost::logic::indeterminate */
			, bool pedestrianCompliance /*= boost::logic::indeterminate */
			, bool drtOnly /*= boost::logic::indeterminate */
			, bool withoutDrt
			, double maxApproachDistance /*= 1000 */
			, double maxApproachTime /*= 23 */
			, double approachSpeed /*= 34 */
			, int maxTransportConnectionCount /*= 10  */
		)	: _maxApproachDistance(maxApproachDistance)
			, _maxApproachTime(maxApproachTime)
			, _approachSpeed(approachSpeed)
			, _maxTransportConnectionCount(maxTransportConnectionCount)
			, _withBike(bikeCompliance)
			, _pedestrian(pedestrianCompliance)
			, _drtOnly(drtOnly)
			, _withoutDrt(withoutDrt)
			, _disabledUser(handicappedCompliance)
			, _fare(fare)
		{
		}

		bool AccessParameters::isCompatibleWith( const Complyer& complyer ) const
		{
			if (_withBike && !complyer.getBikeCompliance()->isCompatibleWith(logic::tribool(true)))
				return false;

			if (_fare && complyer.getFare()->isCompliant() == logic::tribool(true) && complyer.getFare().get() != _fare)
				return false;

			if (_disabledUser && !complyer.getHandicappedCompliance()->isCompatibleWith(logic::tribool(true)))
				return false;

			if (_pedestrian && !complyer.getPedestrianCompliance()->isCompatibleWith(logic::tribool(true)))
				return false;

			if (_drtOnly && !complyer.getReservationRule()->isCompatibleWith(logic::tribool(true)))
				return false;

			if (_withoutDrt && !complyer.getReservationRule()->isCompatibleWith(logic::tribool(false)))
				return false;

			return true;
		}

		bool AccessParameters::isCompatibleWithApproach( double distance, double duration) const
		{
			return distance < _maxApproachDistance && duration < _maxApproachTime;
		}

		bool AccessParameters::getBikeFilter() const
		{
			return _withBike;
		}

		bool AccessParameters::getHandicappedFilter() const
		{
			return _disabledUser;
		}

		double AccessParameters::getApproachSpeed() const
		{
			return _approachSpeed;
		}
	}
}
