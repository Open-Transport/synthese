
/** CreateDisplayScreenAction class implementation.
	@file CreateDisplayScreenAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "DBLogPurgeAction.h"
#include "DBLog.h"
#include "DBLogRight.h"
#include "Request.h"
#include "DBLogEntryTableSync.h"
#include "ActionException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace server;
	using namespace util;
	using namespace time;
	
	template<> const string FactorableTemplate<Action, DBLogPurgeAction>::FACTORY_KEY("DBLogPurgeAction");

	namespace dblog
	{
		const string DBLogPurgeAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "ed");
		const string DBLogPurgeAction::PARAMETER_LOG_KEY(Action_PARAMETER_PREFIX + "lk");

		ParametersMap DBLogPurgeAction::getParametersMap() const
		{
			ParametersMap map;
			if(_dbLog.get())
			{
				map.insert(PARAMETER_LOG_KEY, _dbLog->getFactoryKey());
			}
			map.insert(PARAMETER_END_DATE, _endDate.toSQLString());
			return map;
		}

		void DBLogPurgeAction::_setFromParametersMap(const ParametersMap& map)
		{
			setDBLog(map.getString(PARAMETER_LOG_KEY, true, FACTORY_KEY));

			try
			{
				_endDate = map.getDateTime(PARAMETER_END_DATE, true, FACTORY_KEY);
			}
			catch (...)
			{
				throw ActionException("Bad date");
			}
		}

		void DBLogPurgeAction::run()
		{
			// Action
			DBLogEntryTableSync::Purge(_dbLog->getFactoryKey(), _endDate);

			// Log
			DBLog::AddSimpleEntry(_dbLog->getFactoryKey(), DBLogEntry::DB_LOG_INFO, "Log purge -> " + _endDate.toString(), _request->getUser().get());
		}


		bool DBLogPurgeAction::_isAuthorized(
		) const {
			return 
				_request->isAuthorized<DBLogRight>(DELETE_RIGHT) && _dbLog->isAuthorized(*_request, DELETE_RIGHT);
		}

		void DBLogPurgeAction::setDBLog( const std::string& value )
		{
			try
			{
				_dbLog.reset(Factory<DBLog>::create(value));
			}
			catch(FactoryException<DBLog> e)
			{
				throw ActionException(e.getMessage());
			}

		}

		DBLogPurgeAction::DBLogPurgeAction()
			: _endDate(TIME_UNKNOWN)
		{

		}
	}
}
