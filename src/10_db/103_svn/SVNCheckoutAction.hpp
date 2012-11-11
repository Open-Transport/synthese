
//////////////////////////////////////////////////////////////////////////
/// SVNCheckoutAction class header.
///	@file SVNCheckoutAction.hpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_SVNCheckoutAction_H__
#define SYNTHESE_SVNCheckoutAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include "SVNRepository.hpp"

namespace synthese
{
	namespace db
	{
		namespace svn
		{
			//////////////////////////////////////////////////////////////////////////
			/// 103.15 Action : SVNCheckoutAction.
			/// @ingroup m103Actions refActions
			///	@author Hugues Romain
			///	@date 2012
			/// @since 3.4.0
			//////////////////////////////////////////////////////////////////////////
			/// Key : SVNCheckoutAction
			///
			/// Parameters :
			///	<dl>
			///	<dt>actionParamid</dt><dd>id of the object to update</dd>
			///	</dl>
			class SVNCheckoutAction:
				public util::FactorableTemplate<server::Action, SVNCheckoutAction>
			{
			public:
				static const std::string PARAMETER_REPO_URL;
				static const std::string PARAMETER_USER;
				static const std::string PARAMETER_PASSWORD;

			private:
				std::string _user;
				std::string _password;
				SVNRepository _repo;

			protected:
				//////////////////////////////////////////////////////////////////////////
				/// Generates a generic parameters map from the action parameters.
				/// @return The generated parameters map
				util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads the parameters of the action on a generic parameters map.
				/// @param map Parameters map to interpret
				/// @exception ActionException Occurs when some parameters are missing or incorrect.
				void _setFromParametersMap(const util::ParametersMap& map);

			public:
				//////////////////////////////////////////////////////////////////////////
				/// The action execution code.
				/// @param request the request which has launched the action
				void run(server::Request& request);
				


				//////////////////////////////////////////////////////////////////////////
				/// Tests if the action can be launched in the current session.
				/// @param session the current session
				/// @return true if the action can be launched in the current session
				virtual bool isAuthorized(const server::Session* session) const;



				//! @name Setters
				//@{
					void setUser(const std::string& value){ _user = value; }
					void setPassword(const std::string& value){ _password = value; }
					void setRepo(const SVNRepository& value){ _repo = value; }
				//@}
			};
}	}	}

#endif // SYNTHESE_SVNCheckoutAction_H__
