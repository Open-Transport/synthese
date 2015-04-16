
//////////////////////////////////////////////////////////////////////////
/// RemoveObjectAction class implementation.
/// @file RemoveObjectAction.cpp
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
#include "RemoveObjectAction.hpp"
#include "Request.h"
#include "DBTransaction.hpp"
#include "DBModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<Action, db::RemoveObjectAction>::FACTORY_KEY = "RemoveObjectAction";
	}

	namespace db
	{
		const string RemoveObjectAction::PARAMETER_OLD_OBJECT_ID = Action_PARAMETER_PREFIX + "oi";
		const string RemoveObjectAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "_object_id";



		ParametersMap RemoveObjectAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_OBJECT_ID, _objectId);
			return map;
		}



		void RemoveObjectAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Object ID
			if(map.isDefined(PARAMETER_OBJECT_ID))
			{
				try
				{
					setObjectId(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
				}
				catch(Record::MissingParameterException& e)
				{
					throw ActionException("Invalid object id : "+ e.getMessage());
				}
			}
			else if(map.isDefined(PARAMETER_OLD_OBJECT_ID))
			{
				try
				{
					setObjectId(map.get<RegistryKeyType>(PARAMETER_OLD_OBJECT_ID));
				}
				catch(Record::MissingParameterException& e)
				{
					throw ActionException("Invalid object id : "+ e.getMessage());
				}
			}
			else
			{
				throw ActionException("Object id was not specified");
			}
		}



		void RemoveObjectAction::run(
			Request& request
		){
			DBTransaction transaction;
			_tableSync->deleteRecord(request.getSession().get(), _objectId, transaction);
			transaction.run();
		}



		bool RemoveObjectAction::isAuthorized(
			const Session* session
		) const {
			return _tableSync->canDelete(session, _objectId);
		}



		void RemoveObjectAction::setObjectId( util::RegistryKeyType value )
		{
			_objectId = value;
			_tableSync = boost::shared_ptr<DBTableSync>(DBModule::GetTableSync(decodeTableId(_objectId)));
		}
}	}
