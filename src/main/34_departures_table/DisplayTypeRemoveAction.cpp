
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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "34_departures_table/DisplayTypeRemoveAction.h"
#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace departurestable
	{
		 const string DisplayTypeRemoveAction::PARAMETER_TYPE_ID(Action_PARAMETER_PREFIX + "ti");


		ParametersMap DisplayTypeRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_type.get())
				map.insert(make_pair(PARAMETER_TYPE_ID, Conversion::ToString(_type->getKey())));
			return map;
		}

		void DisplayTypeRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_TYPE_ID);
			if (it == map.end())
				throw ActionException("Type not specified");
			
			try
			{
				_type = DisplayTypeTableSync::get(Conversion::ToLongLong(it->second));
				
				vector<shared_ptr<DisplayScreen> > v = DisplayScreenTableSync::search(
					UNKNOWN_VALUE
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
				if (v.empty())
					throw ActionException("Ce type d'afficheur ne peut être supprimé car il est utilisé par au moins un afficheur.");
			}
			catch(...)
			{
				throw ActionException("Specified type not found");
			}
		}

		void DisplayTypeRemoveAction::run()
		{
			DisplayTypeTableSync::remove(_type->getKey());

			ArrivalDepartureTableLog::addDeleteTypeEntry(_type, _request->getUser());
		}

		void DisplayTypeRemoveAction::setType( boost::shared_ptr<const DisplayType> type )
		{
			_type = type;
		}
	}
}
