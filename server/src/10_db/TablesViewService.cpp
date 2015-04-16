
//////////////////////////////////////////////////////////////////////////////////////////
///	TablesViewService class implementation.
///	@file TablesViewService.cpp
///	@author User
///	@date 2012
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

#include "TablesViewService.hpp"

#include "DBModule.h"
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
	const string FactorableTemplate<Function,db::TablesViewService>::FACTORY_KEY = "tables";
	
	namespace db
	{
		const string TablesViewService::TAG_TABLE = "table";
		const string TablesViewService::ATTR_ID = "id";
		const string TablesViewService::ATTR_NAME = "name";
		const string TablesViewService::TAG_FIELD = "field";
		const string TablesViewService::ATTR_EXPORT_ON_FILE = "export_on_file";
		const string TablesViewService::ATTR_TYPE = "type";



		ParametersMap TablesViewService::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return map;
		}



		void TablesViewService::_setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);
		}

		ParametersMap TablesViewService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			BOOST_FOREACH( const DBModule::TablesByIdMap::value_type& it, DBModule::GetTablesById() )
			{
				boost::shared_ptr<ParametersMap> tablePM(new ParametersMap);

				tablePM->insert(ATTR_ID, static_cast<int>(it.first));
				tablePM->insert(ATTR_NAME, it.second->getFormat().NAME);

				BOOST_FOREACH(const FieldsList::value_type& f, it.second->getFieldsList())
				{
					boost::shared_ptr<ParametersMap> fieldPM(new ParametersMap);

					fieldPM->insert(ATTR_NAME, f.name);
					fieldPM->insert(ATTR_EXPORT_ON_FILE, f.exportOnFile);
					fieldPM->insert(ATTR_TYPE, f.type);

					tablePM->insert(TAG_FIELD, fieldPM);
				}

				map.insert(TAG_TABLE, tablePM);
			}

			return map;
		}
		
		
		
		bool TablesViewService::isAuthorized(
			const Session* session
		) const {
			return true;
			//session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string TablesViewService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}