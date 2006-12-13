
#include "57_accounting/Account.h"

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		Account::Account(uid id)
			: Registrable<uid, Account>(id)
			, _leftCurrency(NULL)
			, _leftUserId(0)
			, _rightCurrency(NULL)
			, _rightUserId(0)
		{

		}

		const std::string& Account::getLeftNumber() const
		{
			return _leftNumber;
		}

		const std::string& Account::getLeftClassNumber() const
		{
			return _leftClassNumber;
		}

		const Currency* Account::getLeftCurrency() const
		{
			return _leftCurrency;
		}

		const std::string& Account::getRightNumber() const
		{
			return _rightNumber;
		}

		const std::string& Account::getRightClassNumber() const
		{
			return _rightClassNumber;
		}

		const Currency* Account::getRightCurrency() const
		{
			return _rightCurrency;
		}

		void Account::setLeftUserId(uid id)
		{
			_leftUserId = id;
		}

		void Account::setLeftNumber( const std::string& LeftNumber )
		{
			_leftNumber = LeftNumber;
		}

		void Account::setLeftClassNumber( const std::string& classNumber )
		{
			_leftClassNumber = classNumber;
		}

		void Account::setLeftCurrency( Currency* currency )
		{
			_leftCurrency = currency;
		}

		void Account::setRightUserId(uid id)
		{
			_rightUserId = id;
		}

		void Account::setRightNumber( const std::string& RightNumber )
		{
			_rightNumber = RightNumber;
		}

		void Account::setRightClassNumber( const std::string& classNumber )
		{
			_rightClassNumber = classNumber;
		}

		void Account::setRightCurrency( Currency* currency )
		{
			_rightCurrency = currency;
		}

		const std::string& Account::getName() const
		{
			return _name;
		}

		void Account::setName( const std::string& name )
		{
			_name = name;
		}

		const uid Account::getRightUserId() const
		{
			return _rightUserId;
		}

		const uid Account::getLeftUserId() const
		{
			return _leftUserId;
		}
	}
}
