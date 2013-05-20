////////////////////////////////////////////////////////////////////////////////
/// ResaCustomerHtmlOptionListFunction class implementation.
///	@file ResaCustomerHtmlOptionListFunction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ResaCustomerHtmlOptionListFunction.h"

#include "Profile.h"
#include "Request.h"
#include "RequestException.h"
#include "ResaRight.h"
#include "Session.h"
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
			if(_name) map.insert(PARAMETER_NAME, *_name);
			if(_surname) map.insert(PARAMETER_NUMBER, *_surname);
			map.insert(PARAMETER_NUMBER, static_cast<int>(_number));
			return map;
		}

		void ResaCustomerHtmlOptionListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			if(!map.getDefault<string>(PARAMETER_SURNAME).empty())
			{
				_surname = map.getOptional<string>(PARAMETER_SURNAME);
			}
			if(!map.getDefault<string>(PARAMETER_NAME).empty())
			{
				_name = map.getOptional<string>(PARAMETER_NAME);
			}
			_number = map.getDefault<size_t>(PARAMETER_NUMBER, 20);
		}



		util::ParametersMap ResaCustomerHtmlOptionListFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{
			if (!_name && !_surname)
			{
				return util::ParametersMap();
			}

			Env env;
			UserTableSync::SearchResult users(
				UserTableSync::Search(
					env,
					optional<string>(),
					_name ? "%"+ *_name +"%" : _name,
					_surname ? "%"+ *_surname +"%" : _surname,
					optional<string>(),
					optional<RegistryKeyType>(),
					logic::indeterminate,
					false,
					optional<RegistryKeyType>(),
					0,
					_number,
					false,
					true,
					false,
					true
			)	);
			BOOST_FOREACH(const boost::shared_ptr<User>& user, users)
			{
				stream << "<option value=\"" << user->getKey() << "\">" << user->getName() << " " << user->getSurname() << " (" << user->getPhone() << " / " << user->getEMail() << ")</option>";
			}

			return util::ParametersMap();
		}

		void ResaCustomerHtmlOptionListFunction::setNumber( int number )
		{
			_number = number;
		}



		bool ResaCustomerHtmlOptionListFunction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(READ);
		}

		std::string ResaCustomerHtmlOptionListFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		ResaCustomerHtmlOptionListFunction::ResaCustomerHtmlOptionListFunction():
			FactorableTemplate<server::Function,ResaCustomerHtmlOptionListFunction>()
		{
			setEnv(boost::shared_ptr<Env>(new Env));
		}
	}
}
