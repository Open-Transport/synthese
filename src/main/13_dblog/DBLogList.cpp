
/** DBLogList class implementation.
	@file DBLogList.cpp

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

#include "01_util/Html.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogList.h"
#include "13_dblog/DBLogViewer.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace dblog
	{
		DBLogList::DBLogList()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		void DBLogList::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
		}

		string DBLogList::getTitle() const
		{
			return "Journaux";
		}

		void DBLogList::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* goRequest = Factory<Request>::create<AdminRequest>();
			goRequest->copy(request);
			goRequest->setPage(Factory<AdminInterfaceElement>::create<DBLogViewer>());

			stream
				<< "<h1>Liste des journaux</h1>"
				<< "<table>";

			for (Factory<DBLog>::Iterator it = Factory<DBLog>::begin(); it != Factory<DBLog>::end(); ++it)
			{
				stream
					<< "<tr>"
					<< "<td>" << it->getName() << "</td>"
					<< "<td>" << goRequest->getHTMLFormHeader(it.getKey())
					<< Html::getHiddenInput(DBLogViewer::PARAMETER_LOG_KEY, it.getKey())
					<< Html::getSubmitButton("Consulter") << "</form></td>"
					<< "</tr>";
			}
			stream << "</table>";

			delete goRequest;
		}
	}
}
