
//////////////////////////////////////////////////////////////////////////
/// ObjectCreateService class implementation.
/// @file ObjectCreateService.cpp
/// @author Thomas Puigt
/// @date 2014
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

#include "ObjectCreateService.hpp"

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
		template<> const string FactorableTemplate<Function, db::ObjectCreateService>::FACTORY_KEY("objectcreate");
	}

	namespace db
	{
		const string ObjectCreateService::PARAMETER_TABLE_ID = "table_id";
		const string ObjectCreateService::PARAMETER_FIELD_PREFIX = "field_";
		const string ObjectCreateService::ATTR_ID = "id";
		


		ParametersMap ObjectCreateService::_getParametersMap() const
		{
			ParametersMap map(_values);
			if(_tableSync.get())
			{
				map.insert(PARAMETER_TABLE_ID, static_cast<size_t>(dynamic_cast<DBTableSync&>(*_tableSync).getFormat().ID));
			}
			return map;
		}



		void ObjectCreateService::_setFromParametersMap(const ParametersMap& map)
		{
			// Table sync
			setTableId(map.get<RegistryTableType>(PARAMETER_TABLE_ID));

			// Record extraction
			ParametersMap record;
			BOOST_FOREACH(const ParametersMap::Map::value_type& item, map.getMap())
			{
				if(	item.first.size() <= PARAMETER_FIELD_PREFIX.size() ||
					item.first.substr(0, PARAMETER_FIELD_PREFIX.size()) != PARAMETER_FIELD_PREFIX
				){
					continue;
				}
				record.insert(
					item.first.substr(PARAMETER_FIELD_PREFIX.size()),
					map.getValue(item.first)
				);
			}

			// Record read
			LinkedObjectsIds linkedObjects(_value->getLinkedObjectsIds(record));
			BOOST_FOREACH(RegistryKeyType linkedId, linkedObjects)
			{
				DBModule::GetObject(linkedId, *_env);
			}
			_value->loadFromRecord(record, *_env);

			// Value check
			try
			{
				_value->checkIntegrity();
			}
			catch(ObjectBase::IntegrityException& e)
			{
				throw Exception(e.getMessage());
			}
		}



		ParametersMap ObjectCreateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			DBModule::CreateObject(_value.get(), _tableSync.get());

			map.insert(ATTR_ID, _value->getKey());
			return map;
		}



		bool ObjectCreateService::isAuthorized(
			const Session* session
		) const {
			return _value->allowCreate(session);
		}



		void ObjectCreateService::setTableId( util::RegistryTableType tableId )
		{
			// Table sync
			_tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
				DBModule::GetTableSync(tableId)
			);
			if(!_tableSync.get())
			{
				throw Exception("Incompatible table");
			}

			// New value
			_value = dynamic_pointer_cast<ObjectBase, Registrable>(_tableSync->newObject());
			if(!_value.get())
			{
				throw Exception("Incompatible table");
			}
		}



		std::string ObjectCreateService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
