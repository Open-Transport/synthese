
//////////////////////////////////////////////////////////////////////////
/// CompositionUpdateAction class implementation.
/// @file CompositionUpdateAction.cpp
/// @author RCSobility
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
#include "Composition.hpp"
#include "RequestException.h"
#include "CompositionTableSync.hpp"
#include "ScheduledService.h"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
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
		template<> const string FactorableTemplate<Action, pt_operation::CompositionUpdateAction>::FACTORY_KEY("CompositionUpdateAction");
	}

	namespace pt_operation
	{
		const string CompositionUpdateAction::PARAMETER_COMPOSITION_ID = Action_PARAMETER_PREFIX + "ci";
		const string CompositionUpdateAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";
		const string CompositionUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "si";
		const string CompositionUpdateAction::PARAMETER_FIRST_QUAY = Action_PARAMETER_PREFIX + "fq";



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
			if(_date)
			{
				map.insert(PARAMETER_DATE, to_iso_extended_string(*_date));
			}
			if(_firstQuay.get())
			{
				map.insert(PARAMETER_FIRST_QUAY, _firstQuay->getKey());
			}
			return map;
		}



		void CompositionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
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
				_composition.reset(new Composition);
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

			// Date
			if(map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = day_clock::local_day();
			}
			else
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
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

			Calendar calendar;
			if(_date)
			{
				calendar.setActive(*_date);
				_composition->setCalendar(calendar);
			}

			if(_service.get())
			{
				_composition->setService(_service.get());
			}

			if(_firstQuay.get())
			{
				Service::ServedVertices vertices;
				size_t rank(0);
				BOOST_FOREACH(const Path::Edges::value_type& edge, _composition->getService()->getPath()->getEdges())
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
				_composition->setServedVertices(vertices);
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
	}
}
