
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

#include "AccessParameters.h"
#include "RuleUser.h"
#include "UseRule.h"

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
			, optional<size_t> maxTransportConnectionCount /*= 10  */
		)	: _maxApproachDistance(maxApproachDistance)
			, _maxApproachTime(maxApproachTime)
			, _approachSpeed(approachSpeed)
			, _maxTransportConnectionCount(maxTransportConnectionCount)
			, _drtOnly(drtOnly)
			, _withoutDrt(withoutDrt),
			_userClass(userClass)
		{
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

		boost::optional<size_t> AccessParameters::getMaxtransportConnectionsCount() const
		{
			return _maxTransportConnectionCount;
		}

		bool AccessParameters::getDRTOnly() const
		{
			return _drtOnly;
		}

		bool AccessParameters::getWithoutDRT() const
		{
			return _withoutDrt;
		}
	}
}
