
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
#include "PhysicalStopTableSync.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableInterfacePage.h"
#include "StopAreaTableSync.hpp"

#include "Interface.h"

#include "Env.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace interfaces;
	using namespace db;
	using namespace pt;
	using namespace departurestable;

	template<> const string util::FactorableTemplate<DisplayScreenContentFunction::_FunctionWithSite,DisplayScreenContentFunction>::FACTORY_KEY("tdg");

	namespace departurestable
	{
		const string DisplayScreenContentFunction::PARAMETER_DATE = "date";
		const string DisplayScreenContentFunction::PARAMETER_TB = "tb";
		const string DisplayScreenContentFunction::PARAMETER_INTERFACE_ID("i");
		const string DisplayScreenContentFunction::PARAMETER_MAC_ADDRESS("m");
		const string DisplayScreenContentFunction::PARAMETER_OPERATOR_CODE("oc");
		const string DisplayScreenContentFunction::PARAMETER_ROWS_NUMBER("rn");
		const string DisplayScreenContentFunction::PARAMETER_CITY_NAME("cn");
		const string DisplayScreenContentFunction::PARAMETER_STOP_NAME("sn");
		
		
		
		ParametersMap DisplayScreenContentFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());
			if(_date && _date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_screen.get()) map.insert(Request::PARAMETER_OBJECT_ID, _screen->getKey());
			return map;
		}



		void DisplayScreenContentFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				RegistryKeyType id(
					map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
				);
				if (!id)
				{
					id = map.getDefault<RegistryKeyType>(PARAMETER_TB, 0);
				}
				
				// Way 1 : pre-configured display screen
				
				// 1.1 by id
				if (decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					_screen = DisplayScreenTableSync::Get(id, *_env);
				}
				
				// 1.2 by mac address
				else if(!map.getDefault<string>(PARAMETER_MAC_ADDRESS).empty())
				{
					_screen = DisplayScreenTableSync::GetByMACAddress(
						*_env,
						map.get<string>(PARAMETER_MAC_ADDRESS),
						UP_LINKS_LOAD_LEVEL
					);
				}
				else
				{
					_FunctionWithSite::_setFromParametersMap(map);

					DisplayScreen* screen(new DisplayScreen);
					_type.reset(new DisplayType);
					_type->setRowNumber(map.getDefault<size_t>(PARAMETER_ROWS_NUMBER, 10));

					_type->setDisplayInterface(Env::GetOfficialEnv().getRegistry<Interface>().get(map.get<RegistryKeyType>(PARAMETER_INTERFACE_ID)).get());
					screen->setType(_type.get());

					// Way 3 : physical stop
					
					// 3.1 by id
					if(decodeTableId(id) == PhysicalStopTableSync::TABLE.ID)
					{
						shared_ptr<const StopPoint> stop(
							Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
						);

						screen->setLocalization(stop->getConnectionPlace());
						screen->setAllPhysicalStopsDisplayed(false);
						screen->addPhysicalStop(stop.get());
					}
					
					// 3.2 by operator code
					else if(!map.getDefault<string>(PARAMETER_OPERATOR_CODE).empty())
					{
						
						string oc(map.get<string>(PARAMETER_OPERATOR_CODE));
						shared_ptr<const StopArea> place(
							Env::GetOfficialEnv().get<StopArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
						);
						screen->setLocalization(place.get());
						screen->setAllPhysicalStopsDisplayed(false);
						const ArrivalDepartureTableGenerator::PhysicalStops& stops(place->getPhysicalStops());
						BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, stops)
						{
							if(it.second->getCodeBySource() == oc)
								screen->addPhysicalStop(it.second);
						}
					}

					// Way 2 : connection place
				
					// 2.1 by id
					else if(decodeTableId(id) == StopAreaTableSync::TABLE.ID)
					{
						screen->setLocalization(Env::GetOfficialEnv().getRegistry<StopArea>().get(id).get());
						screen->setAllPhysicalStopsDisplayed(true);
					}
					
					// 2.2 by name
					else if (!map.getDefault<string>(PARAMETER_CITY_NAME).empty() && !map.getDefault<string>(PARAMETER_STOP_NAME).empty())
					{
						screen->setAllPhysicalStopsDisplayed(true);
						
					}


					else // Failure
					{
						throw RequestException("Not a display screen nor a connection place");
					}


					_screen.reset(screen);
				}
				
				// Date
				if(!map.getDefault<string>(PARAMETER_DATE).empty() && map.getDefault<string>(PARAMETER_DATE) != "A")
				{
					_date = time_from_string(map.get<string>(PARAMETER_DATE));
				}
			}
			catch (ObjectNotFoundException<DisplayScreen> e)
			{
				throw RequestException("Display screen not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<StopArea>& e)
			{
				throw RequestException("Connection place not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<StopPoint>& e)
			{
				throw RequestException("Physical stop not found "+ e.getMessage());
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
