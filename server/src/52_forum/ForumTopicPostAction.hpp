
//////////////////////////////////////////////////////////////////////////
/// ForumTopicPostAction class header.
///	@file ForumTopicPostAction.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_ForumTopicPostAction_H__
#define SYNTHESE_ForumTopicPostAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace forum
	{
		//////////////////////////////////////////////////////////////////////////
		/// 52.15 Action : ForumTopicPostAction.
		/// @ingroup m52Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		//////////////////////////////////////////////////////////////////////////
		/// Key : ForumTopicPostAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamni : id of the node of the topic</li>
		///		<li>actionParamne : node extension key of the topic</li>
		///		<li>actionParamna : topic title</li>
		///		<li>actionParamun : user name</li>
		///		<li>actionParamum : user e-mail</li>
		///		<li>actionParamco : message content</li>
		///	</ul>
		class ForumTopicPostAction:
			public util::FactorableTemplate<server::Action, ForumTopicPostAction>
		{
		public:
			static const std::string PARAMETER_NODE_ID;
			static const std::string PARAMETER_NODE_EXTENSION;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_USER_NAME;
			static const std::string PARAMETER_USER_EMAIL;
			static const std::string PARAMETER_MESSAGE;

		private:
			util::RegistryKeyType _nodeId;
			std::string _nodeExtension;
			std::string _name;
			std::string _userName;
			std::string _userEMail;
			std::string _content;

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
			ForumTopicPostAction();

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
				void setNode(util::RegistryKeyType id, const std::string& extension) { _nodeId = id; _nodeExtension = extension; }
			//@}
		};
	}
}

#endif // SYNTHESE_ForumTopicPostAction_H__
