
//////////////////////////////////////////////////////////////////////////
/// SVNWorkingCopyCreateAction class header.
///	@file SVNWorkingCopyCreateAction.hpp
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

#ifndef SYNTHESE_SVNWorkingCopyCreateAction_H__
#define SYNTHESE_SVNWorkingCopyCreateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include "SVNRepository.hpp"

namespace synthese
{
	class ObjectBase;

	namespace db
	{
		namespace svn
		{
			//////////////////////////////////////////////////////////////////////////
			/// 103.15 Action : SVNWorkingCopyCreateAction.
			/// @ingroup m103Actions refActions
			///	@author Hugues Romain
			///	@date 2012
			/// @since 3.4.0
			//////////////////////////////////////////////////////////////////////////
			/// Key : SVNWorkingCopyCreateAction
			///
			/// Parameters :
			///	<dl>
			///	<dt>actionParamid</dt><dd>id of the object to update</dd>
			///	</dl>
			class SVNWorkingCopyCreateAction:
				public util::FactorableTemplate<server::Action, SVNWorkingCopyCreateAction>
			{
			public:
				static const std::string PARAMETER_USER;
				static const std::string PARAMETER_PASSWORD;
				static const std::string PARAMETER_OBJECT_ID;
				static const std::string PARAMETER_REPO_URL;
				static const std::string PARAMETER_NO_COMMIT;

			private:
				boost::shared_ptr<ObjectBase> _object;
				std::string _user;
				std::string _password;
				SVNRepository _repo;
				bool _noCommit;

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
					void setObject(boost::shared_ptr<ObjectBase> value) { _object = value; }
				//@}
			};
}	}	}

#endif // SYNTHESE_SVNWorkingCopyCreateAction_H__

