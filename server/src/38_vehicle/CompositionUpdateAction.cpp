
//////////////////////////////////////////////////////////////////////////
/// CompositionUpdateAction class implementation.
/// @file CompositionUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "CompositionUpdateAction.hpp"
#include "Request.h"
#include "RequestException.h"
#include "CompositionTableSync.hpp"
#include "ScheduledService.h"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopArea.hpp"
#include "PTUseRule.h"
#include "Path.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace calendar;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vehicle::CompositionUpdateAction>::FACTORY_KEY("CompositionUpdateAction");
	}

	namespace vehicle
	{
		const string CompositionUpdateAction::PARAMETER_COMPOSITION_ID = Action_PARAMETER_PREFIX + "ci";
		const string CompositionUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "si";
		const string CompositionUpdateAction::PARAMETER_FIRST_QUAY = Action_PARAMETER_PREFIX + "fq";
		const string CompositionUpdateAction::PARAMETER_VEHICLES = Action_PARAMETER_PREFIX + "ve";



		ParametersMap CompositionUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_composition.get())
			{
				map.insert(PARAMETER_COMPOSITION_ID, _composition->getKey());
			}
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}

			// Dates
			_getCalendarUpdateParametersMap(map);

			if(_firstQuay.get())
			{
				map.insert(PARAMETER_FIRST_QUAY, _firstQuay->getKey());
			}
			if(_vehicles)
			{
				map.insert(PARAMETER_VEHICLES, CompositionTableSync::SerializeVehicles(*_vehicles));
			}
			return map;
		}



		void CompositionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Composition
			if(map.getOptional<RegistryKeyType>(PARAMETER_COMPOSITION_ID))
			{
				try
				{
					_composition = CompositionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_COMPOSITION_ID), *_env);
				}
				catch(ObjectNotFoundException<Composition>&)
				{
					throw RequestException("No such composition");
				}
			}
			else
			{
				_composition.reset(new ServiceComposition);
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_FIRST_QUAY))
			{
				try
				{
					_firstQuay = StopPointTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_FIRST_QUAY), *_env);
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					throw RequestException("No such first quay");
				}
			}

			// Dates
			_setCalendarUpdateFromParametersMap(*_env, map);
			if(	!_composition->getKey() && !_calendarUpdateToDo()
			){
				// Default value
				setDate(day_clock::local_day());
			}

			// Vehicles
			if(map.isDefined(PARAMETER_VEHICLES))
			{
				_vehicles = CompositionTableSync::UnserializeVehicles(map.get<string>(PARAMETER_VEHICLES), *_env);
			}

			// Service
			try
			{
				_service = ScheduledServiceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}
		}



		void CompositionUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Dates
			_doCalendarUpdate(*_composition, request);

			if(dynamic_cast<ServiceComposition*>(_composition.get()))
			{
				ServiceComposition& serviceComposition(static_cast<ServiceComposition&>(*_composition));

				if(_service.get())
				{
					serviceComposition.setService(_service.get());
				}

				if(_firstQuay.get())
				{
					SchedulesBasedService::ServedVertices vertices;
					size_t rank(0);
					BOOST_FOREACH(const Path::Edges::value_type& edge, serviceComposition.getService()->getPath()->getEdges())
					{
						if(rank++)
						{
							vertices.push_back(edge->getFromVertex());
						}
						else
						{
							vertices.push_back(_firstQuay.get());
						}
					}
					serviceComposition.setServedVertices(vertices);
				}
			}

			if(_vehicles)
			{
				_composition->setVehicles(*_vehicles);
			}

			CompositionTableSync::Save(_composition.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_composition->getKey());
			}

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CompositionUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
}	}
