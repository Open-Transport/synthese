////////////////////////////////////////////////////////////////////////////////
/// ResaLogEntryUpdateAction class implementation.
///	@file ResaLogEntryUpdateAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ResaLogEntryUpdateAction.h"
#include "ResaRight.h"
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
			setEntryId(map.getUid(PARAMETER_LOG_ENTRY_ID, true, FACTORY_KEY));
			int type(map.getInt(PARAMETER_TYPE, false, FACTORY_KEY));
			if (type != UNKNOWN_VALUE)
			{
				_type = static_cast<ResaDBLog::_EntryType>(type);
				_text = map.getString(PARAMETER_TEXT, false, FACTORY_KEY);
				if (_text.empty())
					throw ActionException("Vous devez saisir un texte compl�mentaire");
			}
			_callType = static_cast<ResaDBLog::_EntryType>(map.getInt(PARAMETER_CALL_TYPE, true, FACTORY_KEY));
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



		void ResaLogEntryUpdateAction::setEntryId( uid id )
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



		bool ResaLogEntryUpdateAction::isAuthorized(const Profile& profile
		) const {
			return profile.isAuthorized<ResaRight>(WRITE);
		}
	}
}
