
/** Account class implementation.
	@file Account.cpp

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

#include "57_accounting/Account.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace util;

	namespace accounts
	{
		Account::Account(uid id)
			: Registrable<uid, Account>(id)
			, _leftUserId(0)
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

		shared_ptr<const Currency> Account::getLeftCurrency() const
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

		shared_ptr<const Currency> Account::getRightCurrency() const
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

		void Account::setLeftCurrency(boost::shared_ptr<const Currency> currency )
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

		void Account::setRightCurrency(shared_ptr<const Currency> currency )
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
