
/** CalendarTemplatesAdmin class implementation.
	@file CalendarTemplatesAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "CalendarTemplatesAdmin.h"
#include "TimetableModule.h"

#include "05_html/ResultHTMLTable.h"
#include "05_html/HTMLModule.h"
#include "05_html/HTMLForm.h"

#include "35_timetables/CalendarTemplateAdmin.h"
#include "35_timetables/CalendarTemplateTableSync.h"
#include "35_timetables/CalendarTemplateAddAction.h"
#include "35_timetables/TimetableRight.h"

#include "30_server/QueryString.h"
#include "30_server/Request.h"
#include "30_server/ActionFunctionRequest.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"
#include "32_admin/AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace timetables;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CalendarTemplatesAdmin>::FACTORY_KEY("CalendarTemplatesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CalendarTemplatesAdmin>::ICON("calendar.png");
		template<> const string AdminInterfaceElementTemplate<CalendarTemplatesAdmin>::DEFAULT_TITLE("Calendriers");
	}

	namespace timetables
	{
		CalendarTemplatesAdmin::CalendarTemplatesAdmin()
			: AdminInterfaceElementTemplate<CalendarTemplatesAdmin>()
		{ }
		
		void CalendarTemplatesAdmin::setFromParametersMap(const ParametersMap& map)
		{
		}
		
		void CalendarTemplatesAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> editCalendar(request);
			editCalendar.getFunction()->setPage<CalendarTemplateAdmin>();

			ActionFunctionRequest<CalendarTemplateAddAction,AdminRequest> addCalendar(request);
			addCalendar.getFunction()->setPage<CalendarTemplateAdmin>();
			addCalendar.getFunction()->setActionFailedPage<CalendarTemplatesAdmin>();
			addCalendar.setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
			
			// Search
			vector<shared_ptr<CalendarTemplate> > calendars(CalendarTemplateTableSync::Search());

			// Display
			stream << "<h1>Calendriers</h1>";

			HTMLForm f(addCalendar.getHTMLForm("add"));
			HTMLTable::ColsVector c;
			c.push_back("Description");
			c.push_back("Action");
			HTMLTable t(c,ResultHTMLTable::CSS_CLASS);
			stream << f.open() << t.open();

			for (vector<shared_ptr<CalendarTemplate> >::const_iterator it(calendars.begin()); it != calendars.end(); ++it)
			{
				editCalendar.setObjectId((*it)->getKey());

				stream << t.row();
				stream << t.col() << (*it)->getText();
				stream << t.col() << HTMLModule::getLinkButton(editCalendar.getURL(), "Modifier", string(), "calendar_edit.png");
			}

			stream << t.row();
			stream << t.col() << f.getTextInput(CalendarTemplateAddAction::PARAMETER_TEXT, string(), "(nom du calendrier)");
			stream << t.col() << f.getSubmitButton("Ajouter");

			stream << t.close() << f.close();
		}

		bool CalendarTemplatesAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == TimetableModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;

			vector<shared_ptr<CalendarTemplate> > calendars(CalendarTemplateTableSync::Search());
			for (vector<shared_ptr<CalendarTemplate> >::const_iterator it(calendars.begin()); it != calendars.end(); ++it)
			{
				AdminInterfaceElement::PageLink link;
				link.factoryKey = CalendarTemplateAdmin::FACTORY_KEY;
				link.icon = CalendarTemplateAdmin::ICON;
				link.name = (*it)->getText();
				link.parameterName = QueryString::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString((*it)->getKey());
				links.push_back(link);
			}

			return links;
		}
	}
}
