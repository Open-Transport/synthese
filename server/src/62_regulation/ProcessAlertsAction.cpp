
//////////////////////////////////////////////////////////////////////////
/// ProcessAlertsAction class implementation.
/// @file ProcessAlertsAction.cpp
/// @author Marc Jambert
/// @date 2015
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ProcessAlertsAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"

#include "Request.h"
#include "RegulationModule.hpp"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<Action, regulation::ProcessAlertsAction>::FACTORY_KEY = "ProcessAlertsAction";
	}

	namespace regulation
	{

		ParametersMap ProcessAlertsAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ProcessAlertsAction::_setFromParametersMap(const ParametersMap& map)
		{
		}


		void ProcessAlertsAction::run(
			Request& request)
        {
            util::Log::GetInstance().debug("Processing regulation alerts...");
            RegulationModule::ALERT_PROCESSING.loop();
            util::Log::GetInstance().debug("Processed regulation alerts.");
		}


		bool ProcessAlertsAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



}	}
