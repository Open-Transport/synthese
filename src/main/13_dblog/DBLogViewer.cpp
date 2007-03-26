
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

#include "01_util/Html.h"
#include "01_util/Constants.h"

#include "11_interfaces/InterfaceModule.h"

#include "12_security/SecurityModule.h"
#include "12_security/User.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogViewer.h"
#include "13_dblog/DBLogModule.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminModule.h"
#include "32_admin/ResultHTMLTable.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;

	namespace dblog
	{
		const string DBLogViewer::PARAMETER_LOG_KEY = "dlvk";
		const string DBLogViewer::PARAMETER_SEARCH_USER = "dlvsu";
		const string DBLogViewer::PARAMETER_SEARCH_TYPE = "dlvst";

		DBLogViewer::DBLogViewer()
			: AdminInterfaceElement("dblogs", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _dbLog(NULL)
			, _searchUser(UNKNOWN_VALUE)
			, _searchType(UNKNOWN_VALUE)
		{}

		void DBLogViewer::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(PARAMETER_LOG_KEY);
			if (it == map.end())
				throw AdminParametersException("Log key not specified");
			try
			{
				_dbLog = Factory<DBLog>::create(it->second);
			}
			catch (FactoryException<DBLog> e)
			{
				throw AdminParametersException("Invalid log key : " + it->second);
			}

			it = map.find(PARAMETER_SEARCH_USER);
			if (it != map.end())
				_searchUser = Conversion::ToLongLong(it->second);

			it = map.find(PARAMETER_SEARCH_TYPE);
			if (it != map.end())
				_searchType = Conversion::ToInt(it->second);
		}

		string DBLogViewer::getTitle() const
		{
			return (_dbLog != NULL) ? _dbLog->getName() : "(pas de journal)";
		}

		void DBLogViewer::display(ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<DBLogViewer>());

			stream << "<h1>Recherche d'entrées</h1>";

			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< Html::getHiddenInput(PARAMETER_LOG_KEY, _logKey)
				<< "<table>"
				<< "<tr><td>Date début</td><td>" << Html::getTextInput("", "") << "</td>"
				<< "<td>Date fin</td><td>" <<  Html::getTextInput("", "") << "</td>"
				<< "<td>Utilisateur</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_USER, SecurityModule::getUserLabels(true), _searchUser) << "</td></tr>" // Users list
				<< "<tr><td>Type</td><td>" << Html::getSelectInput(PARAMETER_SEARCH_TYPE, DBLogModule::getEntryLevelLabels(true), _searchType) << "</td>" // Types list 
				<< "<td>Texte</td><td>" << Html::getTextInput("", "") << "</td>"
				<< "<td coslpan=\"2\">" << Html::getSubmitButton("Rechercher") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Résultat de la recherche</h1>";

			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v.push_back(make_pair(string(), "Date"));
			v.push_back(make_pair(PARAMETER_SEARCH_USER, "Utilisateur"));
			DBLog::ColumnsVector customCols = _dbLog->getColumnNames();
			for (DBLog::ColumnsVector::const_iterator it = customCols.begin(); it != customCols.end(); ++it)
				v.push_back(make_pair(string(), *it));

			ResultHTMLTable t(v, searchRequest, string(), true, NULL, string(), InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));

			stream << t.open();
			
			for (vector<DBLogEntry*>::const_iterator it = _result.begin(); it != _result.end(); ++it)
			{
				DBLogEntry* dbe = *it;
				stream << t.row();
				stream << t.col() << DBLogModule::getEntryLevelLabel(dbe->getLevel());
				stream << t.col() << dbe->getDate().toString();
				stream << t.col() << dbe->getUser() ? dbe->getUser()->getLogin() : "";

				DBLog::ColumnsVector cols = _dbLog->parse(dbe->getContent());
				for (DBLog::ColumnsVector::const_iterator it = cols.begin(); it != cols.end(); ++it)
					stream << t.col() << *it;
				
				delete dbe;
			}
			
			stream << t.close();
		}
	}
}
