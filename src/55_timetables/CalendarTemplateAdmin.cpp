
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

#include "HTMLForm.h"
#include "ResultHTMLTable.h"

#include "TimetableRight.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateElementAddAction.h"

#include "Request.h"
#include "AdminRequest.h"
#include "ActionFunctionRequest.h"

#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

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
		
		void CalendarTemplateAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			if (id == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION) return;

			try
			{
				_calendar = CalendarTemplateTableSync::Get(id, _getEnv());
			}
			catch(...)
			{
				throw AdminParametersException("No such calendar");
			}
			
			if(!doDisplayPreparationActions) return;

			CalendarTemplateElementTableSync::Search(_getEnv(), _calendar->getKey());
		}
		
		
		
		server::ParametersMap CalendarTemplateAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}

		
		
		void CalendarTemplateAdmin::display(
			ostream& stream,
			VariablesMap& variables
		) const {
			// Requests
			ActionFunctionRequest<CalendarTemplateElementAddAction,AdminRequest> addRequest(_request);
			addRequest.getFunction()->setPage<CalendarTemplateAdmin>();
			addRequest.setObjectId(_calendar->getKey());
			addRequest.getAction()->setCalendarId(_calendar->getKey());

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

			BOOST_FOREACH(shared_ptr<CalendarTemplateElement> ct, _getEnv().getRegistry<CalendarTemplateElement>()) 
			{
				stream << t.row();

				stream << t.col() << ct->getRank();
				stream << t.col() << ct->getPositive();
				
				if (ct->getIncludeId() != UNKNOWN_VALUE)
				{
					stream << t.col() << ct->getMinDate().toString();
					stream << t.col() << ct->getMaxDate().toString();
					stream << t.col() << ct->getInterval();
				}
				else
				{
					stream << t.col(3) << "Inclusion de " << ct->getIncludeId();
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

		bool CalendarTemplateAdmin::isAuthorized(
		) const {
			return _request->isAuthorized<TimetableRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplateAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}
		
		AdminInterfaceElement::PageLinks CalendarTemplateAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
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
			return _calendar.get() ? Request::PARAMETER_OBJECT_ID : string();
		}

		std::string CalendarTemplateAdmin::getParameterValue() const
		{
			return _calendar.get() ? Conversion::ToString(_calendar->getKey()) : string();
		}
	}
}
