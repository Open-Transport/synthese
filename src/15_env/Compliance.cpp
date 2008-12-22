
/** Compliance class implementation.
	@file Compliance.cpp

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

#include "Compliance.h"
#include "ReservationRule.h"
#include "Env.h"

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace env
	{
		Compliance::Compliance(
			boost::logic::tribool compliant
			, int capacity
			, shared_ptr<const ReservationRule> reservationRule
		):	_compliant (compliant),
			Registrable(UNKNOWN_VALUE)
			, _capacity (capacity)
			, _reservationRule(reservationRule)
		{
			if (_reservationRule.get() == NULL)
			{
				_reservationRule = Env::GetOfficialEnv().getEditableRegistry<ReservationRule>().getWithAutoCreation(0);
			}
		}

		Compliance::~Compliance()
		{
		}


		int 
		Compliance::getCapacity () const
		{
			return _capacity;
		}


		const boost::logic::tribool& Compliance::isCompliant () const
		{
			return _compliant;
		}

		void 
		Compliance::setCapacity (int capacity)
		{
			_capacity = capacity;
		}



		void 
		Compliance::setCompliant (const boost::logic::tribool& compliant)
		{
			_compliant = compliant;
		}

		bool Compliance::isCompatibleWith( const Compliance& compliance ) const
		{
			return isCompatibleWith(compliance._compliant);
		}

		bool Compliance::isCompatibleWith(const boost::logic::tribool& value ) const
		{
			return
				boost::logic::indeterminate(value)
				|| boost::logic::indeterminate(_compliant)
				|| _compliant == value;
		}

		void Compliance::setReservationRule(shared_ptr<const ReservationRule> value )
		{
			_reservationRule = value;
			if (_reservationRule.get() == NULL)
			{
				_reservationRule.reset(new ReservationRule(0));
			}
		}

		
		
		shared_ptr<const ReservationRule> Compliance::getReservationRule() const
		{
			return _reservationRule;
		}
	}
}
