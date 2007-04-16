
/** VinciRate class header.
	@file VinciRate.h

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

#ifndef SYNTHESE_VinciRate_H__
#define SYNTHESE_VinciRate_H__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace vinci
	{
		/** Rates of bike rent.
			The durations are expressed in minutes
			@ingroup m71
		*/
		class VinciRate : public util::Registrable<uid, VinciRate>
		{
		private:
			std::string _name;
			double		_validityDuration;
			double		_startFinancialPrice;
			int			_startTicketsPrice;
			double		_endFinancialPrice;
			int			_endTicketsPrice;
			double		_firstPenalty;
			double		_firstPenaltyValidityDuration;
			double		_recurringPenalty;
			int			_recurringPenaltyPeriod;
			bool		_recurringPenaltyCancelsFirst;

			friend class db::SQLiteTableSyncTemplate<VinciRate>;

		public:
			VinciRate(uid id=0);

			void setName(const std::string& name);
			void setValidityDuration(double hours);
			void setStartFinancialPrice(double price);
			void setStartTicketsPrice(int price);
			void setEndFinancialPrice(double price);
			void setEndTicketsPrice(int price);
			void setFirstPenalty(double price);
			void setFirstPenaltyValidityDuration(double hours);
			void setRecurringPenalty(double price);
			void setRecurringPenaltyPeriod(int hours);
			void setRecurringPenaltyCancelsFirst(bool value);

			const std::string& getName() const;
			double getValidityDuration() const;
			double getStartFinancialPrice() const;
			int getStartTicketsPrice() const;
			double getEndFinancialPrice() const;
			int getEndTicketsPrice() const;
			double getFirstPenalty() const;
			double getFirstPenaltyValidityDuration() const;
			double getRecurringPenalty() const;
			int getRecurringPenaltyPeriod() const;
			bool getRecurringPenaltyCancelsFirst() const;

			time::DateTime	getEndDate(const time::DateTime& startDate)	const;
			time::DateTime	getFirstPenaltyEndDate(const time::DateTime& startDate)	const;
			double getAdditionalAmountToPay(const time::DateTime& startDate) const;
		};
	}
}

#endif // SYNTHESE_VinciRate_H__
