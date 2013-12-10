////////////////////////////////////////////////////////////////////////////////
/// LoginAction class header.
///	@file LoginAction.h
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

#ifndef SYNTHESE_LoginAction_H__
#define SYNTHESE_LoginAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Logs a user in the system.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Login
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m15Actions refActions
		/// @author Hugues Romain
		class LoginAction:
			public util::FactorableTemplate<server::Action, LoginAction>
		{
		public:
			static const std::string PARAMETER_LOGIN;
			static const std::string PARAMETER_PASSWORD;
			static const std::string PARAMETER_TOKEN;

		private:
			std::string _login;
			std::string _password;
			std::string _token;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Login#Request
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Login#Request
			void _setFromParametersMap(const util::ParametersMap& map);

		public:

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			void setLogin(const std::string& login);
			void setPassword(const std::string& password);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_LoginAction_H__

