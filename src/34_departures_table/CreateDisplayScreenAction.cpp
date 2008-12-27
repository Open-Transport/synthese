
/** CreateDisplayScreenAction class implementation.
	@file CreateDisplayScreenAction.cpp

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

#include "CreateDisplayScreenAction.h"

#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"
#include "30_server/QueryString.h"

#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/ConnectionPlaceTableSync.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace security;
	
	template<> const string FactorableTemplate<Action, departurestable::CreateDisplayScreenAction>::FACTORY_KEY("createdisplayscreen");

	namespace departurestable
	{
		const std::string CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string CreateDisplayScreenAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");

		ParametersMap CreateDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEMPLATE_ID, _template ? _template->getKey() : uid(0));
			map.insert(PARAMETER_LOCALIZATION_ID, _place.get() ? _place->getKey() : uid(UNKNOWN_VALUE));
			return map;
		}

		void CreateDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_TEMPLATE_ID, false, FACTORY_KEY));
			if (id > 0)
			{
				_template = DisplayScreenTableSync::Get(id, _env);
			}

			id = map.getUid(PARAMETER_LOCALIZATION_ID, true, FACTORY_KEY);
			try
			{
				_place = ConnectionPlaceTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw ActionException("Specified localization not found");
			}

			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void CreateDisplayScreenAction::run()
		{
			// Preparation
			shared_ptr<DisplayScreen> screen(new DisplayScreen);
			if (_template)
				screen->copy(_template.get());
			screen->setLocalization(_place.get());
			screen->setMaintenanceIsOnline(true);

			// Action
			DisplayScreenTableSync::Save(screen.get());

			// Request update
			_request->setObjectId(screen->getKey());

			// Log
			ArrivalDepartureTableLog::addUpdateEntry(screen.get(), "Création", _request->getUser().get());
		}

		void CreateDisplayScreenAction::setPlace( boost::shared_ptr<const PublicTransportStopZoneConnectionPlace> place )
		{
			_place = place;
		}



		bool CreateDisplayScreenAction::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_place->getKey()));
		}
	}
}
