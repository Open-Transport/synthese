
//////////////////////////////////////////////////////////////////////////////////////////
///	DataSourcesService class implementation.
///	@file DataSourcesService.cpp
///	@author hromain
///	@date 2013
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

#include "DataSourcesService.hpp"

#include "DataSource.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,impex::DataSourcesService>::FACTORY_KEY = "data_sources";
	
	namespace impex
	{
		const string DataSourcesService::TAG_DATA_SOURCE = "data_source";
		


		ParametersMap DataSourcesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void DataSourcesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Datasource filter
			RegistryKeyType dataSourceId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(dataSourceId) try
			{
				_dataSourceFilter = Env::GetOfficialEnv().get<DataSource>(
					dataSourceId
				);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw RequestException("No such datasource");
			}
		}



		ParametersMap DataSourcesService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			if(_dataSourceFilter.get())
			{
				boost::shared_ptr<ParametersMap> dsPM(new ParametersMap);
				_dataSourceFilter->toParametersMap(*dsPM);
				map.insert(TAG_DATA_SOURCE, dsPM);
			}
			else
			{
				BOOST_FOREACH(const Registry<DataSource>::value_type& it, Env::GetOfficialEnv().getRegistry<DataSource>())
				{
					boost::shared_ptr<ParametersMap> dsPM(new ParametersMap);
					it.second->toParametersMap(*dsPM);
					map.insert(TAG_DATA_SOURCE, dsPM);
				}
			}
		
			return map;
		}
		
		
		
		bool DataSourcesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DataSourcesService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
