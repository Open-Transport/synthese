
/** DisplayScreenContentFunction class implementation.
	@file DisplayScreenContentFunction.cpp

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

#include "DisplayScreenContentFunction.h"
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
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;
	using namespace interfaces;
	using namespace db;

	template<> const string util::FactorableTemplate<Function,departurestable::DisplayScreenContentFunction>::FACTORY_KEY("tdg");

	namespace departurestable
	{
		const std::string DisplayScreenContentFunction::PARAMETER_DATE = "date";
		const std::string DisplayScreenContentFunction::PARAMETER_TB = "tb";
		const std::string DisplayScreenContentFunction::PARAMETER_INTERFACE_ID("i");
		const string DisplayScreenContentFunction::PARAMETER_MAC_ADDRESS("m");

		ParametersMap DisplayScreenContentFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_date && _date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_screen.get()) map.insert(Request::PARAMETER_OBJECT_ID, _screen->getKey());
			return map;
		}

		void DisplayScreenContentFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				optional<RegistryKeyType> id;
				// Screen
				id = map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID);
				if (!id)
				{
					id = map.getOptional<RegistryKeyType>(PARAMETER_TB);
				}
				if(id)
				{
					if (decodeTableId(*id) == ConnectionPlaceTableSync::TABLE.ID)
					{
						DisplayScreen* screen(new DisplayScreen);
						_type.reset(new DisplayType);
						_type->setRowNumber(10);
						_type->setDisplayInterface(Env::GetOfficialEnv().getRegistry<Interface>().get(map.get<RegistryKeyType>(PARAMETER_INTERFACE_ID)).get());
						screen->setLocalization(Env::GetOfficialEnv().getRegistry<PublicTransportStopZoneConnectionPlace>().get(*id).get());
						screen->setAllPhysicalStopsDisplayed(true);					
						screen->setType(_type.get());
						_screen.reset(screen);
					}
					else if (decodeTableId(*id) == DisplayScreenTableSync::TABLE.ID)
					{
						_screen = DisplayScreenTableSync::Get(*id, *_env);
					}
					else
						throw RequestException("Not a display screen nor a connection place");
				}
				else
				{
					string macAddress(map.get<string>(PARAMETER_MAC_ADDRESS));
					_screen = DisplayScreenTableSync::GetByMACAddress(*_env, macAddress, UP_LINKS_LOAD_LEVEL);
				}

				// Date
				if(!map.getDefault<string>(PARAMETER_DATE).empty())
				{
					_date = time_from_string(map.get<string>(PARAMETER_DATE));
				}
			}
			catch (ObjectNotFoundException<DisplayScreen> e)
			{
				throw RequestException("Display screen not found "+ e.getMessage());
			}
		}

		void DisplayScreenContentFunction::run( std::ostream& stream, const Request& request ) const
		{
			_screen->display(stream, _date ? *_date : second_clock::local_time(), &request);
		}



		bool DisplayScreenContentFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string DisplayScreenContentFunction::getOutputMimeType() const
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

		void DisplayScreenContentFunction::setScreen(
			shared_ptr<const DisplayScreen> value
		){
			_screen = value;
		}
	}
}
