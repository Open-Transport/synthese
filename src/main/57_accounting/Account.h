
#ifndef SYNTHESE_Account_H__
#define SYNTHESE_Account_H__

#include <string>

#include "01_util/UId.h"
#include "01_util/Registrable.h"

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
			uid				_leftUserId;
			std::string		_leftNumber;
			std::string		_leftClassNumber;
			Currency*		_leftCurrency;
			uid				_rightUserId;
			std::string		_rightNumber;
			std::string		_rightClassNumber;
			Currency*		_rightCurrency;
			std::string		_name;

		public:
			Account(uid id=0);

			const security::User*	getLeftUser() const;
			const uid				getLeftUserId() const;
			const std::string&		getLeftNumber() const;
			const std::string&		getLeftClassNumber() const;
			const Currency*			getLeftCurrency() const;
			const security::User*	getRightUser() const;
			const uid				getRightUserId() const;
			const std::string&		getRightNumber() const;
			const std::string&		getRightClassNumber() const;
			const Currency*			getRightCurrency() const;
			const std::string&		getName() const;

			void setLeftUserId(uid id);
			void setLeftNumber(const std::string& LeftNumber);
			void setLeftClassNumber(const std::string& classNumber);
			void setLeftCurrency(Currency* currency);
			void setRightUserId(uid id);
			void setRightNumber(const std::string& RightNumber);
			void setRightClassNumber(const std::string& classNumber);
			void setRightCurrency(Currency* currency);
			void setName(const std::string& name);

		};
	}
}

#endif // SYNTHESE_Account_H__
