
//////////////////////////////////////////////////////////////////////////
/// RuleUserUpdateAction class implementation.
/// @file RuleUserUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
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
#include "RuleUserUpdateAction.hpp"
#include "Request.h"
#include "DBModule.h"
#include "DBException.hpp"
#include "DBTableSync.hpp"
#include "UpdateQuery.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, graph::RuleUserUpdateAction>::FACTORY_KEY("RuleUserUpdateAction");
	}

	namespace graph
	{
		const string RuleUserUpdateAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "id";
		const string RuleUserUpdateAction::PARAMETER_VALUE_PREFIX = Action_PARAMETER_PREFIX + "va";



		ParametersMap RuleUserUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_id > 0)
			{
				BOOST_FOREACH(const Values::value_type& rule, _values)
				{
					map.insert(PARAMETER_VALUE_PREFIX + rule.first, rule.second);
				}
				map.insert(PARAMETER_OBJECT_ID, _id);
			}

			return map;
		}



		void RuleUserUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			_id = map.get<RegistryKeyType>(PARAMETER_OBJECT_ID);
			if (_id <= 0)
			{
				throw ActionException("id must be positive");
			}
			try
			{
				_tableSync = DBModule::GetTableSync(decodeTableId(_id));
			}
			catch(DBException)
			{
				throw ActionException("Invalid id, the table does not exist");
			}

			ParametersMap values(map.getExtract(PARAMETER_VALUE_PREFIX));
			BOOST_FOREACH(const ParametersMap::Map::value_type& element, values.getMap())
			{
				_values[element.first] = lexical_cast<RegistryKeyType>(element.second);
			}
		}



		void RuleUserUpdateAction::run(
			Request& request
		){
			DynamicUpdateQuery query(_tableSync->getFormat().NAME);
			query.addWhereField(TABLE_COL_ID, _id);
			BOOST_FOREACH(const Values::value_type& element, _values)
			{
				query.addUpdateField(element.first, element.second);
			}
			query.execute(boost::optional<DBTransaction&>());
		}



		bool RuleUserUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		//	return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>(WRITE);
		}
	}
}
