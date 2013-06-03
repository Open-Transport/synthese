
//////////////////////////////////////////////////////////////////////////
/// MailingListSendAction class implementation.
/// @file MailingListSendAction.cpp
/// @author hromain
/// @date 2013
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

#include "MailingListSendAction.hpp"

#include "ActionException.h"
#include "Alarm.h"
#include "MailingList.hpp"
#include "MessageAlternative.hpp"
#include "ParametersMap.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, messages::MailingListSendAction>::FACTORY_KEY = "mailing_list_send";

	namespace messages
	{
		const string MailingListSendAction::PARAMETER_MAILING_LIST_ID = Action_PARAMETER_PREFIX + "_mailing_list_id";
		const string MailingListSendAction::PARAMETER_MESSAGE_ID = Action_PARAMETER_PREFIX + "_message_id";
		
		
		
		ParametersMap MailingListSendAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}
		
		
		
		void MailingListSendAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_list = Env::GetOfficialEnv().get<MailingList>(
					map.get<RegistryKeyType>(PARAMETER_MAILING_LIST_ID)
				).get();
				_message = Env::GetOfficialEnv().get<Alarm>(
					map.get<RegistryKeyType>(PARAMETER_MESSAGE_ID)
				).get();
			}
			catch(ObjectNotFoundException<MailingList>&)
			{
				throw ActionException("No such mailing list");
			}
			catch(ObjectNotFoundException<Alarm>&)
			{
				throw ActionException("No such message");
			}
		}
		
		
		
		void MailingListSendAction::run(
			Request& request
		){
			string message;
			if(_list->get<MessageType>())
			{
				Alarm::MessageAlternatives::const_iterator it(
					_message->getMessageAlternatives().find(&*_list->get<MessageType>())
				);
				if(it == _message->getMessageAlternatives().end())
				{
					throw ActionException("Malformed message");
				}
				message = it->second->get<Content>();
			}
			else
			{
				message = _message->getLongMessage();
			}

			_list->send(
				_message->getShortMessage(),
				message
			);
		}
		
		
		
		bool MailingListSendAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		MailingListSendAction::MailingListSendAction():
			_list(NULL),
			_message(NULL)
		{
		}
}	}

