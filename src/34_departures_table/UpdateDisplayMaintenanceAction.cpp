
////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayMaintenanceAction class implementation.
///	@file UpdateDisplayMaintenanceAction.cpp
///	@author Hugues Romain
///	@date 2008-12-19 3:15
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

#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayScreenTableSync.h"
#include "ActionException.h"
#include "QueryString.h"
#include "ParametersMap.h"
#include "DisplayMaintenanceLog.h"
#include "Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, departurestable::UpdateDisplayMaintenanceAction>::FACTORY_KEY("udm");

	namespace departurestable
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
			setScreenId(map.getUid(PARAMETER_SCREEN_ID, true, FACTORY_KEY));
			_online = map.getBool(PARAMETER_ONLINE, true, false, FACTORY_KEY);
			_message = map.getString(PARAMETER_MESSAGE, true, FACTORY_KEY);
		}

		void UpdateDisplayMaintenanceAction::run()
		{
			// Online
			DisplayMaintenanceLog::AddAdminEntry(
				_displayScreen.get(),
				_request->getUser().get(),
				"Etat en ligne de l'afficheur",
				Conversion::ToString(_displayScreen->getIsOnline()),
				Conversion::ToString(_online),
				_online ? DBLogEntry::DB_LOG_INFO : DBLogEntry::DB_LOG_WARNING
			);
			_displayScreen->setMaintenanceIsOnline(_online);

			// Message
			DisplayMaintenanceLog::AddAdminEntry(
				_displayScreen.get(),
				_request->getUser().get(),
				"Message de maintenance",
				string(),
				_message
			);
			_displayScreen->setMaintenanceMessage(_message);

			// Saving
			DisplayScreenTableSync::Save(_displayScreen.get());
		}



		void UpdateDisplayMaintenanceAction::setScreenId(
			util::RegistryKeyType id
		){
			try
			{
				_displayScreen = DisplayScreenTableSync::GetEditable(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Specified display screen not found");
			}
		}
	}
}
