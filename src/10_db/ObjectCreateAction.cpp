
//////////////////////////////////////////////////////////////////////////
/// ObjectCreateAction class implementation.
/// @file ObjectCreateAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "ObjectCreateAction.hpp"

#include "ActionException.h"
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
		template<> const string FactorableTemplate<Action, db::ObjectCreateAction>::FACTORY_KEY("ObjectCreate");
	}

	namespace db
	{
		const string ObjectCreateAction::PARAMETER_TABLE_ID = Action_PARAMETER_PREFIX + "_table_id";
		const string ObjectCreateAction::PARAMETER_FIELD_PREFIX = Action_PARAMETER_PREFIX + "_field_";



		ParametersMap ObjectCreateAction::getParametersMap() const
		{
			ParametersMap map(_values);
			if(_tableSync.get())
			{
				map.insert(PARAMETER_TABLE_ID, static_cast<size_t>(dynamic_cast<DBTableSync&>(*_tableSync).getFormat().ID));
			}
			return map;
		}



		void ObjectCreateAction::_setFromParametersMap(const ParametersMap& map)
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
				throw ActionException(e.getMessage());
			}
		}



		void ObjectCreateAction::run(
			Request& request
		){
			stringstream logText;
			{
//				Log Object creation
			}

			DBTransaction transaction;
			_value->beforeCreate(transaction);
			_tableSync->saveRegistrable(*_value, transaction);
			_value->afterCreate(transaction);
			transaction.run();

//			::AddUpdateEntry(*_value, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_value->getKey());
			}
		}



		bool ObjectCreateAction::isAuthorized(
			const Session* session
		) const {
			return _value->allowCreate(session);
		}



		void ObjectCreateAction::setTableId( util::RegistryTableType tableId )
		{
			// Table sync
			_tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
				DBModule::GetTableSync(tableId)
			);
			if(!_tableSync.get())
			{
				throw ActionException("Incompatible table");
			}

			// New value
			_value = dynamic_pointer_cast<ObjectBase, Registrable>(_tableSync->newObject());
			if(!_value.get())
			{
				throw ActionException("Incompatible table");
			}
		}



		std::string ObjectCreateAction::_GetInputName(
			const std::string& fieldName
		){
			return PARAMETER_FIELD_PREFIX + fieldName;
		}
}	}
