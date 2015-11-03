
////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayMaintenanceAction class implementation.
///	@file UpdateDisplayMaintenanceAction.cpp
///	@author Hugues Romain
///	@date 2008-12-19 3:15
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

#include "UpdateDisplayMaintenanceAction.h"

#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "DisplayScreenTableSync.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "DisplayMaintenanceLog.h"
#include "Request.h"
#include "DisplayMaintenanceRight.h"
#include "StopArea.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departure_boards::UpdateDisplayMaintenanceAction>::FACTORY_KEY("udm");

	namespace departure_boards
	{
		const string UpdateDisplayMaintenanceAction::PARAMETER_SCREEN_ID = Action_PARAMETER_PREFIX + "si";
		const string UpdateDisplayMaintenanceAction::PARAMETER_ONLINE = Action_PARAMETER_PREFIX + "oli";
		const string UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE = Action_PARAMETER_PREFIX + "mes";

		ParametersMap UpdateDisplayMaintenanceAction::getParametersMap() const
		{
			ParametersMap map;
			if (_displayScreen.get())
			{
				map.insert(PARAMETER_SCREEN_ID, _displayScreen->getKey());
			}
			return map;
		}

		void UpdateDisplayMaintenanceAction::_setFromParametersMap(const ParametersMap& map)
		{
			setScreenId(map.get<RegistryKeyType>(PARAMETER_SCREEN_ID));
			_online = map.get<bool>(PARAMETER_ONLINE);
			_message = map.get<string>(PARAMETER_MESSAGE);
		}

		void UpdateDisplayMaintenanceAction::run(Request& request)
		{
			// Online
			DisplayMaintenanceLog::AddAdminEntry(
				*_displayScreen,
				*request.getUser(),
				"Etat en ligne de l'afficheur",
				lexical_cast<string>(_displayScreen->get<MaintenanceIsOnline>()),
				lexical_cast<string>(_online),
				_online ? DB_LOG_INFO : DB_LOG_WARNING
			);
			_displayScreen->set<MaintenanceIsOnline>(_online);

			// Message
			DisplayMaintenanceLog::AddAdminEntry(
				*_displayScreen,
				*request.getUser(),
				"Message de maintenance",
				string(),
				_message
			);
			_displayScreen->set<MaintenanceMessage>(_message);

			// Saving
			DisplayScreenTableSync::Save(_displayScreen.get());
		}



		void UpdateDisplayMaintenanceAction::setScreenId(
			util::RegistryKeyType id
		){
			try
			{
				_displayScreen = DisplayScreenTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Specified display screen not found");
			}
		}



		bool UpdateDisplayMaintenanceAction::isAuthorized(const Session* session
		) const {
			if (_displayScreen->getLocation() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_displayScreen->getLocation()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(WRITE);
			}
		}
	}
}
