
//////////////////////////////////////////////////////////////////////////
/// JunctionUpdateAction class implementation.
/// @file JunctionUpdateAction.cpp
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

#include "JunctionUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "Junction.hpp"
#include "StopPoint.hpp"
#include "JunctionTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTUseRule.h"

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
		template<> const string FactorableTemplate<Action, pt::JunctionUpdateAction>::FACTORY_KEY("JunctionUpdateAction");
	}

	namespace pt
	{
		const string JunctionUpdateAction::PARAMETER_JUNCTION_ID = Action_PARAMETER_PREFIX + "id";
		const string JunctionUpdateAction::PARAMETER_BIDIRECTIONAL = Action_PARAMETER_PREFIX + "bd";
		const string JunctionUpdateAction::PARAMETER_FROM_ID = Action_PARAMETER_PREFIX + "fi";
		const string JunctionUpdateAction::PARAMETER_TO_ID = Action_PARAMETER_PREFIX + "ti";
		const string JunctionUpdateAction::PARAMETER_LENGTH = Action_PARAMETER_PREFIX + "le";
		const string JunctionUpdateAction::PARAMETER_TIME = Action_PARAMETER_PREFIX + "du";



		ParametersMap JunctionUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_junction.get())
			{
				map.insert(PARAMETER_JUNCTION_ID, _junction->getKey());
			}
			if(_from)
			{
				map.insert(PARAMETER_FROM_ID, _from->get() ? (*_from)->getKey() : RegistryKeyType(0));
			}
			if(_to)
			{
				map.insert(PARAMETER_TO_ID, _to->get() ? (*_to)->getKey() : RegistryKeyType(0));
			}
			if(_length)
			{
				map.insert(PARAMETER_LENGTH, *_length);
			}
			if(_duration)
			{
				map.insert(PARAMETER_TIME, static_cast<int>(_duration->total_seconds() / 3600));
			}
			if(_bidirectional)
			{
				map.insert(PARAMETER_BIDIRECTIONAL, *_bidirectional);
			}
			return map;
		}



		void JunctionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_JUNCTION_ID))
			{
				try
				{
					_junction = JunctionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_JUNCTION_ID), *_env);
				}
				catch (ObjectNotFoundException<Junction>&)
				{
					throw ActionException("No such junction");
				}
			}
			else
			{
				_junction = boost::shared_ptr<Junction>(new Junction);
			}

			if(map.isDefined(PARAMETER_FROM_ID))
			{
				try
				{
					_from = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_FROM_ID), *_env);
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					throw ActionException("No such from stop point");
				}
			}

			if(map.isDefined(PARAMETER_TO_ID))
			{
				try
				{
					_to = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TO_ID), *_env);
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					throw ActionException("No such to stop point");
				}
			}

			if(map.isDefined(PARAMETER_LENGTH))
			{
				_length = map.get<double>(PARAMETER_LENGTH);
			}

			if(map.isDefined(PARAMETER_TIME))
			{
				_duration = minutes(map.get<long>(PARAMETER_TIME));
			}

			if(map.isDefined(PARAMETER_BIDIRECTIONAL))
			{
				_bidirectional = map.getDefault<bool>(PARAMETER_BIDIRECTIONAL);
			}
		}



		void JunctionUpdateAction::run(
			Request& request
		){
			_junction->setStops(
				_from ? _from->get() : _junction->getStart(),
				_to ? _to->get() : _junction->getEnd(),
				_length ? *_length : _junction->getLength(),
				_duration ? *_duration : _junction->getDuration(),
				_bidirectional ? *_bidirectional : (bool) _junction->getBack()
			);

			JunctionTableSync::Save(_junction.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_junction->getKey());
			}
		}



		bool JunctionUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
