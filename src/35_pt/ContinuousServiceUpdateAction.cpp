
//////////////////////////////////////////////////////////////////////////
/// ContinuousServiceUpdateAction class implementation.
/// @file ContinuousServiceUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ContinuousServiceUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "ContinuousServiceTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ContinuousServiceUpdateAction>::FACTORY_KEY("ContinuousServiceUpdateAction");
	}

	namespace pt
	{
		const string ContinuousServiceUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "id";
		const string ContinuousServiceUpdateAction::PARAMETER_WAITING_DURATION = Action_PARAMETER_PREFIX + "wa";
		const string ContinuousServiceUpdateAction::PARAMETER_END_TIME = Action_PARAMETER_PREFIX + "et";



		ParametersMap ContinuousServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
//				map.insert(PARAMETER_END_TIME, (_service->getDepartureSchedule(false, 0) + _range).total_seconds() / 60);
//				map.insert(PARAMETER_WAITING_DURATION, _service->getMaxWaitingTime().total_seconds() / 60);
			}
			return map;
		}



		void ContinuousServiceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = ContinuousServiceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<ContinuousService>&)
			{
				throw ActionException("No such service");
			}

			_duration = minutes(map.get<int>(PARAMETER_WAITING_DURATION));

			time_duration endTime(not_a_date_time);
			if(!map.getDefault<string>(PARAMETER_END_TIME).empty())
			{
				try
				{
					endTime = duration_from_string(map.get<string>(PARAMETER_END_TIME));
				}
				catch(bad_lexical_cast)
				{
					throw ActionException("Bad end time");
				}
			}


			time_duration startTime(_service->getDepartureSchedule(false, 0));

			_range = endTime - startTime;
		}



		void ContinuousServiceUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_service->setMaxWaitingTime(_duration);
			_service->setRange(_range);

			ContinuousServiceTableSync::Save(_service.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool ContinuousServiceUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
	}
}
