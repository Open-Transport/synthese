////////////////////////////////////////////////////////////////////////////////
///	DBLogHTMLView class implementation.
///	@file DBLogHTMLView.cpp
///	@author Hugues Romain (RCS)
///	@date jeu jan 29 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DBLogHTMLView.h"
#include "Exception.h"
#include "DBLogEntryTableSync.h"
#include "User.h"
#include "UserTableSync.h"
#include "SearchFormHTMLTable.h"
#include "DBLog.h"
#include "DBLogModule.h"
#include "DBLogEntryTableSync.h"
#include "DBLogRight.h"
#include "Request.h"
#include "Request.h"
#include "SecurityModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace security;
	using namespace time;
	using namespace server;
	using namespace html;
	
	namespace dblog
	{
		const string DBLogHTMLView::PARAMETER_PREFIX = "dblhv";
		const string DBLogHTMLView::PARAMETER_SEARCH_USER = "dlvsu";
		const string DBLogHTMLView::PARAMETER_SEARCH_TYPE = "dlvst";
		const string DBLogHTMLView::PARAMETER_START_DATE = "dlvsd";
		const string DBLogHTMLView::PARAMETER_END_DATE = "dlved";
		const string DBLogHTMLView::PARAMETER_SEARCH_TEXT = "dlvsx";
		const string DBLogHTMLView::PARAMETER_OBJECT_ID = "dlvoi";
		const string DBLogHTMLView::PARAMETER_OBJECT_ID2 = "dlvo2";

		DBLogHTMLView::DBLogHTMLView(
			const std::string& code
		):	_code(code),
			_searchLevel(DBLogEntry::DB_LOG_UNKNOWN),
			_searchStartDate(TIME_UNKNOWN),
			_searchEndDate(TIME_UNKNOWN),
			_searchObjectId(UNKNOWN_VALUE),
			_searchObjectId2(UNKNOWN_VALUE),
			_searchUserId(UNKNOWN_VALUE),
			_fixedLevel(true),
			_fixedStartDate(true),
			_fixedEndDate(true),
			_fixedObjectId(true),
			_fixedUserId(true),
			_fixedText(true)
		{
		}


		void DBLogHTMLView::set(
			const server::ParametersMap& map,
			std::string logKey,
			util::RegistryKeyType searchObjectId,
			util::RegistryKeyType searchObjectId2,
			util::RegistryKeyType searchUserId,
			DBLogEntry::Level searchLevel,
			time::DateTime searchStartDate,
			time::DateTime searchEndDate,
			std::string searchText
		){
			const string FACTORY_KEY("LogViewer");
			
			// Log key
			setLogKey(logKey);

			// Start Date
			if(searchStartDate.isUnknown())
			{
				searchStartDate = map.getDateTime(
					_getParameterName(PARAMETER_START_DATE), false, FACTORY_KEY
				);
				_fixedStartDate = false;
			}
			_searchStartDate = searchStartDate;

			// End Date
			if(searchEndDate.isUnknown())
			{
				searchEndDate = map.getDateTime(
					_getParameterName(PARAMETER_END_DATE), false, FACTORY_KEY
				);
				_fixedEndDate = false;
			}
			_searchEndDate = searchEndDate;

			// User
			if(searchUserId == UNKNOWN_VALUE)
			{
				searchUserId = map.getUid(
					_getParameterName(PARAMETER_SEARCH_USER), false, FACTORY_KEY
				);
				_fixedUserId = false;
			}
			_searchUserId = searchUserId;

			// Level
			if(searchLevel == DBLogEntry::DB_LOG_UNKNOWN)
			{
				int id(
					map.getInt(_getParameterName(PARAMETER_SEARCH_TYPE), false, FACTORY_KEY)
				);
				if (id > 0)
				{
					searchLevel = static_cast<DBLogEntry::Level>(id);
				}
				_fixedLevel = false;
			}
			_searchLevel = searchLevel;
			
			// Text
			if(searchText.empty())
			{
				searchText = map.getString(
					_getParameterName(PARAMETER_SEARCH_TEXT), false, FACTORY_KEY
				);
				_fixedText = false;
			}
			_searchText = searchText;

			// Object
			if(searchObjectId == UNKNOWN_VALUE)
			{
				searchObjectId = map.getUid(
					_getParameterName(PARAMETER_OBJECT_ID), false, FACTORY_KEY
				);
				_fixedObjectId = false;
			}
			_searchObjectId = searchObjectId;

			// Object 2
			if(searchObjectId2 == UNKNOWN_VALUE)
			{
				searchObjectId2 = map.getUid(
					_getParameterName(PARAMETER_OBJECT_ID2), false, FACTORY_KEY
				);
				_fixedObjectId2 = false;
			}
			_searchObjectId2 = searchObjectId2;

			// table parameters
			_requestParameters.setFromParametersMap(
				map.getMap(),
				_getParameterName(PARAMETER_START_DATE),
				30,
				false,
				PARAMETER_PREFIX  + _code
			);
		}
		
		
		
		std::string DBLogHTMLView::_getParameterName(
			const std::string& parameter
		) const {
			return PARAMETER_PREFIX + _code + parameter;
		}
		
		
		void DBLogHTMLView::display(
			std::ostream& stream,
			const Request& searchRequest,
			bool withForm,
			bool withLinkToAdminPage
		) const {

			SearchFormHTMLTable st(searchRequest.getHTMLForm("searchlog"+ _code));
			if(withForm)
			{
				stream << st.open();
				if(!_fixedStartDate)
				{
					stream <<
						st.cell(
							"Date début",
							st.getForm().getCalendarInput(
								_getParameterName(PARAMETER_START_DATE),
								_searchStartDate
						)	)
					;
				}
				if(!_fixedEndDate)
				{
					stream << st.cell(
						"Date fin",
						st.getForm().getCalendarInput(
							_getParameterName(PARAMETER_END_DATE),
							_searchEndDate
						)	)
					;
				}
				if(!_fixedUserId)
				{
					stream << st.cell(
							"Utilisateur",
							st.getForm().getSelectInput(
								_getParameterName(PARAMETER_SEARCH_USER),
								SecurityModule::getUserLabels(true),
								_searchUserId
						)	)
					;
				}
				if(!_fixedLevel)
				{
					stream << st.cell(
							"Type",
							st.getForm().getSelectInput(
								_getParameterName(PARAMETER_SEARCH_TYPE),
								DBLogModule::getEntryLevelLabels(true),
								static_cast<int>(_searchLevel)
						)	)
					;
				}
				if(!_fixedText)
				{
					stream << st.cell(
							"Texte",
							st.getForm().getTextInput(
								_getParameterName(PARAMETER_SEARCH_TEXT), _searchText
						)	)
					;
				}
				stream << st.close();
			}
			
			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(_getParameterName(PARAMETER_SEARCH_TYPE), "Type"));
			v.push_back(make_pair(_getParameterName(PARAMETER_START_DATE), "Date"));
			if(!_fixedUserId)
			{
				v.push_back(make_pair(_getParameterName(PARAMETER_SEARCH_USER), "Utilisateur"));
			}
			if(!_fixedObjectId && !_dbLog->getObjectColumnName().empty())
			{
				v.push_back(make_pair(string(), _dbLog->getObjectColumnName()));
			}
			if(!_fixedObjectId2 && !_dbLog->getObject2ColumnName().empty())
			{
				v.push_back(make_pair(string(), _dbLog->getObject2ColumnName()));
			}
			DBLog::ColumnsVector customCols = _dbLog->getColumnNames();
			BOOST_FOREACH(
				const DBLog::ColumnsVector::value_type& col,
				customCols
			){
				v.push_back(make_pair(string(), col));
			}

			// Search
			DBLogEntryTableSync::SearchResult entries(
				DBLogEntryTableSync::Search(
					_env,
					_dbLog->getFactoryKey()
					, _searchStartDate
					, _searchEndDate
					, _searchUserId
					, _searchLevel
					, _searchObjectId,
					_searchObjectId2,
					_searchText
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == _getParameterName(PARAMETER_START_DATE)
					, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_USER)
					, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_TYPE)
					, _requestParameters.raisingOrder
			)	);

			ResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				entries
			);

			stream << t.open();
			BOOST_FOREACH(shared_ptr<DBLogEntry> dbe, entries)
			{
				shared_ptr<const User> user;
				try
				{
					user = UserTableSync::Get(dbe->getUserId(), _env);
				}
				catch (...)
				{
				}
				stream << t.row();
				stream <<
					t.col() <<
					HTMLModule::getHTMLImage(
						DBLogModule::getEntryIcon(dbe->getLevel()),
						DBLogModule::getEntryLevelLabel(dbe->getLevel())
					)
				;
				stream << t.col() << dbe->getDate().toString(true);
				if(!_fixedUserId)
				{
					stream <<
						t.col() <<
						(	user.get() ?
							user->getLogin() :
							((dbe->getUserId() > 0) ? "(supprimé)" : "(robot)")
						)
					;
				}
				if(!_fixedObjectId && !_dbLog->getObjectColumnName().empty())
				{
					stream <<
						t.col() <<
						(	(dbe->getObjectId() > 0) ?
							_dbLog->getObjectName(dbe->getObjectId(), searchRequest) :
							string()
						)
					;
				}
				if(!_fixedObjectId2 && !_dbLog->getObject2ColumnName().empty())
				{
					stream <<
						t.col() <<
						(	(dbe->getObjectId2() > 0) ?
							_dbLog->getObjectName(dbe->getObjectId2(), searchRequest) :
							string()
						)
					;
				}

				DBLog::ColumnsVector cols = _dbLog->parse(*dbe, searchRequest);
				BOOST_FOREACH(const DBLog::ColumnsVector::value_type& col, cols)
				{
					stream << t.col() << col;
				}
			}
			
			stream << t.close();

		}
		
		string DBLogHTMLView::getLogKey() const
		{
			return
				_dbLog.get() ?
				_dbLog->getFactoryKey() :
				string();
		}
		
		
		
		string DBLogHTMLView::getLogName() const
		{
			return
				_dbLog.get() ?
				_dbLog->getName() :
				string();
		}
		
		
		
		void DBLogHTMLView::setLogKey(const string& value)
		{
			if (!Factory<DBLog>::contains(value))
			{
				_dbLog.reset();
				throw Exception("Invalid log key : " + value);
			}
			_dbLog.reset(Factory<DBLog>::create(value));
		}
		
		
		
		bool DBLogHTMLView::isAuthorized(
			const Request& request
		) const {
			return _dbLog.get() && _dbLog->isAuthorized(request, READ);
		}
		
		
		
		ParametersMap DBLogHTMLView::getParametersMap(
		) const {
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(_getParameterName(PARAMETER_SEARCH_USER), _searchUserId);
			m.insert(_getParameterName(PARAMETER_SEARCH_TYPE), static_cast<int>(_searchLevel));
			m.insert(_getParameterName(PARAMETER_START_DATE), _searchStartDate);
			m.insert(_getParameterName(PARAMETER_END_DATE), _searchEndDate);
			m.insert(_getParameterName(PARAMETER_SEARCH_TEXT), _searchText);
			m.insert(_getParameterName(PARAMETER_OBJECT_ID), _searchObjectId);
			m.insert(_getParameterName(PARAMETER_OBJECT_ID2), _searchObjectId2);
			return m;
		}
	}
}
