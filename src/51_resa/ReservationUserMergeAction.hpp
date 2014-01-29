
/** ReservationUserMergeAction class header.
	@file ReservationUserMergeAction.hpp
	@date 2014

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

#ifndef SYNTHESE_ReservationUserMergeAction_H__
#define SYNTHESE_ReservationUserMergeAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** ReservationUserMergeAction action class.
			@ingroup m51Actions refActions
		*/
		class ReservationUserMergeAction:
			public util::FactorableTemplate<server::Action, ReservationUserMergeAction>
		{
		public:
			static const std::string PARAMETER_USER_ID;
            static const std::string PARAMETER_USER_TO_DELETE;
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_ADDRESS;
			static const std::string PARAMETER_POSTAL_CODE;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_PHONE;
			static const std::string PARAMETER_EMAIL;
			static const std::string PARAMETER_AUTHORIZED_LOGIN;
			static const std::string PARAMETER_AUTORESA_ACTIVATED;

		private:
			boost::shared_ptr<security::User>	_userToMerge;
			boost::shared_ptr<security::User>	_userToDelete;

			std::string				_login;
			std::string				_surname;
			std::string				_name;
			std::string				_address;
			std::string				_postalCode;
			std::string				_city;
			std::string				_phone;
			std::string				_email;
			boost::optional<bool>	_authorizedLogin;
			boost::optional<bool>	_autoResaActivated;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setUserToDelete(boost::shared_ptr<const security::User> user);
		};
	}
}

#endif // SYNTHESE_ReservationUserMergeAction_H__
