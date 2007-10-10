
/** DisplayTypeRemoveAction class implementation.
	@file DisplayTypeRemoveAction.cpp

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

#include "DisplayTypeRemoveAction.h"

#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;

	template<> const string FactorableTemplate<Action, departurestable::DisplayTypeRemoveAction>::FACTORY_KEY("dtra");

	namespace departurestable
	{
		 const string DisplayTypeRemoveAction::PARAMETER_TYPE_ID(Action_PARAMETER_PREFIX + "ti");


		ParametersMap DisplayTypeRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_type.get())
				map.insert(PARAMETER_TYPE_ID, _type->getKey());
			return map;
		}

		void DisplayTypeRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_TYPE_ID, true, FACTORY_KEY));
			try
			{
				_type = DisplayTypeTableSync::Get(id);
			}
			catch(...)
			{
				throw ActionException("Specified type not found");
			}
				
			vector<shared_ptr<DisplayScreen> > v = DisplayScreenTableSync::search(
				RightsOfSameClassMap()
				, true
				, UNKNOWN_RIGHT_LEVEL
				, UNKNOWN_VALUE
				, UNKNOWN_VALUE
				, UNKNOWN_VALUE
				, _type->getKey()
				, std::string()
				, std::string()
				, std::string()
				, UNKNOWN_VALUE
				, UNKNOWN_VALUE
				, 0
				, 1
				);
			if (!v.empty())
				throw ActionException("Ce type d'afficheur ne peut être supprimé car il est utilisé par au moins un afficheur.");
		}

		void DisplayTypeRemoveAction::run()
		{
			DisplayTypeTableSync::remove(_type->getKey());

			ArrivalDepartureTableLog::addDeleteTypeEntry(_type.get(), _request->getUser().get());
		}

		void DisplayTypeRemoveAction::setType( boost::shared_ptr<const DisplayType> type )
		{
			_type = type;
		}
	}
}
