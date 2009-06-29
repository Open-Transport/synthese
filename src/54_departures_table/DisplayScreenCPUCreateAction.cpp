
/** DisplayScreenCPUCreateAction class implementation.
	@file DisplayScreenCPUCreateAction.cpp

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

#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DeparturesTableModule.h"
#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Request.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace env;
	using namespace util;
	using namespace db;
	using namespace security;

	template<> const string FactorableTemplate<Action, departurestable::DisplayScreenCPUCreateAction>::FACTORY_KEY("createdisplayscreenCPUAction");

	namespace departurestable
	{
		const std::string DisplayScreenCPUCreateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "pti";
		const string DisplayScreenCPUCreateAction::PARAMETER_LOCALIZATION_ID(Action_PARAMETER_PREFIX + "pli");

		ParametersMap DisplayScreenCPUCreateAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEMPLATE_ID, _template ? _template->getKey() : uid(0));
			map.insert(PARAMETER_LOCALIZATION_ID, _place.get() ? _place->getKey() : uid(UNKNOWN_VALUE));
			return map;
		}

		void DisplayScreenCPUCreateAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_TEMPLATE_ID, false, FACTORY_KEY));
			if (id > 0)
			{
				_template = DisplayScreenCPUTableSync::Get(id, *_env);
			}

			id = map.getUid(PARAMETER_LOCALIZATION_ID, true, FACTORY_KEY);
			try
			{
				_place = ConnectionPlaceTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Specified localization not found");
			}
		}

		void DisplayScreenCPUCreateAction::run()
		{
			// Preparation
			DisplayScreenCPU cpu;
			if (_template.get())
				cpu.copy(*_template);
			cpu.setPlace(_place.get());
			cpu.setIsOnline(true);

			// Action
			DisplayScreenCPUTableSync::Save(&cpu);

			// Request update
			if(_request->getObjectId() == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
			{
				_request->setObjectId(cpu.getKey());
			}

			// Log
			ArrivalDepartureTableLog::addCreateEntry(cpu, *_request->getUser());
		}

		void DisplayScreenCPUCreateAction::setPlace( boost::shared_ptr<const PublicTransportStopZoneConnectionPlace> place )
		{
			_place = place;
		}



		bool DisplayScreenCPUCreateAction::_isAuthorized(
		) const {
			return
				_place.get() ?
				_request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_place->getKey())) :
				_request->isAuthorized<ArrivalDepartureTableRight>(WRITE)
			;
		}
	}
}
