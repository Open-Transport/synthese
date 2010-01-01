
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
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "ActionResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "CalendarRight.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateElementAddAction.h"
#include "CalendarTemplateElementRemoveAction.h"
#include "CalendarTemplatePropertiesUpdateAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "CalendarHTMLViewer.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SearchFormHTMLTable.h"
#include "Profile.h"

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
			_resultEndDate(day_clock::local_day().year() + 1, 12, 31)
		{ }
		
		void CalendarTemplateAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map.getMap(), CalendarTemplateElementTableSync::COL_RANK);
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
			ParametersMap m(_requestParameters.getParametersMap());
			if(_calendar.get()) m.insert(Request::PARAMETER_OBJECT_ID, _calendar->getKey());
			return m;
		}

		
		
		void CalendarTemplateAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const {

			////////////////////////////////////////////////////////////////////
			// TAB SOURCE
			if (openTabContent(stream, TAB_SOURCE))
			{
				stream << "<h1>Propri�t�s</h1>";

				AdminActionFunctionRequest<CalendarTemplatePropertiesUpdateAction, CalendarTemplateAdmin> updateRequest(_request);
				updateRequest.getAction()->setCalendar(const_pointer_cast<CalendarTemplate>(_calendar));

				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.cell("Nom", pt.getForm().getTextInput(CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME, _calendar->getText()));
				stream << pt.cell("Cat�gorie", pt.getForm().getSelectInput(CalendarTemplatePropertiesUpdateAction::PARAMETER_CATEGORY, CalendarTemplate::GetCategoriesList(), _calendar->getCategory()));
				stream << pt.close();

				stream << "<h1>Commandes</h1>";

				// Requests
				AdminActionFunctionRequest<CalendarTemplateElementAddAction,CalendarTemplateAdmin> addRequest(_request);
				addRequest.getAction()->setCalendar(const_pointer_cast<CalendarTemplate>(_calendar));

				AdminActionFunctionRequest<CalendarTemplateElementRemoveAction,CalendarTemplateAdmin> delRequest(_request);

				AdminFunctionRequest<CalendarTemplateAdmin> searchRequest(_request);

				AdminFunctionRequest<CalendarTemplateAdmin> goRequest(_request);
				
				// Display
				CalendarTemplateElementTableSync::SearchResult elements(
					CalendarTemplateElementTableSync::Search(_getEnv(), _calendar->getKey())
				);
				
				HTMLForm f(addRequest.getHTMLForm("add"));
				ActionResultHTMLTable::HeaderVector c;
				c.push_back(make_pair(CalendarTemplateElementTableSync::COL_RANK, "Rang"));
				c.push_back(make_pair(string(), "Sens"));
				c.push_back(make_pair(string(), "Date d�but"));
				c.push_back(make_pair(string(), "Date fin"));
				c.push_back(make_pair(string(), "Intervale"));
				c.push_back(make_pair(string(), "Inclusion"));
				c.push_back(make_pair(string(), "Action"));
				ActionResultHTMLTable t(
					c,
					searchRequest.getHTMLForm(),
					_requestParameters,
					elements,
					addRequest.getHTMLForm(),
					CalendarTemplateElementAddAction::PARAMETER_RANK
				);

				stream << f.open() << t.open();

				size_t nextRank(0);
				BOOST_FOREACH(shared_ptr<CalendarTemplateElement> ct, elements)
				{
					delRequest.getAction()->setElement(ct);
					nextRank = ct->getRank() + 1;
				
					stream << t.row(lexical_cast<string>(ct->getRank()));

					stream << t.col() << ct->getRank();
					stream << t.col() << static_cast<char>(ct->getOperation());
					
					stream << t.col() << (ct->getMinDate().is_special() ? "-&infin;" : to_simple_string(ct->getMinDate()));
					stream << t.col() << (ct->getMaxDate().is_special() ? "+&infin;" : to_simple_string(ct->getMaxDate()));
					stream << t.col() << ct->getInterval().days();
					
					stream << t.col();
					if (ct->getInclude())
					{
						goRequest.getPage()->setCalendar(const_pointer_cast<CalendarTemplate>(_getEnv().getSPtr(ct->getInclude())));
						stream << HTMLModule::getHTMLLink(goRequest.getURL(), ct->getInclude()->getText());
					}

					stream << t.col() << HTMLModule::getLinkButton(delRequest.getURL(), "Supprimer");
				}

				stream << t.row(lexical_cast<string>(nextRank));

				stream << t.col();

				vector<pair<CalendarTemplateElement::Operation, string> > addSub;
				addSub.push_back(make_pair(CalendarTemplateElement::ADD, "+"));
				addSub.push_back(make_pair(CalendarTemplateElement::SUB, "-"));
				addSub.push_back(make_pair(CalendarTemplateElement::AND, "&"));

				stream << t.col() << f.getSelectInput(CalendarTemplateElementAddAction::PARAMETER_POSITIVE, addSub, CalendarTemplateElement::ADD);
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MIN_DATE, string());
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MAX_DATE, string());
				stream << t.col() << f.getSelectNumberInput(CalendarTemplateElementAddAction::PARAMETER_INTERVAL, 1, 21);
				stream << t.col() << f.getSelectInput(
					CalendarTemplateElementAddAction::PARAMETER_INCLUDE_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)", _calendar->getKey()),
					RegistryKeyType(0)
				);
				stream << t.col() << f.getSubmitButton("Ajouter");

				stream << t.close() << f.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB RESULT
			if (openTabContent(stream, TAB_RESULT))
			{
				AdminFunctionRequest<CalendarTemplateAdmin> resultRequest(_request);

				stream << "<h1>Recherche</h1>";

				SearchFormHTMLTable f(resultRequest.getHTMLForm());
				stream << f.open();
				stream << f.cell("Date min", f.getForm().getCalendarInput(PARAMETER_RESULT_START, _resultStartDate));
				stream << f.cell("Date max", f.getForm().getCalendarInput(PARAMETER_RESULT_END, _resultEndDate));
				stream << f.close();

				stream << "<h1>R�sultat</h1>";
				
				Calendar mask(_resultStartDate, _resultEndDate);
				Calendar result(_calendar->getResult(mask));
				CalendarHTMLViewer v(result);
				v.display(stream);
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		bool CalendarTemplateAdmin::isAuthorized(
			const security::Profile& profile
		) const {
			return profile.isAuthorized<CalendarRight>(READ);
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



		void CalendarTemplateAdmin::_buildTabs( const admin::AdminRequest& request ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Donn�es", TAB_SOURCE, true));
			_tabs.push_back(Tab("R�sultat", TAB_RESULT, true));

			_tabBuilded = true;
		}
	}
}
