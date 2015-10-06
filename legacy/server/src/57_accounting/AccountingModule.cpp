
/** AccountingModule class implementation.
	@file AccountingModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "57_accounting/AccountingModule.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,accounts::AccountingModule>::FACTORY_KEY("57_accounting");
	}

	namespace accounts
	{

		void AccountingModule::initialize()
		{

		}

		std::vector<std::pair<uid, std::string> > AccountingModule::getAccountsName(
			uid rightUserId
			, const std::string& className
			, bool emptyLine
		){
			string emptyString("%");
			vector<shared_ptr<Account> > accountss(AccountTableSync::search(rightUserId, className, UNKNOWN_VALUE, emptyString));
			vector<pair<uid, string> > result;
			if (emptyLine)
				result.push_back(make_pair(UNKNOWN_VALUE, string()));
			for (vector<shared_ptr<Account> >::const_iterator it(accountss.begin()); it != accountss.end(); ++it)
				result.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			return result;
		}

		std::string AccountingModule::getName() const
		{
			return "Modélisation comptabilité";
		}
	}
}
