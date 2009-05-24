
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
#include "Request.h"
#include "ActionFunctionRequest.h"

#include "ModuleAdmin.h"
#include "AdminRequest.h"
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
			bool doDisplayPreparationActions
		){
			if(!doDisplayPreparationActions) return;

			CalendarTemplateTableSync::Search(_env);
		}
		
		
		
		ParametersMap CalendarTemplatesAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}		
		
		
		void CalendarTemplatesAdmin::display(
			ostream& stream,
			VariablesMap& variables
		) const {
			// Requests
			FunctionRequest<AdminRequest> editCalendar(_request);
			editCalendar.getFunction()->setPage<CalendarTemplateAdmin>();

			ActionFunctionRequest<CalendarTemplateAddAction,AdminRequest> addCalendar(_request);
			addCalendar.getFunction()->setPage<CalendarTemplateAdmin>();
			addCalendar.getFunction()->setActionFailedPage<CalendarTemplatesAdmin>();
			addCalendar.setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
			
			
			// Display
			stream << "<h1>Calendriers</h1>";

			HTMLForm f(addCalendar.getHTMLForm("add"));
			HTMLTable::ColsVector c;
			c.push_back("Description");
			c.push_back("Action");
			HTMLTable t(c,ResultHTMLTable::CSS_CLASS);
			stream << f.open() << t.open();

			BOOST_FOREACH(shared_ptr<CalendarTemplate> ct, _env.getRegistry<CalendarTemplate>())
			{
				editCalendar.setObjectId(ct->getKey());

				stream << t.row();
				stream << t.col() << ct->getText();
				stream <<
					t.col() <<
					HTMLModule::getLinkButton(
						editCalendar.getURL(),
						"Modifier",
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

		bool CalendarTemplatesAdmin::isAuthorized() const
		{
			return _request->isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == TimetableModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplatesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;

			Env env;
			CalendarTemplateTableSync::Search(env);
			BOOST_FOREACH(shared_ptr<CalendarTemplate> ct, env.getRegistry<CalendarTemplate>())
			{
				AdminInterfaceElement::PageLink link(getPageLink());
				link.factoryKey = CalendarTemplateAdmin::FACTORY_KEY;
				link.icon = CalendarTemplateAdmin::ICON;
				link.name = ct->getText();
				link.parameterName = Request::PARAMETER_OBJECT_ID;
				link.parameterValue = Conversion::ToString(ct->getKey());
				links.push_back(link);
			}

			return links;
		}
	}
}
