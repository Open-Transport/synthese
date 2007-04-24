
/** DisplayScreenUpdateLocalizationAction class implementation.
	@file DisplayScreenUpdateLocalizationAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "30_server/ActionException.h"

#include "34_departures_table/DisplayScreenUpdateLocalizationAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/BroadcastPointTableSync.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include "30_server/Request.h"

#include "15_env/ConnectionPlaceTableSync.h"

#include "02_db/DBEmptyResultException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace db;
	
	namespace departurestable
	{
		const std::string DisplayScreenUpdateLocalizationAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "pi";
		const std::string DisplayScreenUpdateLocalizationAction::PARAMETER_LOCALIZATION_ID = Action_PARAMETER_PREFIX + "li";
		const std::string DisplayScreenUpdateLocalizationAction::PARAMETER_LOCALIZATION_COMMENT = Action_PARAMETER_PREFIX + "lc";


		ParametersMap DisplayScreenUpdateLocalizationAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void DisplayScreenUpdateLocalizationAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;
			shared_ptr<const ConnectionPlace> place;

			_screen = DisplayScreenTableSync::get(_request->getObjectId());

			it= map.find(PARAMETER_LOCALIZATION_COMMENT);
			if (it == map.end())
				throw ActionException("Localization comment not specified");
			_localizationComment = it->second;

			it = map.find(PARAMETER_PLACE_ID);
			if (it != map.end())
			{
				try
				{
					place = ConnectionPlaceTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (DBEmptyResultException<ConnectionPlace>)
				{
					throw ActionException("Specified connection place not found");
				}
			}

			it = map.find(PARAMETER_LOCALIZATION_ID);
			if (it != map.end())
			{
				try
				{
					_localization = BroadcastPointTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (DBEmptyResultException<BroadcastPoint>)
				{
					throw ActionException("Specified broadcast point not found");
				}
				if (_localization->getConnectionPlace() != place)
				{
					vector<shared_ptr<BroadcastPoint> > result = BroadcastPointTableSync::search(place, 0, 1);
					if (result.empty())
						throw ActionException("Corrupted data on Display Screen Localization Update Action");
					_localization = result.front();
				}
			}
		}

		void DisplayScreenUpdateLocalizationAction::run()
		{
			string oldLocalization = _screen->getFullName();
			_screen->setLocalizationComment(_localizationComment);
			if (_localization.get())
				_screen->setLocalization(_localization);

			// Log
			shared_ptr<ArrivalDepartureTableLog> log = Factory<dblog::DBLog>::create<ArrivalDepartureTableLog>();
			log->addUpdateEntry(_screen, "Mise à jour localisation " + oldLocalization + "=>" + _screen->getFullName(), _request->getUser());
		}
	}
}
