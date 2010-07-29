
/** DisplayScreenPhysicalStopFunction class implementation.
	@file DisplayScreenPhysicalStopFunction.cpp
	@author Hugues Romain
	@date 2008

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

#include "PhysicalStop.h"
#include "StopArea.hpp"
#include "Interface.h"
#include "RequestException.h"
#include "Request.h"
#include "DisplayScreenPhysicalStopFunction.h"
#include "DeparturesTableInterfacePage.h"
#include "DisplayScreen.h"
#include "DisplayType.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace interfaces;
	
	template<> const string util::FactorableTemplate<Function,departurestable::DisplayScreenPhysicalStopFunction>::FACTORY_KEY("td");
	
	namespace departurestable
	{
		const string DisplayScreenPhysicalStopFunction::PARAMETER_INTERFACE_ID("i");
		const string DisplayScreenPhysicalStopFunction::PARAMETER_OPERATOR_CODE("oc");
		
		ParametersMap DisplayScreenPhysicalStopFunction::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void DisplayScreenPhysicalStopFunction::_setFromParametersMap(const ParametersMap& map)
		{
			string oc(map.get<string>(PARAMETER_OPERATOR_CODE));
			shared_ptr<const StopArea> place(
				Env::GetOfficialEnv().get<StopArea>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
			)	);

			DisplayScreen* screen(new DisplayScreen);
			_type.reset(new DisplayType);
			_type->setRowNumber(10);
			_type->setDisplayInterface(
				Env::GetOfficialEnv().get<Interface>(map.get<RegistryKeyType>(PARAMETER_INTERFACE_ID)).get()
			);

			screen->setLocalization(place.get());
			screen->setAllPhysicalStopsDisplayed(false);
			const ArrivalDepartureTableGenerator::PhysicalStops& stops(place->getPhysicalStops());
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, stops)
			{
				if(it.second->getCodeBySource() == oc)
					screen->addPhysicalStop(it.second);
			}
			screen->setType(_type.get());
			_screen.reset(screen);
		}

		void DisplayScreenPhysicalStopFunction::run( std::ostream& stream, const Request& request ) const
		{
			ptime date(second_clock::local_time());
			_screen->display(stream, date, &request);
		}



		bool DisplayScreenPhysicalStopFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string DisplayScreenPhysicalStopFunction::getOutputMimeType() const
		{
			return
				(	_type &&
					_type->getDisplayInterface() &&
					_type->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
				) ?
				_type->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->getMimeType() :
				"text/plain";
		}
	}
}
