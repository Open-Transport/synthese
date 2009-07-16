
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

#include "ResultHTMLTable.h"
#include "HTMLModule.h"
#include "HTMLForm.h"

#include "CalendarTemplateAdmin.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateAddAction.h"
#include "TimetableRight.h"

#include "Request.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"

#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"

#include <boost/foreach.hpp>

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
		
		void CalendarTemplatesAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){

		}
		
		
		
		ParametersMap CalendarTemplatesAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}		
		
		
		void CalendarTemplatesAdmin::display(
			ostream& stream,
			VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			// Requests

			AdminActionFunctionRequest<CalendarTemplateAddAction,CalendarTemplateAdmin> addCalendar(_request);
			addCalendar.getFunction()->setActionFailedPage<CalendarTemplatesAdmin>();
			addCalendar.setActionWillCreateObject();
			
			
			// Display
			stream << "<h1>Calendriers</h1>";

			CalendarTemplateTableSync::SearchResult calendars(
				CalendarTemplateTableSync::Search(_getEnv())
			);

			HTMLForm f(addCalendar.getHTMLForm("add"));
			HTMLTable::ColsVector c;
			c.push_back("Description");
			c.push_back("Action");
			HTMLTable t(c,ResultHTMLTable::CSS_CLASS);
			stream << f.open() << t.open();

			AdminFunctionRequest<CalendarTemplateAdmin> editCalendar(_request);
			BOOST_FOREACH(
				shared_ptr<CalendarTemplate> ct,
				calendars
			){
				editCalendar.getPage()->setCalendar(ct);

				stream << t.row();
				stream << t.col() << ct->getText();
				stream <<
					t.col() <<
					HTMLModule::getLinkButton(
						editCalendar.getURL(),
						"Ouvrir",
						string(),
						"calendar_edit.png"
					)
				;
			}

			stream << t.row();
			stream <<
				t.col() <<
				f.getTextInput(CalendarTemplateAddAction::PARAMETER_TEXT, string(), "(nom du calendrier)")
			;
			stream << t.col() << f.getSubmitButton("Ajouter");

			stream << t.close() << f.close();
		}

		bool CalendarTemplatesAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return _request.isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == TimetableModule::FACTORY_KEY &&
				isAuthorized(request)
			){
				const CalendarTemplatesAdmin* ta(
					dynamic_cast<const CalendarTemplatesAdmin*>(currentPage.get())
				);

				if(ta)
				{
					AddToLinks(links, currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
				}
			}
			return links;
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const {
			AdminInterfaceElement::PageLinks links;

			const CalendarTemplateAdmin* ta(
				dynamic_cast<const CalendarTemplateAdmin*>(currentPage.get())
			);

			CalendarTemplateTableSync::SearchResult calendars(
				CalendarTemplateTableSync::Search(*_env)
			);
			BOOST_FOREACH(shared_ptr<CalendarTemplate> ct, calendars)
			{
				if(	ta &&
					ta->getCalendar().get() &&
					ta->getCalendar()->getKey() == ct->getKey()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					shared_ptr<CalendarTemplateAdmin> p(
					 	getNewOtherPage<CalendarTemplateAdmin>()
					);
					p->setCalendar(ct);
					AddToLinks(links, p);
				}
			}

			return links;
		}
	}
}
