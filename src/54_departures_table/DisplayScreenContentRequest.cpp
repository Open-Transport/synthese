
/** DisplayScreenContentRequest class implementation.
	@file DisplayScreenContentRequest.cpp

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

#include <sstream>

#include "Conversion.h"

#include "RequestException.h"
#include "Request.h"

#include "DisplayScreenContentRequest.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableInterfacePage.h"
#include "ConnectionPlaceTableSync.h"

#include "Interface.h"

#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace time;
	using namespace env;
	using namespace interfaces;
	using namespace db;

	template<> const string util::FactorableTemplate<Function,departurestable::DisplayScreenContentRequest>::FACTORY_KEY("tdg");

	namespace departurestable
	{
		const std::string DisplayScreenContentRequest::PARAMETER_DATE = "date";
		const std::string DisplayScreenContentRequest::PARAMETER_TB = "tb";
		const std::string DisplayScreenContentRequest::PARAMETER_INTERFACE_ID("i");

		ParametersMap DisplayScreenContentRequest::_getParametersMap() const
		{
			ParametersMap map;
			if(_date) map.insert(PARAMETER_DATE, *_date);
			if(_screen.get()) map.insert(Request::PARAMETER_OBJECT_ID, _screen->getKey());
			return map;
		}

		void DisplayScreenContentRequest::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				RegistryKeyType id;
				// Screen
				if (map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
				{
					id = map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID);
				}
				else
				{
					id = map.get<RegistryKeyType>(PARAMETER_TB);
				}

				if (decodeTableId(id) == ConnectionPlaceTableSync::TABLE.ID)
				{
					DisplayScreen* screen(new DisplayScreen);
					_type.reset(new DisplayType);
					_type->setRowNumber(10);
					_type->setDisplayInterface(Env::GetOfficialEnv().getRegistry<Interface>().get(map.getUid(PARAMETER_INTERFACE_ID, true, FACTORY_KEY)).get());
					screen->setLocalization(Env::GetOfficialEnv().getRegistry<PublicTransportStopZoneConnectionPlace>().get(id).get());
					screen->setAllPhysicalStopsDisplayed(true);					
					screen->setType(_type.get());
					_screen.reset(screen);
				}
				else if (decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					_screen = DisplayScreenTableSync::Get(id, *_env);
				}
				else
					throw RequestException("Not a display screen nor a connection place");

				// Date
				if(map.getOptional<string>(PARAMETER_DATE))
				{
					_date = map.getDateTime(PARAMETER_DATE, false, FACTORY_KEY);
				}
			}
			catch (ObjectNotFoundException<DisplayScreen> e)
			{
				throw RequestException("Display screen not found "+ e.getMessage());
			}
		}

		void DisplayScreenContentRequest::_run( std::ostream& stream ) const
		{
			_screen->display(stream, _date ? *_date : DateTime(TIME_CURRENT));
		}



		bool DisplayScreenContentRequest::_isAuthorized(
		) const {
			return true;
		}

		std::string DisplayScreenContentRequest::getOutputMimeType() const
		{
			return
				(	_screen.get() &&
					_screen->getType() &&
					_screen->getType()->getDisplayInterface() &&
					_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
				) ?
				_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->getMimeType() :
				"text/plain"
			;
		}

		void DisplayScreenContentRequest::setScreen(
			shared_ptr<const DisplayScreen> value
		){
			_screen = value;
		}
	}
}
