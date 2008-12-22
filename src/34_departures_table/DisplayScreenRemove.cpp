
/** DisplayScreenRemove class implementation.
	@file DisplayScreenRemove.cpp

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

#include "DisplayScreenRemove.h"

#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "12_security/Right.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, departurestable::DisplayScreenRemove>::FACTORY_KEY("dsra");

	namespace departurestable
	{
		const string DisplayScreenRemove::PARAMETER_DISPLAY_SCREEN_ID(Action_PARAMETER_PREFIX + "dsi");


		ParametersMap DisplayScreenRemove::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_SCREEN_ID, _displayScreen->getKey());
			return map;
		}

		void DisplayScreenRemove::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_DISPLAY_SCREEN_ID, true, FACTORY_KEY));
			
			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw ActionException("Specified display screen not found");
			}
		}

		void DisplayScreenRemove::run()
		{
			DisplayScreenTableSync::Remove(_displayScreen->getKey());

			ArrivalDepartureTableLog::addRemoveEntry(_displayScreen.get(), _request->getUser().get());
		}

		bool DisplayScreenRemove::_isAuthorized() const
		{
			return _request->isAuthorized<ArrivalDepartureTableRight>(
				DELETE_RIGHT
				, UNKNOWN_RIGHT_LEVEL
				, Conversion::ToString(_displayScreen->getKey())
				);
		}

		void DisplayScreenRemove::setDisplayScreen( boost::shared_ptr<const DisplayScreen> screen )
		{
			_displayScreen = screen;
		}
	}
}
