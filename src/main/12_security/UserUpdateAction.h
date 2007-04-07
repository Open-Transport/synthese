
/** UserUpdateAction class header.
	@file UserUpdateAction.h

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

#ifndef SYNTHESE_UserUpdateAction_H__
#define SYNTHESE_UserUpdateAction_H__

#include "30_server/Action.h"

namespace synthese
{
	namespace security
	{
		class Profile;
		class User;

		/** UserUpdateAction action class.
			@ingroup m12
		*/
		class UserUpdateAction : public server::Action
		{
		public:
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POSTAL_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_PHONE;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_AUTHORIZED_LOGIN;
			static const std::string PARAMETER_PROFILE_ID;

		private:
			User*		_user;
			std::string _login;
			std::string _surname;
			std::string _name;
			std::string _address;
			std::string _postalCode;
			std::string _city;
			std::string _phone;
			std::string _email;
			bool		_authorizedLogin;
			Profile*	_profile;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Constructor.
			*/
			UserUpdateAction();
			~UserUpdateAction();

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_UserUpdateAction_H__
