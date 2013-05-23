
//////////////////////////////////////////////////////////////////////////
/// ObjectUpdateAction class implementation.
/// @file ObjectUpdateAction.cpp
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

#include "ObjectUpdateAction.hpp"

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
		template<> const string FactorableTemplate<Action, db::ObjectUpdateAction>::FACTORY_KEY("ObjectUpdate");
	}

	namespace db
	{
		const string ObjectUpdateAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "_object_id";
		const string ObjectUpdateAction::PARAMETER_FIELD_PREFIX = Action_PARAMETER_PREFIX + "_field_";



		ObjectUpdateAction::ObjectUpdateAction():
			_object(NULL)
		{}



		ParametersMap ObjectUpdateAction::getParametersMap() const
		{
			ParametersMap map(_values);
			if(_object)
			{
				map.insert(PARAMETER_OBJECT_ID, _object->getKey());
			}
			return map;
		}



		void ObjectUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Object id
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
			try
			{
				RegistryTableType tableId(decodeTableId(id));
				_tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
					DBModule::GetTableSync(tableId)
				);
				if(!_tableSync.get())
				{
					throw ActionException("Incompatible table");
				}
				_object = dynamic_cast<ObjectBase*>(
					_tableSync->getEditableRegistrable(id, *_env).get()
				);
				if(!_object)
				{
					throw ActionException("Incompatible table");
				}
				_setObject(*_object);
			}
			catch(ObjectNotFoundException<void*>&)
			{
				throw ActionException("No such object");
			}

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
			ParametersMap existingMap;
			_object->toParametersMap(existingMap);
			record.merge(existingMap);

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



		void ObjectUpdateAction::run(
			Request& request
		){
			stringstream logText;
			// Loop on updated fields
			//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _value->getAttribute());

			DBTransaction transaction;
			_value->beforeUpdate(*_object, transaction);
			_tableSync->saveRegistrable(*_value, transaction);
			_value->afterUpdate(*_object, transaction);
			transaction.run();

//			::AddUpdateEntry(*_value, text.str(), request.getUser().get());
		}



		bool ObjectUpdateAction::isAuthorized(
			const Session* session
		) const {
			return _object && _object->allowUpdate(session);
		}



		void ObjectUpdateAction::_setObject( const ObjectBase& value )
		{
			// Object to update
			_object = &value;

			// Table sync
			RegistryTableType tableId(decodeTableId(_object->getKey()));
			_tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
				DBModule::GetTableSync(tableId)
			);
			if(!_tableSync.get())
			{
				throw ActionException("Incompatible table");
			}

			// Copied value
			_value = _object->copy();
		}



		std::string ObjectUpdateAction::_GetInputName(
			const std::string& fieldName
		){
			return PARAMETER_FIELD_PREFIX + fieldName;
		}
}	}
