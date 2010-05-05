
//////////////////////////////////////////////////////////////////////////
/// ServiceDateChangeAction class implementation.
/// @file ServiceDateChangeAction.cpp
/// @author Hugues Romain
/// @date 2010
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
#include "ServiceDateChangeAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Fetcher.h"
#include "SchedulesBasedService.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceDateChangeAction>::FACTORY_KEY("ServiceDateChangeAction");
	}

	namespace pt
	{
		const string ServiceDateChangeAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "id";
		const string ServiceDateChangeAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";
		
		
		
		ParametersMap ServiceDateChangeAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_DATE, _date);
			return map;
		}
		
		
		
		void ServiceDateChangeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Fetcher<SchedulesBasedService>::FetchEditable(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<NonPermanentService>&)
			{
				throw ActionException("No such service");
			}

			_date = from_simple_string(map.get<string>(PARAMETER_DATE));
		}
		
		
		
		void ServiceDateChangeAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_service->isActive(_date))
			{
				_service->setInactive(_date);
			}
			else
			{
				_service->setActive(_date);
			}

			Fetcher<SchedulesBasedService>::FetchSave(*_service);

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool ServiceDateChangeAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
