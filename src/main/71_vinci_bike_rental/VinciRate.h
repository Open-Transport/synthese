
#ifndef SYNTHESE_VinciRate_H__
#define SYNTHESE_VinciRate_H__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		/** Rates of bike rent.
			The durations are expressed in minutes
		*/
		class VinciRate : public util::Registrable<uid, VinciRate>
		{
		private:
			std::string _name;
			int			_validityDuration;
			double		_startFinancialPrice;
			int			_startTicketsPrice;
			double		_endFinancialPrice;
			int			_endTicketsPrice;
			double		_firstPenalty;
			int			_firstPenaltyValidityDuration;
			double		_recurringPenalty;
			int			_recurringPenaltyPeriod;

			friend class db::SQLiteTableSyncTemplate<VinciRate>;

		public:
			VinciRate(uid id=0);

			void setValidityDuration(int hours);
			void setStartFinancialPrice(double price);
			void setStartTicketsPrice(int price);
			void setEndFinancialPrice(double price);
			void setEndTicketsPrice(int price);
			void setFirstPenalty(double price);
			void setFirstPenaltyValidityDuration(int hours);
			void setRecurringPenalty(double price);
			void setRecurringPenaltyPeriod(int hours);

			int getValidityDuration() const;
			double getStartFinancialPrice() const;
			int getStartTicketsPrice() const;
			double getEndFinancialPrice() const;
			int getEndTicketsPrice() const;
			double getFirstPenalty() const;
			int getFirstPenaltyValidityDuration() const;
			double getRecurringPenalty() const;
			int getRecurringPenaltyPeriod() const;
		};
	}
}

#endif // SYNTHESE_VinciRate_H__

