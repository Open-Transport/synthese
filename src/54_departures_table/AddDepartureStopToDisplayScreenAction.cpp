
/** AddDepartureStopToDisplayScreenAction class implementation.
	@file AddDepartureStopToDisplayScreenAction.cpp

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

#include "AddDepartureStopToDisplayScreenAction.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayScreenTableSync.h"
#include "PhysicalStopTableSync.h"
#include "Conversion.h"
#include "ArrivalDepartureTableLog.h"
#include "PhysicalStop.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace db;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<Action, departurestable::AddDepartureStopToDisplayScreenAction>::FACTORY_KEY("adstdsa");
	}
	
	namespace departurestable
	{
		const string AddDepartureStopToDisplayScreenAction::PARAMETER_STOP = Action_PARAMETER_PREFIX + "sto";


		ParametersMap AddDepartureStopToDisplayScreenAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP, _stop->getKey());
			}
			return map;
		}

		void AddDepartureStopToDisplayScreenAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(_request->getObjectId(), _env);
				setStopId(map.getUid(PARAMETER_STOP, true, FACTORY_KEY));
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw ActionException("Display screen not found");
			}
		}



		void AddDepartureStopToDisplayScreenAction::run()
		{
			// Preparation
			_screen->addPhysicalStop(_stop.get());
			
			// Log
			ArrivalDepartureTableLog::addUpdateEntry(
				_screen.get(),
				"Ajout de l'arrêt de départ "+ _stop->getOperatorCode() +"/"+ _stop->getName(),
				_request->getUser().get()
			);
			
			// Action
			DisplayScreenTableSync::Save(_screen.get());
		}



		bool AddDepartureStopToDisplayScreenAction::_isAuthorized(
		) const {
			return _request->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
		
		void AddDepartureStopToDisplayScreenAction::setStopId(
			RegistryKeyType id
		){
			try
			{
				_stop = PhysicalStopTableSync::Get(id, _env, UP_LINKS_LOAD_LEVEL);
			}
			catch (ObjectNotFoundException<PhysicalStop>& e)
			{
				throw ActionException("Departure physical stop", id, FACTORY_KEY, e);
			}
		}
	}
}
