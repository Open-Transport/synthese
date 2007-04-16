
/** VinciRate class implementation.
	@file VinciRate.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include <math.h>

#include "71_vinci_bike_rental/VinciRate.h"

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace vinci
	{
		VinciRate::VinciRate(uid id)
			: Registrable<uid, VinciRate>(id)
		{}

		void VinciRate::setValidityDuration(double hours)
		{
			_validityDuration = hours;
		}

		void VinciRate::setStartFinancialPrice(double price)
		{
			_startFinancialPrice = price;
		}

		void VinciRate::setStartTicketsPrice(int price)
		{
			_startTicketsPrice = price;
		}

		void VinciRate::setEndFinancialPrice(double price)
		{
			_endFinancialPrice = price;
		}

		void VinciRate::setEndTicketsPrice(int price)
		{
			_endTicketsPrice = price;
		}

		void VinciRate::setFirstPenalty(double price)
		{
			_firstPenalty = price;
		}

		void VinciRate::setFirstPenaltyValidityDuration(double hours)
		{
			_firstPenaltyValidityDuration = hours;
		}

		void VinciRate::setRecurringPenalty(double price)
		{
			_recurringPenalty = price;
		}

		void VinciRate::setRecurringPenaltyPeriod(int hours)
		{
			_recurringPenaltyPeriod = hours;
		}

		double VinciRate::getValidityDuration() const
		{
			return _validityDuration;
		}

		double VinciRate::getStartFinancialPrice() const
		{
			return _startFinancialPrice;
		}

		int VinciRate::getStartTicketsPrice() const
		{
			return _startTicketsPrice;
		}

		double VinciRate::getEndFinancialPrice() const
		{
			return _endFinancialPrice;
		}

		int VinciRate::getEndTicketsPrice() const
		{
			return _endTicketsPrice;
		}

		double VinciRate::getFirstPenalty() const
		{
			return _firstPenalty;
		}

		double VinciRate::getFirstPenaltyValidityDuration() const
		{
			return _firstPenaltyValidityDuration;
		}

		double VinciRate::getRecurringPenalty() const
		{
			return _recurringPenalty;
		}

		int VinciRate::getRecurringPenaltyPeriod() const
		{
			return _recurringPenaltyPeriod;
		}

		const std::string& VinciRate::getName() const
		{
			return _name;
		}

		void VinciRate::setName( const std::string& name )
		{
			_name = name;
		}

		time::DateTime VinciRate::getEndDate( const time::DateTime& startDate ) const
		{
			DateTime endDate = startDate;
			endDate += (int) (_validityDuration * 60);
			return endDate;
		}

		double VinciRate::getAdditionalAmountToPay(const time::DateTime& startDate) const
		{
			DateTime now;
			if (now <= getEndDate(startDate))
				return 0;

			if (now <= getFirstPenaltyEndDate(startDate))
				return _firstPenalty;

			if (getRecurringPenaltyCancelsFirst())
			{
				double penalties = ceil((double) ((now - startDate) / (double) (_recurringPenaltyPeriod * 60)));
				return _recurringPenalty * penalties;
			}
			else
			{
				double penalties = ceil((double) ((now - getFirstPenaltyEndDate(startDate)) / (double) (_recurringPenaltyPeriod * 60)));
				return _recurringPenalty * penalties + _firstPenalty;
			}

		}

		time::DateTime VinciRate::getFirstPenaltyEndDate( const time::DateTime& startDate ) const
		{
			DateTime endDate = getEndDate(startDate);
			endDate += (int) (_firstPenaltyValidityDuration * 60);
			return endDate;
		}

		void VinciRate::setRecurringPenaltyCancelsFirst( bool value )
		{
			_recurringPenaltyCancelsFirst = value;
		}

		bool VinciRate::getRecurringPenaltyCancelsFirst() const
		{
			return _recurringPenaltyCancelsFirst;
		}
	}
}
