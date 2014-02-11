
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternUpdateAction class implementation.
/// @file JourneyPatternUpdateAction.cpp
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

#include "JourneyPatternUpdateAction.hpp"

#include "ActionException.h"
#include "CommercialLineTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "JourneyPatternTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "DestinationTableSync.hpp"
#include "ScheduledServiceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace vehicle;
	using namespace impex;
	using namespace graph;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::JourneyPatternUpdateAction>::FACTORY_KEY("JourneyPatternUpdateAction");
	}

	namespace pt
	{
		const string JourneyPatternUpdateAction::PARAMETER_ROUTE_ID = Action_PARAMETER_PREFIX + "id";
		const string JourneyPatternUpdateAction::PARAMETER_TRANSPORT_MODE_ID = Action_PARAMETER_PREFIX + "tm";
		const string JourneyPatternUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string JourneyPatternUpdateAction::PARAMETER_DIRECTION = Action_PARAMETER_PREFIX + "di";
		const string JourneyPatternUpdateAction::PARAMETER_DIRECTION_ID = Action_PARAMETER_PREFIX + "ii";
		const string JourneyPatternUpdateAction::PARAMETER_WAYBACK = Action_PARAMETER_PREFIX + "wb";
		const string JourneyPatternUpdateAction::PARAMETER_MAIN = Action_PARAMETER_PREFIX + "ma";
		const string JourneyPatternUpdateAction::PARAMETER_PLANNED_LENGTH = Action_PARAMETER_PREFIX + "_planned_length";
		const string JourneyPatternUpdateAction::PARAMETER_LINE_ID = Action_PARAMETER_PREFIX + "_line_id";



		ParametersMap JourneyPatternUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_route.get())
			{
				map.insert(PARAMETER_ROUTE_ID, _route->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_direction)
			{
				map.insert(PARAMETER_DIRECTION, *_direction);
			}
			if(_wayback)
			{
				map.insert(PARAMETER_WAYBACK, *_wayback);
			}
			if(_transportMode)
			{
				map.insert(PARAMETER_TRANSPORT_MODE_ID, _transportMode->get() ? (*_transportMode)->getKey() : RegistryKeyType(0));
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			if(_direction)
			{
				map.insert(PARAMETER_DIRECTION_ID, _directionObj->get() ? (*_directionObj)->getKey() : RegistryKeyType(0));
			}
			if(_main)
			{
				map.insert(PARAMETER_MAIN, *_main);
			}

			// Line
			if(_line)
			{
				map.insert(PARAMETER_LINE_ID, (*_line)->getKey());
			}
			return map;
		}



		void JourneyPatternUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_route = JourneyPatternTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROUTE_ID), *_env);
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such route");
			}

			if(map.isDefined(PARAMETER_DIRECTION))
			{
				_direction = map.get<string>(PARAMETER_DIRECTION);
			}
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
			if(map.isDefined(PARAMETER_WAYBACK))
			{
				_wayback = map.get<bool>(PARAMETER_WAYBACK);
			}

			if(map.isDefined(PARAMETER_MAIN))
			{
				_main = map.get<bool>(PARAMETER_MAIN);
			}

			if(map.isDefined(PARAMETER_PLANNED_LENGTH))
			{
				_plannedLength = map.get<MetricOffset>(PARAMETER_PLANNED_LENGTH);
			}

			if(map.isDefined(PARAMETER_TRANSPORT_MODE_ID))
			{
				RegistryKeyType rid(map.getDefault<RegistryKeyType>(PARAMETER_TRANSPORT_MODE_ID, 0));
				if(rid > 0)	try
				{
					_transportMode	= RollingStockTableSync::GetEditable(rid, *_env);
				}
				catch(ObjectNotFoundException<RollingStock>&)
				{
					throw ActionException("No such transport mode");
				}
				else
				{
					_transportMode = boost::shared_ptr<RollingStock>();
				}
			}

			// Line
			if(map.isDefined(PARAMETER_LINE_ID))
			{
				RegistryKeyType lid(map.getDefault<RegistryKeyType>(PARAMETER_LINE_ID, 0));
				if(lid > 0)	try
				{
					_line = CommercialLineTableSync::GetEditable(lid, *_env);
				}
				catch(ObjectNotFoundException<CommercialLine>&)
				{
					throw ActionException("No such line");
				}
			}

			if(map.isDefined(PARAMETER_DIRECTION_ID))
			{
				RegistryKeyType rid(map.getDefault<RegistryKeyType>(PARAMETER_DIRECTION_ID, 0));
				if(rid > 0)	try
				{
					_directionObj = DestinationTableSync::GetEditable(rid, *_env);
				}
				catch(ObjectNotFoundException<Destination>&)
				{
					throw ActionException("No such direction id");
				}
				else
				{
					_directionObj = boost::shared_ptr<Destination>();
				}
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);

			// Massive calendar service updates
			_setCalendarUpdateFromParametersMap(*_env, map);
			if(_calendarUpdateToDo())
			{
				ScheduledServiceTableSync::Search(*_env, _route->getKey());
			}
		}



		void JourneyPatternUpdateAction::run(
			Request& request
		){

			DBTransaction transaction;

//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_name)
			{
				_route->setName(*_name);
			}
			if(_transportMode)
			{
				_route->setRollingStock(_transportMode->get());
			}
			if(_main)
			{
				_route->setMain(*_main);
			}
			if(_direction)
			{
				_route->setDirection(*_direction);
			}
			if(_wayback)
			{
				_route->setWayBack(*_wayback);
			}
			if(_plannedLength)
			{
				_route->setPlannedLength(*_plannedLength);
			}

			// Importable
			_doImportableUpdate(*_route, request);

			if(_directionObj)
			{
				_route->setDirectionObj(_directionObj->get());
			}

			// Line
			if(_line)
			{
				_route->setCommercialLine(_line->get());
			}

			JourneyPatternTableSync::Save(_route.get(), transaction);

			// Massive date update of services
			if(_calendarUpdateToDo())
			{
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*_route->sharedServicesMutex
				);
				BOOST_FOREACH(Service* itService, _route->getServices())
				{
					ScheduledService* service(dynamic_cast<ScheduledService*>(itService));
					if(service == NULL)
					{
						continue;
					}

					_doCalendarUpdate(*service, request);

					ScheduledServiceTableSync::Save(service, transaction);
				}
			}

			transaction.run();

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool JourneyPatternUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this journey pattern
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
	}
}
