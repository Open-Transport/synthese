
//////////////////////////////////////////////////////////////////////////
/// ForumMessagePostAction class implementation.
/// @file ForumMessagePostAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ForumMessagePostAction.hpp"
#include "ForumMessageTableSync.hpp"
#include "ForumTopicTableSync.hpp"
#include "Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, forum::ForumMessagePostAction>::FACTORY_KEY("ForumMessagePostAction");
	}

	namespace forum
	{
		const string ForumMessagePostAction::PARAMETER_MESSAGE = Action_PARAMETER_PREFIX + "co";
		const string ForumMessagePostAction::PARAMETER_USER_NAME = Action_PARAMETER_PREFIX + "un";
		const string ForumMessagePostAction::PARAMETER_USER_EMAIL = Action_PARAMETER_PREFIX + "um";
		const string ForumMessagePostAction::PARAMETER_TOPIC_ID = Action_PARAMETER_PREFIX + "ti";



		ParametersMap ForumMessagePostAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_MESSAGE, _content);
			map.insert(PARAMETER_USER_NAME, _userName);
			map.insert(PARAMETER_USER_EMAIL, _userEMail);
			if(_topic.get())
			{
				map.insert(PARAMETER_TOPIC_ID, _topic->getKey());
			}
			return map;
		}



		void ForumMessagePostAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_topic = ForumTopicTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TOPIC_ID), *_env);
			}
			catch(ObjectNotFoundException<ForumTopic>&)
			{
				throw ActionException("No such topic");
			}

			_userName = map.get<string>(PARAMETER_USER_NAME);
			if(_userName.empty())
			{
				throw ActionException("Veuillez entrer votre nom");
			}

			// User e-mail
			_userEMail = map.get<string>(PARAMETER_USER_EMAIL);
			if(_userEMail.empty())
			{
				throw ActionException("Veuillez entrer votre e-mail");
			}
			/// @todo control @ and .xx presence

			_content = map.get<string>(PARAMETER_MESSAGE);
			if(_content.empty())
			{
				throw ActionException("Veuillez entrer un contenu de message");
			}

		}



		void ForumMessagePostAction::run(
			Request& request
		){
			ForumMessage message;
			message.setTopic(_topic.get());
			message.setContent(_content);
			message.setIP(request.getIP());
			message.setUserEMail(_userEMail);
			message.setUserName(_userName);

			ForumMessageTableSync::Save(&message);

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(message.getKey());
		}



		bool ForumMessagePostAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
