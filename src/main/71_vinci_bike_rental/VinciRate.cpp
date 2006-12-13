
#include "71_vinci_bike_rental/VinciRate.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{
		VinciRate::VinciRate(uid id)
			: Registrable<uid, VinciRate>(id)
		{}

		void VinciRate::setValidityDuration(int hours)
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

		void VinciRate::setFirstPenaltyValidityDuration(int hours)
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

		int VinciRate::getValidityDuration() const
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

		int VinciRate::getFirstPenaltyValidityDuration() const
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

	}
}
