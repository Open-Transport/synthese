
#ifndef SYNTHESE_Account_H__
#define SYNTHESE_Account_H__

#include <string>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class Currency;

		/** Account.
			@ingroup m57
			The account can be used in several ways :
				- standard account between two entities : leftUser and rightUser, with or without numbers.
				- template account handled by an entity : all left variables are null or empty. leftUser will be specified in the transactions.
				- internal accounts of an entity : leftUser and rightUser points to the entity.
		*/
		class Account : public util::Registrable<uid, Account>
		{
		private:
			security::User* _leftUser;
			std::string		_leftNumber;
			std::string		_leftClassNumber;
			Currency*		_leftCurrency;
			security::User*	_rightUser;
			std::string		_rightNumber;
			std::string		_rightClassNumber;
			Currency*		_rightCurrency;
		};
	}
}

#endif // SYNTHESE_Account_H__
