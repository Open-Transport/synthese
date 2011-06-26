
//////////////////////////////////////////////////////////////////////////
/// SimpleMessageCreationAction class header.
///	@file SimpleMessageCreationAction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_SimpleMessageCreationAction_H__
#define SYNTHESE_SimpleMessageCreationAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "MessagesTypes.h"

namespace synthese
{
	namespace messages
	{
		//////////////////////////////////////////////////////////////////////////
		/// 17.15 Action : SimpleMessageCreationAction.
		/// @ingroup m17Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : SimpleMessageCreationAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamc</dt><dd>content of the message</dd>
		///	<dt>actionParamr</dt><dd>if of the recipient of the message</dd>
		///	<dt>actionParaml</dt><dd>level of the message</dd>
		///	</dl>
		///
		/// This action does not control the validity of the recipient id.
		class SimpleMessageCreationAction:
			public util::FactorableTemplate<server::Action, SimpleMessageCreationAction>
		{
		public:
			static const std::string PARAMETER_CONTENT;
			static const std::string PARAMETER_RECIPIENT_ID;
			static const std::string PARAMETER_LEVEL;

		private:
			std::string _content;
			util::RegistryKeyType _recipientId;
			AlarmLevel _level;

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
			SimpleMessageCreationAction();

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
				// void setObject(boost::shared_ptr<Object> value) { _object = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_SimpleMessageCreationAction_H__
