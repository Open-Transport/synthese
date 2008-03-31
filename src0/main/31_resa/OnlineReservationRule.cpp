
/** OnlineReservationRule class implementation.
	@file OnlineReservationRule.cpp

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

#include "31_resa/OnlineReservationRule.h"

#include "15_env/EnvModule.h"

#include "01_util/Constants.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace env;

	namespace resa
	{
		OnlineReservationRule::OnlineReservationRuleMap OnlineReservationRule::_onlineReservationRuleMap;

		OnlineReservationRule::OnlineReservationRule()
			: _maxSeats(UNKNOWN_VALUE)
			, _reservationRule(NULL)
		{
			
		}

		const env::ReservationRule* OnlineReservationRule::getReservationRule() const
		{
			return _reservationRule;
		}

		const std::string& OnlineReservationRule::getEMail() const
		{
			return _eMail;
		}

		const std::string& OnlineReservationRule::getCopyEMail() const
		{
			return _copyEMail;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsSurname() const
		{
			return _needsSurname;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsAddress() const
		{
			return _needsAddress;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsPhone() const
		{
			return _needsPhone;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsCustomerNumber() const
		{
			return _needsCustomerNumber;
		}

		boost::logic::tribool OnlineReservationRule::getNeedsEMail() const
		{
			return _needsEMail;
		}

		int OnlineReservationRule::getMaxSeats() const
		{
			return _maxSeats;
		}

		const OnlineReservationRule::CapacityThresholds& OnlineReservationRule::getThresholds() const
		{
			return _thresholds;
		}

		void OnlineReservationRule::setReservationRule(const ReservationRule* rule)
		{
			if (_reservationRule != NULL)
			{
				OnlineReservationRuleMap::iterator it(_onlineReservationRuleMap.find(_reservationRule));
				assert(it != _onlineReservationRuleMap.end());
				_onlineReservationRuleMap.erase(it);
			}
			_reservationRule = rule;
			_onlineReservationRuleMap.insert(make_pair(rule, this));
		}

		void OnlineReservationRule::setEMail( const std::string& email )
		{
			_eMail = email;
		}

		void OnlineReservationRule::setCopyEMail( const std::string& email )
		{
			_copyEMail = email;
		}

		void OnlineReservationRule::setNeedsSurname( boost::logic::tribool value )
		{
			_needsSurname = value;
		}

		void OnlineReservationRule::setNeedsAddress( boost::logic::tribool value )
		{
			_needsAddress = value;
		}

		void OnlineReservationRule::setNeedsPhone( boost::logic::tribool value )
		{
			_needsPhone = value;
		}

		void OnlineReservationRule::setNeedsCustomerNumber( boost::logic::tribool value )
		{
			_needsCustomerNumber = value;
		}

		void OnlineReservationRule::setNeedsEMail( boost::logic::tribool value )
		{
			_needsEMail = value;
		}

		void OnlineReservationRule::setMaxSeats( int value )
		{
			_maxSeats = value;
		}

		void OnlineReservationRule::setThresholds( const CapacityThresholds& thresholds )
		{
			_thresholds = thresholds;
		}

		const OnlineReservationRule* OnlineReservationRule::GetOnlineReservationRule( const env::ReservationRule* rule )
		{
			OnlineReservationRuleMap::const_iterator it(_onlineReservationRuleMap.find(rule));
			return (it == _onlineReservationRuleMap.end()) ? NULL : it->second;
		}
	}
}
