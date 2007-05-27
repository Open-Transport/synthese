
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

#include "32_admin/HomeAdmin.h"

#include <boost/shared_ptr.hpp>

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogList.h"
#include "13_dblog/DBLogViewer.h"
#include "13_dblog/DBLogRight.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace dblog;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DBLogList>::FACTORY_KEY("dblogs");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DBLogList>::ICON("book.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<DBLogList>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
		template<> string AdminInterfaceElementTemplate<DBLogList>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace dblog
	{
		void DBLogList::setFromParametersMap(const ParametersMap& map)
		{
		}

		string DBLogList::getTitle() const
		{
			return "Journaux";
		}

		void DBLogList::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> goRequest(request);
			goRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DBLogViewer>());

			stream << "<h1>Liste des journaux</h1>";

			HTMLTable t;
			stream << t.open();
			HTMLForm form(goRequest.getHTMLForm());

			for (Factory<DBLog>::Iterator it = Factory<DBLog>::begin(); it != Factory<DBLog>::end(); ++it)
			{
				form.addHiddenField(DBLogViewer::PARAMETER_LOG_KEY, it.getKey());
				stream << t.row();
				stream << t.col() << it->getName();
				stream << t.col() << form.getLinkButton("Consulter");
			}
			stream << t.close();
		}

		bool DBLogList::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return request->isAuthorized<DBLogRight>(READ);
		}

		DBLogList::DBLogList()
			: AdminInterfaceElementTemplate<DBLogList>()
		{

		}
	}
}
