
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
#include "CalendarModule.h"

#include "HTMLForm.h"
#include "ResultHTMLTable.h"

#include "CalendarRight.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateElementAddAction.h"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"

#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace calendar;
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

	namespace calendar
	{
		const string CalendarTemplateAdmin::TAB_RESULT("trs");
		const string CalendarTemplateAdmin::TAB_SOURCE("tsr");
		const string CalendarTemplateAdmin::PARAMETER_RESULT_START("prs");
		const string CalendarTemplateAdmin::PARAMETER_RESULT_END("pre");

		CalendarTemplateAdmin::CalendarTemplateAdmin()
		:	AdminInterfaceElementTemplate<CalendarTemplateAdmin>(),
			_resultStartDate(day_clock::local_day().year(), 1, 1),
			_resultEndDate(day_clock::local_day().year(), 12, 31)
		{ }
		
		void CalendarTemplateAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(objectWillBeCreatedLater) return;
			
			try
			{
				_calendar = CalendarTemplateTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv()
				);
			}
			catch(...)
			{
				throw AdminParametersException("No such calendar");
			}
			if(map.getOptional<string>(PARAMETER_RESULT_START))
			{
				_resultStartDate = from_string(get<string>(PARAMETER_RESULT_START));
			}
			if(map.getOptional<string>(PARAMETER_RESULT_END))
			{
				_resultEndDate = from_string(get<string>(PARAMETER_RESULT_END));
			}
		}
		
		
		
		server::ParametersMap CalendarTemplateAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_calendar.get()) m.insert(Request::PARAMETER_OBJECT_ID, _calendar->getKey());
			return m;
		}

		
		
		void CalendarTemplateAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {

			////////////////////////////////////////////////////////////////////
			// TAB SOURCE
			if (openTabContent(stream, TAB_SOURCE))
			{

				// Requests
				AdminActionFunctionRequest<CalendarTemplateElementAddAction,CalendarTemplateAdmin> addRequest(_request);
				addRequest.getAction()->setCalendar(_calendar);

				// Display
				CalendarTemplateElementTableSync::SearchResult elements(
					CalendarTemplateElementTableSync::Search(_getEnv(), _calendar->getKey())
				);
				
				HTMLForm f(addRequest.getHTMLForm("add"));
				HTMLTable::ColsVector c;
				c.push_back("Rang");
				c.push_back("Sens");
				c.push_back("Date début");
				c.push_back("Date fin");
				c.push_back("Intervale");
				c.push_back("Inclusion");
				c.push_back("Action");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

				stream << f.open() << t.open();

				BOOST_FOREACH(shared_ptr<CalendarTemplateElement> ct, elements)
				{
					stream << t.row();

					stream << t.col() << ct->getRank();
					stream << t.col() << (ct->getPositive() ? "+" : "-");
					
					stream << t.col() << ct->getMinDate();
					stream << t.col() << ct->getMaxDate();
					stream << t.col() << ct->getInterval().days();
					
					stream << t.col();
					if (ct->getIncludeId())
					{
						stream << *ct->getIncludeId();
					}

					stream << t.col() << HTMLModule::getLinkButton(string(), "Supprimer");
				}

				stream << t.row();

				vector<pair<bool, string> > addSub;
				addSub.push_back(make_pair(true, "+"));
				addSub.push_back(make_pair(false, "-"));

				stream << t.col() << f.getSelectInput(CalendarTemplateElementAddAction::PARAMETER_POSITIVE, addSub, true);
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MIN_DATE, string());
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MAX_DATE, string());
				stream << t.col() << f.getSelectNumberInput(CalendarTemplateElementAddAction::PARAMETER_INTERVAL, 1, 21);
				stream << t.col() << f.getSubmitButton("Ajouter");

				stream << t.close() << f.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB RESULT
			if (openTabContent(stream, TAB_RESULT))
			{
				AdminFunctionRequest<CalendarTemplateAdmin> resultRequest(_request);
				
			}
		}

		bool CalendarTemplateAdmin::isAuthorized(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			return _request.isAuthorized<CalendarRight>(READ);
		}
		


		std::string CalendarTemplateAdmin::getTitle() const
		{
			return _calendar.get() ? _calendar->getText() : DEFAULT_TITLE;
		}
		
		void CalendarTemplateAdmin::setCalendar(shared_ptr<CalendarTemplate> value)
		{
			_calendar = const_pointer_cast<const CalendarTemplate>(value);
		}
		
		boost::shared_ptr<const CalendarTemplate> CalendarTemplateAdmin::getCalendar() const
		{
			return _calendar;
		}
		
		bool CalendarTemplateAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _calendar == static_cast<const CalendarTemplateAdmin&>(other)._calendar;
		}



		void CalendarTemplateAdmin::_buildTabs( const server::FunctionRequest<admin::AdminRequest>& request ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Source", TAB_SOURCE, true));
			_tabs.push_back(Tab("Résultat", TAB_RESULT, true));

			_tabBuilded = true;
		}
	}
}
