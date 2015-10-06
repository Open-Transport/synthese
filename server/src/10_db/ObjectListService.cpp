
//////////////////////////////////////////////////////////////////////////
/// ObjectListService.cpp class implementation.
/// @file ObjectListService.cpp
/// @author Marc Jambert
/// @date 2015
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
#include "ObjectListService.hpp"

#include "Exception.h"
#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Function, db::ObjectListService>::FACTORY_KEY("objectlist");
	}

	namespace db
	{
		const string ObjectListService::PARAMETER_TABLE_ID = "table_id";


		ParametersMap ObjectListService::_getParametersMap() const
		{
			ParametersMap map(_values);
			if(_tableSync.get())
			{
				map.insert(PARAMETER_TABLE_ID, static_cast<size_t>(dynamic_cast<DBTableSync&>(*_tableSync).getFormat().ID));
			}
			return map;
		}



		void ObjectListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Table sync
			setTableId(map.get<RegistryTableType>(PARAMETER_TABLE_ID));
            Function::setOutputFormatFromMap(map, string());            
		}



		ParametersMap ObjectListService::run(
			std::ostream& stream,
			const Request& request
		) const {

            const RegistryBase& registry = _tableSync->getRegistry(Env::GetOfficialEnv());
            RegistryBase::RegistrablesVector objects = registry.getRegistrablesVector();
            ParametersMap listMap;
            for (RegistryBase::RegistrablesVector::const_iterator it = objects.begin();
                 it != objects.end(); ++it)
            {
                boost::shared_ptr<ParametersMap> map(new ParametersMap());
                (*it)->toParametersMap(*map, false);
                listMap.insert("object", map);
            }
            if (_outputFormat == MimeTypes::JSON)
            {
                listMap.outputJSON(stream, "objects");
            }
            else if (_outputFormat == MimeTypes::XML)
            {
                listMap.outputXML(stream, "objects");
            }
            return listMap;
		}



		bool ObjectListService::isAuthorized(
			const Session* session
		) const {
			return _tableSync->allowList(session);
		}



		void ObjectListService::setTableId( util::RegistryTableType tableId )
		{
			// Table sync
			_tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
				DBModule::GetTableSync(tableId)
			);
			if(!_tableSync.get())
			{
				throw Exception("Incompatible table");
			}

		}



		std::string ObjectListService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}
}	}
