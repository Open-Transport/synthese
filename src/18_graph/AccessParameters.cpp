
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

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace graph;
	
	namespace graph
	{
		const char* AccessParameters::_SERIALIZATION_SEPARATOR("|");

		AccessParameters::AccessParameters(
			UserClassCode userClass,
			bool drtOnly /*= boost::logic::indeterminate */
			, bool withoutDrt
			, double maxApproachDistance /*= 1000 */
			, double maxApproachTime /*= 23 */
			, double approachSpeed /*= 34 */
			, optional<size_t> maxTransportConnectionCount /*= 10  */
		):	_maxApproachDistance(maxApproachDistance)
			, _maxApproachTime(maxApproachTime)
			, _approachSpeed(approachSpeed)
			, _maxTransportConnectionCount(maxTransportConnectionCount)
			, _drtOnly(drtOnly)
			, _withoutDrt(withoutDrt),
			_userClass(userClass)
		{
		}



		AccessParameters::AccessParameters( const std::string& serialized )
		{
			tokenizer<char_separator<char> > tokens(serialized, char_separator<char>(_SERIALIZATION_SEPARATOR, 0, keep_empty_tokens));
			tokenizer<char_separator<char> >::iterator it(tokens.begin());
			_maxApproachDistance = lexical_cast<double>(*it);
			++it;
			_maxApproachTime = lexical_cast<double>(*it);
			++it;
			_approachSpeed = lexical_cast<double>(*it);
			++it;
			if(!it->empty()) _maxTransportConnectionCount = lexical_cast<size_t>(*it);
			++it;
			_drtOnly = lexical_cast<bool>(*it);
			++it;
			_withoutDrt = lexical_cast<bool>(*it);
			++it;
			_userClass = lexical_cast<UserClassCode>(*it);
		}



		std::string AccessParameters::serialize() const
		{
			stringstream s;
			s <<
				_maxApproachDistance << _SERIALIZATION_SEPARATOR <<
				_maxApproachTime << _SERIALIZATION_SEPARATOR <<
				_approachSpeed << _SERIALIZATION_SEPARATOR <<
				(_maxTransportConnectionCount ? lexical_cast<string>(*_maxTransportConnectionCount) : string()) << _SERIALIZATION_SEPARATOR <<
				_drtOnly << _SERIALIZATION_SEPARATOR <<
				_withoutDrt << _SERIALIZATION_SEPARATOR <<
				_userClass
			;
			return s.str();
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



		double AccessParameters::getMaxApproachTime() const
		{
			return _maxApproachTime;
		}
	}
}
