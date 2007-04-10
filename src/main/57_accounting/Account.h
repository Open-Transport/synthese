
/** Account class header.
	@file Account.h

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

#ifndef SYNTHESE_Account_H__
#define SYNTHESE_Account_H__

#include <string>

#include <boost/shared_ptr.hpp>

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
			boost::shared_ptr<const Currency>	_leftCurrency;
			uid				_rightUserId;
			std::string		_rightNumber;
			std::string		_rightClassNumber;
			boost::shared_ptr<const Currency>	_rightCurrency;
			std::string		_name;

		public:
			Account(uid id=0);

			const uid				getLeftUserId() const;
			const std::string&		getLeftNumber() const;
			const std::string&		getLeftClassNumber() const;
			boost::shared_ptr<const Currency>		getLeftCurrency() const;
			const uid				getRightUserId() const;
			const std::string&		getRightNumber() const;
			const std::string&		getRightClassNumber() const;
			boost::shared_ptr<const Currency>		getRightCurrency() const;
			const std::string&		getName() const;

			void setLeftUserId(uid id);
			void setLeftNumber(const std::string& LeftNumber);
			void setLeftClassNumber(const std::string& classNumber);
			void setLeftCurrency(boost::shared_ptr<const Currency> currency);
			void setRightUserId(uid id);
			void setRightNumber(const std::string& RightNumber);
			void setRightClassNumber(const std::string& classNumber);
			void setRightCurrency(boost::shared_ptr<const Currency> currency);
			void setName(const std::string& name);

		};
	}
}

#endif // SYNTHESE_Account_H__
