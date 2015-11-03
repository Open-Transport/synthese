////////////////////////////////////////////////////////////////////////////////
///	DBLogHTMLView class implementation.
///	@file DBLogHTMLView.cpp
///	@author Hugues Romain (RCS)
///	@date jeu jan 29 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
#include "ResaDBLog.h"
#include "Request.h"
#include "Profile.h"
#include "SecurityModule.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace util;
	using namespace security;
	using namespace server;
	using namespace html;
	using namespace resa;

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
		
		const string DBLogHTMLView::FILTER_ALL = "All";
		const string DBLogHTMLView::FILTER_RESA = "Réservation";
		const string DBLogHTMLView::FILTER_ABS = "Absence";
		const string DBLogHTMLView::FILTER_CANCEL = "Annulation";
		const string DBLogHTMLView::FILTER_CANC_D = "hors délai";



		DBLogHTMLView::DBLogHTMLView(
			const std::string& code
		):	_code(code),
			_searchStartDate(not_a_date_time),
			_fixedStartDate(true),
			_searchEndDate(not_a_date_time),
			_fixedEndDate(true),
			_searchLevel(DB_LOG_UNKNOWN),
			_fixedLevel(true),
			_fixedUserId(true),
			_fixedText(true),
			_fixedObjectId(true),
			_fixedObjectId2(true)
		{
			_requestParameters.orderField = _getParameterName(PARAMETER_START_DATE);
			_requestParameters.raisingOrder = false;
			_requestParameters.maxSize = 200;
		}



		void DBLogHTMLView::set(
			const util::ParametersMap& map,
			std::string logKey,
			optional<util::RegistryKeyType> searchObjectId,
			optional<util::RegistryKeyType> searchObjectId2,
			optional<util::RegistryKeyType> searchUserId,
			Level searchLevel,
			ptime searchStartDate,
			ptime searchEndDate,
			std::string searchText
		){
			const string FACTORY_KEY("LogViewer");

			// Log key
			setLogKey(logKey);

			// Start Date
			if(	searchStartDate.is_not_a_date_time() &&
				!map.getDefault<string>(_getParameterName(PARAMETER_START_DATE)).empty()
			){
				searchStartDate = time_from_string(map.get<string>(_getParameterName(PARAMETER_START_DATE)));
				_fixedStartDate = false;
			}
			_searchStartDate = searchStartDate;

			// End Date
			if(	searchEndDate.is_not_a_date_time() &&
				!map.getDefault<string>(_getParameterName(PARAMETER_END_DATE)).empty()
			){
				searchEndDate = time_from_string(map.get<string>(_getParameterName(PARAMETER_END_DATE)));
				_fixedEndDate = false;
			}
			_searchEndDate = searchEndDate;

			// User
			if(!searchUserId)
			{
				searchUserId = map.getOptional<RegistryKeyType>(
					_getParameterName(PARAMETER_SEARCH_USER)
				);
				_fixedUserId = false;
			}
			_searchUserId = searchUserId;

			// Level
			if(searchLevel == DB_LOG_UNKNOWN)
			{
				optional<int> id(
					map.getOptional<int>(_getParameterName(PARAMETER_SEARCH_TYPE))
				);
				if (id)
				{
					searchLevel = static_cast<Level>(*id);
				}
				_fixedLevel = false;
			}
			_searchLevel = searchLevel;

			// Text
			if(searchText.empty())
			{
				searchText = map.getDefault<string>(
					_getParameterName(PARAMETER_SEARCH_TEXT)
				);
				_fixedText = false;
			}
			_searchText = searchText;

			// Object
			if(!searchObjectId)
			{
				searchObjectId = map.getOptional<RegistryKeyType>(
					_getParameterName(PARAMETER_OBJECT_ID)
				);
				_fixedObjectId = false;
			}
			_searchObjectId = searchObjectId;

			// Object 2
			if(!searchObjectId2)
			{
				searchObjectId2 = map.getOptional<RegistryKeyType>(
					_getParameterName(PARAMETER_OBJECT_ID2)
				);
				_fixedObjectId2 = false;
			}
			_searchObjectId2 = searchObjectId2;

			// table parameters
			_requestParameters.setFromParametersMap(
				map,
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
			bool withLinkToAdminPage,
			const std::string& type
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
								optional<int>(static_cast<int>(_searchLevel))
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

			DBLogEntryTableSync::SearchResult entries;

			if (type == FILTER_ALL)
			{
				// Search all entries for one user
				entries = DBLogEntryTableSync::Search(
						_env,
						_dbLog->getFactoryKey()
						, _searchStartDate
						, _searchEndDate
						, _searchUserId
						, _searchLevel
						, _searchObjectId
						, _searchObjectId2
						, _searchText
						, _requestParameters.first
						, _requestParameters.maxSize
						, _requestParameters.orderField == _getParameterName(PARAMETER_START_DATE)
						, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_USER)
						, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_TYPE)
						, _requestParameters.raisingOrder
				);
			}
			else
			{
				// Search specific level entries for one user
				entries = DBLogEntryTableSync::Search(
						_env,
						_dbLog->getFactoryKey()
						, _searchStartDate
						, _searchEndDate
						, _searchUserId
						, _searchLevel
						, _searchObjectId
						, _searchObjectId2
						, type
						, _requestParameters.first
						, _requestParameters.maxSize
						, _requestParameters.orderField == _getParameterName(PARAMETER_START_DATE)
						, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_USER)
						, _requestParameters.orderField == _getParameterName(PARAMETER_SEARCH_TYPE)
						, _requestParameters.raisingOrder
				);
			}

			ResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				entries
			);

			stream << t.open();

			bool displayFilter = false;

			BOOST_FOREACH(const boost::shared_ptr<DBLogEntry>& dbe, entries)
			{
				if (type != FILTER_ALL)
				{
					try {
						const DBLogEntry::Content& content(dbe->getContent());
						const resa::ResaDBLog::_EntryType entryType(static_cast<resa::ResaDBLog::_EntryType>(lexical_cast<int>(content[0])));

						if (
							(type == FILTER_RESA && (entryType == ResaDBLog::RESERVATION_ENTRY || entryType == ResaDBLog::RESERVATION_UPDATE)) ||
							(type == FILTER_CANCEL && entryType == ResaDBLog::CANCELLATION_ENTRY) || 
							(type == FILTER_CANC_D && entryType == ResaDBLog::DELAYED_CANCELLATION_ENTRY) ||
							(type == FILTER_ABS && entryType == ResaDBLog::NO_SHOW_ENTRY)
						){
							displayFilter = true;
						}
						else
						{
							displayFilter = false;
						}
					}
					catch(bad_lexical_cast)
					{
					}
				}
				else
				{
					displayFilter = true;
				}

				if (displayFilter)
				{
					boost::shared_ptr<const User> user;
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
							"/admin/img/" + DBLogModule::getEntryIcon(dbe->getLevel()),
							DBLogModule::getEntryLevelLabel(dbe->getLevel())
						)
					;
					stream << t.col() << to_simple_string(dbe->getDate());
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
			const security::Profile& profile
		) const {
			return _dbLog.get() && _dbLog->isAuthorized(profile, READ);
		}



		ParametersMap DBLogHTMLView::getParametersMap(
		) const {
			ParametersMap m(_requestParameters.getParametersMap(PARAMETER_PREFIX + _code));
			if(_searchUserId)
			{
				m.insert(_getParameterName(PARAMETER_SEARCH_USER), *_searchUserId);
			}
			m.insert(_getParameterName(PARAMETER_SEARCH_TYPE), static_cast<int>(_searchLevel));
			m.insert(_getParameterName(PARAMETER_START_DATE), _searchStartDate);
			m.insert(_getParameterName(PARAMETER_END_DATE), _searchEndDate);
			m.insert(_getParameterName(PARAMETER_SEARCH_TEXT), _searchText);
			if(_searchObjectId)
			{
				m.insert(_getParameterName(PARAMETER_OBJECT_ID), *_searchObjectId);
			}
			if(_searchObjectId2)
			{
				m.insert(_getParameterName(PARAMETER_OBJECT_ID2), *_searchObjectId2);
			}
			return m;
		}
	}
}
