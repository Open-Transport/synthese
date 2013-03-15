
//////////////////////////////////////////////////////////////////////////////////////////
///	DBLogViewService class implementation.
///	@file DBLogViewService.cpp
///	@author hromain
///	@date 2013
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

#include "DBLogViewService.hpp"

#include "DBLog.h"
#include "DBLogEntryTableSync.h"
#include "DBLogModule.h"
#include "RequestException.h"
#include "Request.h"
#include "UserTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,dblog::DBLogViewService>::FACTORY_KEY = "db_log";
	


	namespace dblog
	{
		const string DBLogViewService::PARAMETER_LOG_KEY = "log_key";
		const string DBLogViewService::PARAMETER_FIRST = "first";
		const string DBLogViewService::PARAMETER_NUMBER = "number";
		const string DBLogViewService::PARAMETER_SEARCH_USER = "user";
		const string DBLogViewService::PARAMETER_SEARCH_TYPE = "type";
		const string DBLogViewService::PARAMETER_START_DATE = "date";
		const string DBLogViewService::PARAMETER_END_DATE = "end_date";
		const string DBLogViewService::PARAMETER_SEARCH_TEXT = "text";
		const string DBLogViewService::PARAMETER_OBJECT_ID = "object_id";
		const string DBLogViewService::PARAMETER_OBJECT_ID2 = "object2_id";
		


		ParametersMap DBLogViewService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void DBLogViewService::_setFromParametersMap(const ParametersMap& map)
		{
			const string FACTORY_KEY("LogViewer");

			// Log key
			string logKey(map.get<string>(PARAMETER_LOG_KEY));
			if (!Factory<DBLog>::contains(logKey))
			{
				_dbLog.reset();
				throw RequestException("Invalid log key : " + logKey);
			}
			_dbLog.reset(Factory<DBLog>::create(logKey));

			// Start Date
			if(	!map.getDefault<string>(PARAMETER_START_DATE).empty()
			){
				_searchStartDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
			}
			
			// End Date
			if(	!map.getDefault<string>(PARAMETER_END_DATE).empty()
			){
				_searchEndDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
			}

			// User
			_searchUserId = map.getOptional<RegistryKeyType>(
				PARAMETER_SEARCH_USER
			);

			// Level
			optional<int> id(
				map.getOptional<int>(PARAMETER_SEARCH_TYPE)
			);
			if (id)
			{
				_searchLevel = static_cast<DBLogEntry::Level>(*id);
			}
		
			// Text
			_searchText = map.getDefault<string>(
				PARAMETER_SEARCH_TEXT
			);
			
			// Object
			_searchObjectId = map.getOptional<RegistryKeyType>(
				PARAMETER_OBJECT_ID
			);

			// Object 2
			_searchObjectId2 = map.getOptional<RegistryKeyType>(
				PARAMETER_OBJECT_ID2
			);

			// table parameters
			_first = map.getDefault<size_t>(PARAMETER_FIRST, 0);
			_number = map.getDefault<size_t>(PARAMETER_NUMBER, 50);
		}



		ParametersMap DBLogViewService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			DBLogEntryTableSync::SearchResult entries(
				DBLogEntryTableSync::Search(
					*_env,
					_dbLog->getFactoryKey()
					, _searchStartDate
					, _searchEndDate
					, _searchUserId
					, _searchLevel
					, _searchObjectId,
					_searchObjectId2,
					_searchText
					, _first
					, _number
			)	);

			BOOST_FOREACH(const shared_ptr<DBLogEntry>& dbe, entries)
			{
				shared_ptr<ParametersMap> entryPM(new ParametersMap);
				entryPM->insert("id", dbe->getKey());

				entryPM->insert("entry_level", static_cast<int>(dbe->getLevel()));
				entryPM->insert("entry_level_text", DBLogModule::getEntryLevelLabel(dbe->getLevel()));

				shared_ptr<const User> user;
				try
				{
					user = UserTableSync::Get(dbe->getUserId(), *_env);
				}
				catch (...)
				{
				}
				if(user.get())
				{
					shared_ptr<ParametersMap> userPM(new ParametersMap);
					user->toParametersMap(*userPM);
					entryPM->insert("user", userPM);
				}

				if(dbe->getObjectId())
				{
					entryPM->insert("object_name", _dbLog->getObjectName(dbe->getObjectId(), request));
					entryPM->insert("object_id", dbe->getObjectId());
				}

				if(dbe->getObjectId2())
				{
					entryPM->insert("object2_name", _dbLog->getObjectName(dbe->getObjectId2(), request));
					entryPM->insert("object2_id", dbe->getObjectId2());
				}

				DBLog::ColumnsVector cols = _dbLog->parse(*dbe, request);
				size_t colNumber(0);
				BOOST_FOREACH(const DBLog::ColumnsVector::value_type& col, cols)
				{
					entryPM->insert("col"+ lexical_cast<string>(colNumber), col);
					++colNumber;
				}

				map.insert("entry", entryPM);
			}

			return map;
		}
		
		
		
		bool DBLogViewService::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string DBLogViewService::getOutputMimeType() const
		{
			return "text/html";
		}



		DBLogViewService::DBLogViewService(
		):	_searchStartDate(not_a_date_time),
			_searchEndDate(not_a_date_time),
			_searchLevel(DBLogEntry::DB_LOG_UNKNOWN),
			_first(0),
			_number(50)
		{
		}
}	}
