
//////////////////////////////////////////////////////////////////////////
/// StopAreaTransferRemoveAction class implementation.
/// @file StopAreaTransferRemoveAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "StopAreaTransferRemoveAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRule.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaTransferRemoveAction>::FACTORY_KEY("StopAreaTransferRemoveAction");
	}

	namespace pt
	{
		const string StopAreaTransferRemoveAction::PARAMETER_FROM_ID = Action_PARAMETER_PREFIX + "fi";
		const string StopAreaTransferRemoveAction::PARAMETER_TO_ID = Action_PARAMETER_PREFIX + "ti";



		ParametersMap StopAreaTransferRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_from.get())
			{
				map.insert(PARAMETER_FROM_ID, _from->getKey());
			}
			if(_to.get())
			{
				map.insert(PARAMETER_TO_ID, _to->getKey());
			}
			return map;
		}



		void StopAreaTransferRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_from = StopPointTableSync::Get(map.get<RegistryKeyType>(PARAMETER_FROM_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such from stop point");
			}

			try
			{
				_to = StopPointTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TO_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such to stop point");
			}

			if(_from->getConnectionPlace() != _to->getConnectionPlace())
			{
				throw ActionException("Internal transfers must concern two stops within the same stop area.");
			}
		}



		void StopAreaTransferRemoveAction::run(
			Request& request
		){
			StopArea::TransferDelaysMap value(
				_from->getConnectionPlace()->getTransferDelays()
			);
			value.erase(
				make_pair(
					_from->getKey(),
					_to->getKey()
			)	);
			const_cast<StopArea*>(_from->getConnectionPlace())->setTransferDelaysMatrix(value);

			StopAreaTableSync::Save(const_cast<StopArea*>(_from->getConnectionPlace()));
		}



		bool StopAreaTransferRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
