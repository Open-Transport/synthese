
/** MySQLDBModifiedAction class implementation.
	@file MySQLDBModifiedAction.cpp
	@author Sylvain Pasche
	@date 2011

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "102_mysql/MySQLDBModifiedAction.hpp"
#include "102_mysql/MySQLDB.hpp"
#include "102_mysql/MySQLException.hpp"
#include "DBModule.h"
#include "ActionException.h"
#include "ParametersMap.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using boost::lexical_cast;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, db::MySQLDBModifiedAction>::FACTORY_KEY("MySQLDBModifiedAction");

	namespace db
	{
		const string MySQLDBModifiedAction::PARAMETER_SECRET_TOKEN(Action_PARAMETER_PREFIX + "st");
		const string MySQLDBModifiedAction::PARAMETER_TABLE(Action_PARAMETER_PREFIX + "tb");
		const string MySQLDBModifiedAction::PARAMETER_TYPE(Action_PARAMETER_PREFIX + "ty");
		const string MySQLDBModifiedAction::PARAMETER_ID(Action_PARAMETER_PREFIX + "id");



		ParametersMap MySQLDBModifiedAction::getParametersMap() const
		{
			return ParametersMap();
		}



		void MySQLDBModifiedAction::_setFromParametersMap(const ParametersMap& map) throw(ActionException)
		{
			_secretToken = map.get<string>(PARAMETER_SECRET_TOKEN);
			_table = map.get<string>(PARAMETER_TABLE);
			_type = map.get<string>(PARAMETER_TYPE);
			_id = map.get<RegistryKeyType>(PARAMETER_ID);
		}



		void MySQLDBModifiedAction::run(Request& request) throw(ActionException) {
			Log::GetInstance().debug(
				"MySQLDBModifiedAction::run. "
				" table: " + _table +
				" type: " + lexical_cast<string>(_type) +
				" id: " + lexical_cast<string>(_id)
			);

			MySQLDB* db = dynamic_cast<MySQLDB*>(DBModule::GetDB());
			if (!db)
			{
				throw ActionException("No MySQL database loaded");
			}

			try
			{
				db->addDBModifEvent(_table, _type, _id);
			}
			catch(MySQLException& e)
			{
				throw ActionException(e.getMessage());
			}
		}



		bool MySQLDBModifiedAction::isAuthorized(const server::Session* session) const
		{
			MySQLDB* db = dynamic_cast<MySQLDB*>(DBModule::GetDB());
			if (!db) {
				return false;
			}

			if (db->getSecretToken() != _secretToken)
			{
				Log::GetInstance().warn("Received a trigger action with the wrong secret token.");
				return false;
			}

			return true;
		}
	}
}
