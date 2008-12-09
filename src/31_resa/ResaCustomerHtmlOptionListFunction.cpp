
/** ResaCustomerHtmlOptionListFunction class implementation.
	@file ResaCustomerHtmlOptionListFunction.cpp
	@author Hugues Romain
	@date 2008

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

#include "Conversion.h"

#include "RequestException.h"
#include "RequestMissingParameterException.h"

#include "ResaCustomerHtmlOptionListFunction.h"

#include "User.h"
#include "UserTableSync.h"
#include "Env.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,resa::ResaCustomerHtmlOptionListFunction>::FACTORY_KEY("ResaCustomerHtmlOptionListFunction");

	namespace resa
	{
		const string ResaCustomerHtmlOptionListFunction::PARAMETER_NAME("na");
		const string ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME("sn");
		const string ResaCustomerHtmlOptionListFunction::PARAMETER_NUMBER("nu");

		ParametersMap ResaCustomerHtmlOptionListFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NUMBER, _number);
			return map;
		}

		void ResaCustomerHtmlOptionListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_surname = "%" + map.getString(PARAMETER_SURNAME, true, FACTORY_KEY) +"%";
			_name = "%" + map.getString(PARAMETER_NAME, true, FACTORY_KEY) + "%";
			_number = map.getInt(PARAMETER_NUMBER, true, FACTORY_KEY);
		}

		void ResaCustomerHtmlOptionListFunction::_run( std::ostream& stream ) const
		{
			if (_name == "%%" && _surname == "%%")
				return;

			Env env;
			UserTableSync::Search(env, "%", _name, _surname, "%", UNKNOWN_VALUE, logic::indeterminate, 0, _number, false, true, false, true);
			BOOST_FOREACH(shared_ptr<User> user, env.template getRegistry<User>())
			{
				stream << "<option value=\"" << user->getKey() << "\">" << user->getName() << " " << user->getSurname() << " (" << user->getPhone() << " / " << user->getEMail() << ")</option>";
			}
		}

		void ResaCustomerHtmlOptionListFunction::setNumber( int number )
		{
			_number = number;
		}
	}
}
