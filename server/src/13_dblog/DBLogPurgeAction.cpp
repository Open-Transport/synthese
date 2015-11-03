
/** DBLogPurgeAction class implementation.
	@file DBLogPurgeAction.cpp

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

#include "DBLogPurgeAction.h"

#include "ActionException.h"
#include "DBLog.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace server;
	using namespace util;

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
			map.insert(PARAMETER_END_DATE, _endDate);
			return map;
		}

		void DBLogPurgeAction::_setFromParametersMap(const ParametersMap& map)
		{
			setDBLog(map.get<string>(PARAMETER_LOG_KEY));

			try
			{
				_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
			}
			catch (...)
			{
				throw ActionException("Bad date");
			}
		}

		void DBLogPurgeAction::run(Request& request)
		{
			// Action
			DBLogEntryTableSync::Purge(_dbLog->getFactoryKey(), _endDate);

			// Log
			DBLog::AddSimpleEntry(_dbLog->getFactoryKey(), DB_LOG_INFO, "Log purge -> " + to_simple_string(_endDate), request.getUser().get());
		}



		bool DBLogPurgeAction::isAuthorized(const Session* session
		) const {
			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<DBLogRight>(DELETE_RIGHT) &&
				_dbLog->isAuthorized(*session->getUser()->getProfile(), DELETE_RIGHT)
			;
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
			: _endDate(not_a_date_time)
		{

		}
	}
}
