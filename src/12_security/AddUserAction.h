
/** AddUserAction class header.
	@file AddUserAction.h

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

#ifndef SYNTHESE_AddUserAction_H__
#define SYNTHESE_AddUserAction_H__

#include <boost/shared_ptr.hpp>

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** User creation action.
			@ingroup m12Actions refActions
		*/
		class AddUserAction : public util::FactorableTemplate<server::Action,AddUserAction>
		{
		public:
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PROFILE_ID;
			static const std::string PARAMETER_PASS1;
			static const std::string PARAMETER_PASS2;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POSTAL_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_COUNTRY;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_PHONE;

		private:
			std::string _login;
			std::string _name;
			boost::shared_ptr<const Profile>	_profile;
			std::string _password;
			std::string _surname;
			std::string _address;
			std::string _postalCode;
			std::string _city;
			std::string _country;
			std::string _phone;
			std::string _email;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters to read
				@throw ActionException if invalid parameter
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_AddUserAction_H__

