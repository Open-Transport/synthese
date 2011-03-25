
//////////////////////////////////////////////////////////////////////////
/// DataSourceRemoveAction class implementation.
/// @file DataSourceRemoveAction.cpp
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
#include "DataSourceRemoveAction.hpp"
#include "Request.h"
#include "DataSourceTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, impex::DataSourceRemoveAction>::FACTORY_KEY("DataSourceRemoveAction");
	}

	namespace impex
	{
		const string DataSourceRemoveAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "ds";
		
		
		
		ParametersMap DataSourceRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_dataSource.get())
			{
				map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
			}
			return map;
		}
		
		
		
		void DataSourceRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_dataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID), *_env);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such datasource");
			}

		}
		
		
		
		void DataSourceRemoveAction::run(
			Request& request
		){
			DataSourceTableSync::Remove(_dataSource->getKey());
//			::AddDeleteEntry(*_object, *request.getUser());
		}
		
		
		
		bool DataSourceRemoveAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
