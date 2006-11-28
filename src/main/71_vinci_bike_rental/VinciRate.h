
#ifndef SYNTHESE_VinciRate_H__
#define SYNTHESE_VinciRate_H__

#include <string>

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
		};
	}
}

#endif // SYNTHESE_VinciRate_H__
