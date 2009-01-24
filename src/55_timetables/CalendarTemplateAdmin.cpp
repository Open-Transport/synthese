
/** CalendarTemplateAdmin class implementation.
	@file CalendarTemplateAdmin.cpp
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

#include "CalendarTemplateAdmin.h"
#include "CalendarTemplatesAdmin.h"
#include "TimetableModule.h"

#include "05_html/HTMLForm.h"
#include "05_html/ResultHTMLTable.h"

#include "35_timetables/TimetableRight.h"
#include "35_timetables/CalendarTemplate.h"
#include "35_timetables/CalendarTemplateTableSync.h"
#include "35_timetables/CalendarTemplateElement.h"
#include "35_timetables/CalendarTemplateElementTableSync.h"
#include "35_timetables/CalendarTemplateElementAddAction.h"

#include "30_server/QueryString.h"
#include "30_server/Request.h"
#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

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
		template<> const string FactorableTemplate<AdminInterfaceElement, CalendarTemplateAdmin>::FACTORY_KEY("CalendarTemplateAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CalendarTemplateAdmin>::ICON("calendar.png");
		template<> const string AdminInterfaceElementTemplate<CalendarTemplateAdmin>::DEFAULT_TITLE("Calendrier inconnu");
	}

	namespace timetables
	{
		CalendarTemplateAdmin::CalendarTemplateAdmin()
			: AdminInterfaceElementTemplate<CalendarTemplateAdmin>()
		{ }
		
		void CalendarTemplateAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_calendar = CalendarTemplateTableSync::Get(id);
			}
			catch(...)
			{
				throw AdminParametersException("No such calendar");
			}
		}
		
		void CalendarTemplateAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			ActionFunctionRequest<CalendarTemplateElementAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<CalendarTemplateAdmin>();
			addRequest.setObjectId(_calendar->getKey());
			addRequest.getAction()->setCalendarId(_calendar->getKey());

			// Search
			vector<shared_ptr<CalendarTemplateElement> > elements(CalendarTemplateElementTableSync::Search(_calendar->getKey()));

			// Display
			HTMLForm f(addRequest.getHTMLForm("add"));
			HTMLTable::ColsVector c;
			c.push_back(string());
			c.push_back("Sens");
			c.push_back("Date début");
			c.push_back("Date fin");
			c.push_back("Intervalle");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open() << t.open();

			for (vector<shared_ptr<CalendarTemplateElement> >::const_iterator it(elements.begin()); it != elements.end(); ++it)
			{
				stream << t.row();

				stream << t.col() << (*it)->getRank();
				stream << t.col() << (*it)->getPositive();
				
				if ((*it)->getIncludeId() != UNKNOWN_VALUE)
				{
					stream << t.col() << (*it)->getMinDate().toString();
					stream << t.col() << (*it)->getMaxDate().toString();
					stream << t.col() << (*it)->getInterval();
				}
				else
				{
					stream << t.col(3) << "Inclusion de " << (*it)->getIncludeId();
				}
				stream << t.col() << HTMLModule::getLinkButton(string(), "Supprimer");
			}

			stream << t.row();

			stream << t.col();
			stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MIN_DATE, string());
			stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MAX_DATE, string());
			stream << t.col() << f.getSelectNumberInput(CalendarTemplateElementAddAction::PARAMETER_INTERVAL, 1, 21);

			stream << t.close() << f.close();
		}

		bool CalendarTemplateAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplateAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplateAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}


		std::string CalendarTemplateAdmin::getTitle() const
		{
			return _calendar.get() ? _calendar->getText() : DEFAULT_TITLE;
		}

		std::string CalendarTemplateAdmin::getParameterName() const
		{
			return _calendar.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string CalendarTemplateAdmin::getParameterValue() const
		{
			return _calendar.get() ? Conversion::ToString(_calendar->getKey()) : string();
		}
	}
}
