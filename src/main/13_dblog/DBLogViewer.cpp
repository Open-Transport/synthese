
/** DBLogViewer class implementation.
	@file DBLogViewer.cpp

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

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "01_util/Constants.h"

#include "05_html/ResultHTMLTable.h"
#include "05_html/SearchFormHTMLTable.h"

#include "11_interfaces/InterfaceModule.h"

#include "12_security/SecurityModule.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogViewer.h"
#include "13_dblog/DBLogModule.h"
#include "13_dblog/DBLogEntryTableSync.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace html;
	using namespace time;

	namespace dblog
	{
		const string DBLogViewer::PARAMETER_LOG_KEY = "dlvk";
		const string DBLogViewer::PARAMETER_SEARCH_USER = "dlvsu";
		const string DBLogViewer::PARAMETER_SEARCH_TYPE = "dlvst";
		const string DBLogViewer::PARAMETER_START_DATE = "dlvsd";
		const string DBLogViewer::PARAMETER_END_DATE = "dlved";
		const string DBLogViewer::PARAMETER_SEARCH_TEXT = "dlvsx";

		DBLogViewer::DBLogViewer()
			: AdminInterfaceElement("dblogs", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _searchLevel(DBLogEntry::DB_LOG_UNKNOWN), _searchStartDate(TIME_UNKNOWN)
			, _searchEndDate(TIME_UNKNOWN)
		{}

		void DBLogViewer::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Log key
			it = map.find(PARAMETER_LOG_KEY);
			if (it == map.end())
				throw AdminParametersException("Log key not specified");
			if (!Factory<DBLog>::contains(it->second))
				throw AdminParametersException("Invalid log key : " + it->second);
			_dbLog = Factory<DBLog>::create(it->second);

			// Start Date
			it = map.find(PARAMETER_START_DATE);
			if (it != map.end())
				_searchStartDate = DateTime::FromString(it->second);

			// End Date
			it = map.find(PARAMETER_END_DATE);
			if (it != map.end())
				_searchEndDate = DateTime::FromString(it->second);

			// User
			it = map.find(PARAMETER_SEARCH_USER);
			if (it != map.end() && Conversion::ToLongLong(it->second) > 0)
				_searchUser = UserTableSync::get(Conversion::ToLongLong(it->second));

			// Level
			it = map.find(PARAMETER_SEARCH_TYPE);
			if (it != map.end())
				_searchLevel = (DBLogEntry::Level) Conversion::ToInt(it->second);

			// Text
			it = map.find(PARAMETER_SEARCH_TEXT);
			if (it != map.end())
				_searchText = it->second;

			// table parameters
			_resultTableRequestParameters = ResultHTMLTable::getParameters(map, PARAMETER_START_DATE, 30);

			// Launch the search
			_result = DBLogEntryTableSync::search(
				_dbLog->getFactoryKey()
				, _searchStartDate
				, _searchEndDate
				, _searchUser
				, _searchLevel
				, _searchText
				, _resultTableRequestParameters.first
				, _resultTableRequestParameters.maxSize
				, _resultTableRequestParameters.orderField == PARAMETER_START_DATE
				, _resultTableRequestParameters.orderField == PARAMETER_SEARCH_USER
				, _resultTableRequestParameters.orderField == PARAMETER_SEARCH_TYPE
				, _resultTableRequestParameters.raisingOrder
				);

			_resultTableResultParameters.next = _result.size() == _resultTableRequestParameters.maxSize + 1;
			_resultTableResultParameters.size = _result.size();
			if (_resultTableResultParameters.next)
				_result.pop_back();
		}

		string DBLogViewer::getTitle() const
		{
			return (_dbLog != NULL) ? _dbLog->getName() : "(pas de journal)";
		}

		void DBLogViewer::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DBLogViewer>());

			stream << "<h1>Recherche d'entrées</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			st.getForm().addHiddenField(PARAMETER_LOG_KEY, _dbLog->getFactoryKey());
			stream << st.open();
			stream << st.cell("Date début", st.getForm().getCalendarInput(PARAMETER_START_DATE, _searchStartDate));
			stream << st.cell("Date fin", st.getForm().getCalendarInput(PARAMETER_END_DATE, _searchEndDate));
			stream << st.cell("Utilisateur", st.getForm().getSelectInput(PARAMETER_SEARCH_USER, SecurityModule::getUserLabels(true), _searchUser ? _searchUser->getKey() : 0));
			stream << st.cell("Type", st.getForm().getSelectInput(PARAMETER_SEARCH_TYPE, DBLogModule::getEntryLevelLabels(true), (int) _searchLevel));
			stream << st.cell("Texte", st.getForm().getTextInput(PARAMETER_SEARCH_TEXT, _searchText));
			stream << st.close();
			
			stream << "<h1>Résultat de la recherche</h1>";

			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v.push_back(make_pair(PARAMETER_START_DATE, "Date"));
			v.push_back(make_pair(PARAMETER_SEARCH_USER, "Utilisateur"));
			DBLog::ColumnsVector customCols = _dbLog->getColumnNames();
			for (DBLog::ColumnsVector::const_iterator it = customCols.begin(); it != customCols.end(); ++it)
				v.push_back(make_pair(string(), *it));

			ResultHTMLTable t(v, st.getForm(), _resultTableRequestParameters, _resultTableResultParameters, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();
			
			for (vector<shared_ptr<DBLogEntry> >::const_iterator it = _result.begin(); it != _result.end(); ++it)
			{
				shared_ptr<DBLogEntry> dbe = *it;
				stream << t.row();
				stream << t.col() << DBLogModule::getEntryLevelLabel(dbe->getLevel());
				stream << t.col() << dbe->getDate().toString();
				stream << t.col() << (dbe->getUser() ? dbe->getUser()->getLogin() : "");

				DBLog::ColumnsVector cols = _dbLog->parse(dbe->getContent());
				for (DBLog::ColumnsVector::const_iterator it = cols.begin(); it != cols.end(); ++it)
					stream << t.col() << *it;
			}
			
			stream << t.close();
		}

		bool DBLogViewer::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}
