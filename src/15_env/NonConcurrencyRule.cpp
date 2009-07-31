
/** NonConcurrencyRule class implementation.
	@file NonConcurrencyRule.cpp

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

#include "NonConcurrencyRule.h"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<env::NonConcurrencyRule>::KEY("NonConcurrencyRule");
	}

	namespace env
	{


		NonConcurrencyRule::NonConcurrencyRule(
			RegistryKeyType key
		):	Registrable(key),
			_prorityLine(NULL),
			_hiddenLine(NULL),
			_delay(minutes(0))
		{}

		CommercialLine* NonConcurrencyRule::getPriorityLine() const
		{
			return _prorityLine;
		}

		CommercialLine* NonConcurrencyRule::getHiddenLine() const
		{
			return _hiddenLine;
		}

		const time_duration& NonConcurrencyRule::getDelay() const
		{
			return _delay;
		}

		void NonConcurrencyRule::setPriorityLine(CommercialLine* value )
		{
			_prorityLine = value;
		}

		void NonConcurrencyRule::setHiddenLine(CommercialLine* value )
		{
			_hiddenLine = value;
		}

		void NonConcurrencyRule::setDelay(const time_duration& value )
		{
			_delay = value;
		}
	}
}
