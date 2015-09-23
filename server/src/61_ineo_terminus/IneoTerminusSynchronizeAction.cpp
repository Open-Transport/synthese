
/** IneoTerminusSynchronizeAction class implementation.
	@file IneoTerminusSynchronizeAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "IneoTerminusSynchronizeAction.hpp"
#include "IneoTerminusConnection.hpp"

#include "ParametersMap.h"
#include "Log.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"


namespace synthese
{
	template<> const std::string util::FactorableTemplate<server::Action, ineo_terminus::IneoTerminusSynchronizeAction>::FACTORY_KEY("IneoTerminusSynchronize");

	namespace ineo_terminus
	{

		util::ParametersMap IneoTerminusSynchronizeAction::getParametersMap() const
		{
			// This action has no parameter, return an empty map
			util::ParametersMap m;
			return m;
		}


		void IneoTerminusSynchronizeAction::_setFromParametersMap(const util::ParametersMap& map)
		{

		}


		void IneoTerminusSynchronizeAction::run(server::Request& request)
		{
			util::Log::GetInstance().info("Ineo Terminus : manual synchronization requested");
			IneoTerminusConnection::GetTheConnection()->synchronizeMessages();
		}



		IneoTerminusSynchronizeAction::IneoTerminusSynchronizeAction(
		): FactorableTemplate<server::Action, IneoTerminusSynchronizeAction>()
		{

		}


		bool IneoTerminusSynchronizeAction::isAuthorized(const server::Session* session) const
		{
			bool result = (NULL != session) && (true == session->hasProfile());
			return result;
		}
}	}
