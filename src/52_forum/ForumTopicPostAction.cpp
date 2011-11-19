
//////////////////////////////////////////////////////////////////////////
/// ForumTopicPostAction class implementation.
/// @file ForumTopicPostAction.cpp
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
#include "ForumTopicPostAction.hpp"
#include "ForumTopicTableSync.hpp"
#include "ForumMessageTableSync.hpp"
#include "Request.h"
#include "DBTransaction.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, forum::ForumTopicPostAction>::FACTORY_KEY("ForumTopicPostAction");
	}

	namespace forum
	{
		const string ForumTopicPostAction::PARAMETER_MESSAGE = Action_PARAMETER_PREFIX + "co";
		const string ForumTopicPostAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string ForumTopicPostAction::PARAMETER_NODE_EXTENSION = Action_PARAMETER_PREFIX + "ne";
		const string ForumTopicPostAction::PARAMETER_NODE_ID = Action_PARAMETER_PREFIX + "ni";
		const string ForumTopicPostAction::PARAMETER_USER_NAME = Action_PARAMETER_PREFIX + "un";
		const string ForumTopicPostAction::PARAMETER_USER_EMAIL = Action_PARAMETER_PREFIX + "um";



		ParametersMap ForumTopicPostAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NODE_ID, _nodeId);
			map.insert(PARAMETER_NODE_EXTENSION, _nodeExtension);
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_USER_NAME, _userName);
			map.insert(PARAMETER_USER_EMAIL, _userEMail);
			map.insert(PARAMETER_MESSAGE, _content);
			return map;
		}



		void ForumTopicPostAction::_setFromParametersMap(const ParametersMap& map)
		{
			_nodeId = map.get<RegistryKeyType>(PARAMETER_NODE_ID);
			_nodeExtension = map.getDefault<string>(PARAMETER_NODE_EXTENSION);
			_name = map.get<string>(PARAMETER_NAME);
			if(_name.empty())
			{
				throw ActionException("Veuillez entrer un titre de message");
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



		void ForumTopicPostAction::run(
			Request& request
		){
			ForumTopic topic;
			topic.setIP(request.getIP());
			topic.setName(_name);
			topic.setNode(_nodeId);
			topic.setNodeExtension(_nodeExtension);
			topic.setUserEMail(_userEMail);
			topic.setUserName(_userName);

			ForumMessage message;
			message.setTopic(&topic);
			message.setContent(_content);
			message.setIP(request.getIP());
			message.setUserEMail(_userEMail);
			message.setUserName(_userName);

			DBTransaction t;
			ForumTopicTableSync::Save(&topic, t);
			ForumMessageTableSync::Save(&message, t);
			t.run();

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(topic.getKey());
		}



		bool ForumTopicPostAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		ForumTopicPostAction::ForumTopicPostAction():
		_nodeId(0)
		{

		}
	}
}
