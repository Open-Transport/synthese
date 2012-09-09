////////////////////////////////////////////////////////////////////////////////
/// ResaLogEntryUpdateAction class implementation.
///	@file ResaLogEntryUpdateAction.cpp
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

#include "ResaLogEntryUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "ResaRight.h"
#include "Session.h"
#include "User.h"
#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace dblog;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ResaLogEntryUpdateAction>::FACTORY_KEY("ResaLogEntryUpdateAction");
	}

	namespace resa
	{
		const string ResaLogEntryUpdateAction::PARAMETER_LOG_ENTRY_ID = Action_PARAMETER_PREFIX + "li";
		const string ResaLogEntryUpdateAction::PARAMETER_CALL_TYPE(Action_PARAMETER_PREFIX + "ct");
		const string ResaLogEntryUpdateAction::PARAMETER_TEXT = Action_PARAMETER_PREFIX + "te";
		const string ResaLogEntryUpdateAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";



		ResaLogEntryUpdateAction::ResaLogEntryUpdateAction()
			: util::FactorableTemplate<Action, ResaLogEntryUpdateAction>()
		{
		}



		ParametersMap ResaLogEntryUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_entry.get())
				map.insert(PARAMETER_LOG_ENTRY_ID, _entry->getKey());
			return map;
		}



		void ResaLogEntryUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setEntryId(map.get<RegistryKeyType>(PARAMETER_LOG_ENTRY_ID));
			optional<int> type(map.getOptional<int>(PARAMETER_TYPE));
			if (type)
			{
				_type = static_cast<ResaDBLog::_EntryType>(*type);
				_text = map.getDefault<string>(PARAMETER_TEXT);
				if (_text.empty())
					throw ActionException("Vous devez saisir un texte complémentaire");
			}
			_callType = static_cast<ResaDBLog::_EntryType>(map.get<int>(PARAMETER_CALL_TYPE));
		}



		void ResaLogEntryUpdateAction::run(Request& request)
		{
			// Update the call status if necessary
			DBLogEntry::Content content(_entry->getContent());
			content[ResaDBLog::COL_TYPE] = lexical_cast<string>(_callType);
			_entry->setContent(content);
			DBLogEntryTableSync::Save(_entry.get());

			// Create the additional information if necessary
			if(_type)
			{
				ResaDBLog::AddCallInformationEntry(
					*_entry,
					*_type,
					_text,
					*request.getUser()
				);
			}
		}



		void ResaLogEntryUpdateAction::setEntryId( RegistryKeyType id )
		{
			try
			{
				_entry = DBLogEntryTableSync::GetEditable(id, *_env);
				if (_entry->getLogKey() != ResaDBLog::FACTORY_KEY)
					throw ActionException("Bad entry : does not belong to resa log");
			}
			catch (...)
			{
				throw ActionException("No such log entry");
			}
		}



		bool ResaLogEntryUpdateAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE);
		}
	}
}
