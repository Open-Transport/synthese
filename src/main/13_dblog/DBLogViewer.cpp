
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

#include "12_security/SecurityModule.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogViewer.h"
#include "13_dblog/DBLogModule.h"

#include "32_admin/AdminParametersException.h"

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

			stream
				<< "<h1>Recherche d'entrées</h1>"
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

				<< "<h1>Résultat de la recherche</h1>"
				<< "<table>"
				<< "<tr><th rowspan=\"2\">Type</th><th rowspan=\"2\">Date</th><th rowspan=\"2\">Utilisateur</th><th colspan=\"" << 2 /* number of columns of the log */ << "\">Entrée</th></tr>"
				<< "<tr>";

			// Columns loop
			{
				stream << "<th></th>";	// Column name
			}
			stream << "</tr>";
			
			// Entries Loop
			{
				stream
					<< "<tr><td>Info</td>"
					<< "<td>21/10/2006 13:30</td>"
					<< "<td>Administrateur</td>";

				// Columns content loop
				{
					stream
						<< "<td>Création</td>";
				}
				stream << "</tr>";
			}
			
			stream
				<< "</table>"
				<< "<p align=\"right\">Entrées&nbsp;suivantes &gt;</p>";
		}
	}
}
