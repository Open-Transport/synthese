
/** CalendarTemplateAdmin class implementation.
	@file CalendarTemplateAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "User.h"
#include "CalendarModule.h"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "ActionResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "CalendarRight.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementAddAction.h"
#include "RemoveObjectAction.hpp"
#include "CalendarTemplatePropertiesUpdateAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SearchFormHTMLTable.h"
#include "Profile.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarLinkTableSync.hpp"
#include "CalendarTemplateCleanAction.hpp"
#include "ImportableAdmin.hpp"
#include "BaseCalendarAdmin.hpp"

#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace calendar;
	using namespace html;
	using namespace security;
	using namespace db;
	using namespace impex;

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
		const string CalendarTemplateAdmin::TAB_DATASOURCE("tds");

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
			_requestParameters.setFromParametersMap(map, CalendarTemplateElementTableSync::COL_RANK);
			try
			{
				_calendar = Env::GetOfficialEnv().get<CalendarTemplate>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(...)
			{
				throw AdminParametersException("No such calendar");
			}
			if(!map.getDefault<string>(PARAMETER_RESULT_START).empty())
			{
				_resultStartDate = from_string(map.get<string>(PARAMETER_RESULT_START));
			}
			if(!map.getDefault<string>(PARAMETER_RESULT_END).empty())
			{
				_resultEndDate = from_string(map.get<string>(PARAMETER_RESULT_END));
			}
		}



		util::ParametersMap CalendarTemplateAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_calendar.get()) m.insert(Request::PARAMETER_OBJECT_ID, _calendar->getKey());
			return m;
		}



		void CalendarTemplateAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {

			////////////////////////////////////////////////////////////////////
			// TAB SOURCE
			if (openTabContent(stream, TAB_SOURCE))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<CalendarTemplatePropertiesUpdateAction, CalendarTemplateAdmin> updateRequest(_request, *this);
				updateRequest.getAction()->setCalendar(const_pointer_cast<CalendarTemplate>(_calendar));

				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.cell("Nom", pt.getForm().getTextInput(CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME, _calendar->getName()));
				stream << pt.cell(
					"Catégorie",
					pt.getForm().getSelectInput(
						CalendarTemplatePropertiesUpdateAction::PARAMETER_CATEGORY,
						CalendarTemplate::GetCategoriesList(),
						optional<CalendarTemplateCategory>(_calendar->getCategory())
				)	);
				stream << pt.cell(
					"Parent",
					pt.getForm().getSelectInput(
						CalendarTemplatePropertiesUpdateAction::PARAMETER_PARENT_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList(CalendarTemplatesAdmin::DEFAULT_TITLE, boost::optional<util::RegistryKeyType>(_calendar->getKey())),
						optional<RegistryKeyType>(_calendar->getParent(true) ? _calendar->getParent()->getKey() : RegistryKeyType(0))
				)	);
				stream << pt.close();

				stream << "<h1>Commandes</h1>";

				// Requests
				AdminActionFunctionRequest<CalendarTemplateElementAddAction,CalendarTemplateAdmin> addRequest(_request, *this);
				addRequest.getAction()->setCalendar(const_pointer_cast<CalendarTemplate>(_calendar));

				AdminActionFunctionRequest<CalendarTemplateCleanAction,CalendarTemplateAdmin> cleanRequest(_request, *this);
				cleanRequest.getAction()->setCalendar(_calendar);

				AdminActionFunctionRequest<RemoveObjectAction,CalendarTemplatesAdmin> removeCalendar(_request);
				removeCalendar.getAction()->setObjectId(_calendar->getKey());

				AdminActionFunctionRequest<RemoveObjectAction,CalendarTemplateAdmin> delRequest(_request, *this);

				AdminFunctionRequest<CalendarTemplateAdmin> searchRequest(_request, *this);

				AdminFunctionRequest<CalendarTemplateAdmin> goRequest(_request, *this);

				stream <<
					"<p>" <<
					HTMLModule::getLinkButton(cleanRequest.getURL(), "Vider le calendrier", "Etes-vous sûr de vouloir vider le calendrier "+ _calendar->getName() +" ?")
				;
				// Getting list of CalendarTemplates based on the current calendar
				CalendarTemplateElementTableSync::SearchResult result(
					CalendarTemplateElementTableSync::Search(
						_getEnv(), optional<RegistryKeyType>(), _calendar->getKey()
				)	);
				// Getting list of sons of the current calendar
				CalendarTemplateTableSync::SearchResult result1(
					CalendarTemplateTableSync::Search(
						_getEnv(),
						optional<std::string>(),
						optional<RegistryKeyType>(),
						false,
						false,
						0,
						optional<size_t>(),
						UP_LINKS_LOAD_LEVEL,
						_calendar->getKey()
				)	);
				// Getting list of links between the current calendar and Services
				CalendarLinkTableSync::SearchResult result2(
					CalendarLinkTableSync::Search(
								_getEnv(),
								boost::optional<util::RegistryKeyType>(),
								0,
								boost::optional<std::size_t>(),
								false,
								false,
								UP_LINKS_LOAD_LEVEL,
								boost::optional<util::RegistryKeyType>(_calendar->getKey()),
								boost::optional<util::RegistryKeyType>(),
								true
				)	);
				if(result.empty() && result1.empty() && result2.empty())
				{
					stream << " " <<
					HTMLModule::getLinkButton(removeCalendar.getURL(), "Supprimer le calendrier", "Etes-vous sûr de vouloir supprimer le calendrier "+ _calendar->getName() +" ?", "/admin/img/calendar_delete.png");
				}
				stream << "</p>";

				// Display
				CalendarTemplateElementTableSync::SearchResult elements(
					CalendarTemplateElementTableSync::Search(Env::GetOfficialEnv(), _calendar->getKey())
				);

				HTMLForm f(addRequest.getHTMLForm("add"));
				ActionResultHTMLTable::HeaderVector c;
				c.push_back(make_pair(CalendarTemplateElementTableSync::COL_RANK, "Rang"));
				c.push_back(make_pair(string(), "Sens"));
				c.push_back(make_pair(string(), "Date début"));
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
				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& ct, elements)
				{
					delRequest.getAction()->setObjectId(ct->getKey());
					nextRank = ct->getRank() + 1;

					stream << t.row(lexical_cast<string>(ct->getRank()));

					stream << t.col() << ct->getRank();
					stream << t.col() << static_cast<char>(ct->getOperation());

					stream << t.col() << (ct->getMinDate().is_special() ? "-&infin;" : to_simple_string(ct->getMinDate()));
					stream << t.col() << (ct->getMaxDate().is_special() ? "+&infin;" : to_simple_string(ct->getMaxDate()));
					stream << t.col() << ct->getStep().days();

					stream << t.col();
					if (ct->getInclude())
					{
						goRequest.getPage()->setCalendar(const_pointer_cast<CalendarTemplate>(Env::GetOfficialEnv().getSPtr(ct->getInclude())));
						stream << HTMLModule::getHTMLLink(goRequest.getURL(), ct->getInclude()->getName());
					}

					stream << t.col() << HTMLModule::getLinkButton(delRequest.getURL(), "Supprimer");
				}

				stream << t.row(lexical_cast<string>(nextRank));

				stream << t.col();

				vector<pair<optional<CalendarTemplateElement::Operation>, string> > addSub;
				addSub.push_back(make_pair(CalendarTemplateElement::ADD, "+"));
				addSub.push_back(make_pair(CalendarTemplateElement::SUB, "-"));
				addSub.push_back(make_pair(CalendarTemplateElement::AND, "&"));

				stream << t.col() << f.getSelectInput(
					CalendarTemplateElementAddAction::PARAMETER_POSITIVE,
					addSub,
					optional<CalendarTemplateElement::Operation>(CalendarTemplateElement::ADD)
				);
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MIN_DATE, string());
				stream << t.col() << f.getTextInput(CalendarTemplateElementAddAction::PARAMETER_MAX_DATE, string());
				stream << t.col() << f.getSelectNumberInput(CalendarTemplateElementAddAction::PARAMETER_INTERVAL, 1, 21);
				stream << t.col() << f.getSelectInput(
					CalendarTemplateElementAddAction::PARAMETER_INCLUDE_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)", optional<RegistryKeyType>(_calendar->getKey())),
					optional<RegistryKeyType>(0)
				);
				stream << t.col() << f.getSubmitButton("Ajouter");

				stream << t.close() << f.close();

				// Calendar checker
				stream << "<h1>Vérification de pertinence du calendrier</h1>";
				if(result.empty() && result1.empty() && result2.empty())
				{
					stream << "<p>Le calendrier n'est lié à aucun service, n'est inclus dans la définition d'aucun calendrier et ne possède aucun calendrier fils.</p>";
					stream << "<p>Une suppression du calendrier peut donc être effectuée.</p>";
				}
				else
				{

					stream << "<h2>Calendriers fils</h2>";
					if(result1.empty())
					{
						stream << "Le calendrier ne possède aucun calendrier fils.";
					}
					else
					{
						stream << "Le calendrier possède le(s) calendrier(s) fils suivant(s) :";
						stream << "<ul>";
						BOOST_FOREACH(const boost::shared_ptr<CalendarTemplate>& ct, result1)
						{
							stream << "<li>";
							goRequest.getPage()->setCalendar(const_pointer_cast<CalendarTemplate>(ct));
							stream << HTMLModule::getHTMLLink(goRequest.getURL(), ct->getName());
							stream << "</li>";
						}
						stream << "</ul>";
					}
					stream << "<h2>Services liés</h2>";
					if(result2.empty())
					{
						stream << "Le calendrier n'est rattaché à aucun service";
					}
					else
					{
						stream << "Le calendrier est rattaché au(x) service(s) suivant(s) :";
						stream << "<ul>";
						BOOST_FOREACH(const boost::shared_ptr<CalendarLink>& cl, result2)
						{
							stream << "<li>";
							stream << (cl->getCalendar() ? lexical_cast<string>(cl->getCalendar()->getKey()) : "Identifiant de service non trouvé");
							stream << "</li>";
						}
						stream << "</ul>";
					}
					stream << "<h2>Calendriers incluant le calendrier courant</h2>";
					if (result.empty())
					{
						stream << "Le calendrier n'est inclus dans la définition d'aucun autre calendrier.";
					}
					else
					{
						stream << "Le calendrier est inclus dans le(s) calendrier(s) suivant(s) :";
						stream << "<ul>";
						BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& ct, result)
						{
							stream << "<li>";
							if (ct->getCalendar())
							{
								goRequest.getPage()->setCalendar(const_pointer_cast<CalendarTemplate>(_getEnv().getSPtr(ct->getCalendar())));
								stream << HTMLModule::getHTMLLink(goRequest.getURL(), ct->getCalendar()->getName());
							}
							stream << "</li>";
						}
						stream << "</ul>";
					}
				}

				// Calendar add form
				stream << "<h1>Ajout de calendrier fils</h1>";

				AdminActionFunctionRequest<CalendarTemplatePropertiesUpdateAction,CalendarTemplateAdmin> addCalendar(_request);
				addCalendar.setActionFailedPage<CalendarTemplatesAdmin>();
				addCalendar.getAction()->setParent(const_pointer_cast<CalendarTemplate>(_calendar));
				addCalendar.setActionWillCreateObject();

				PropertiesHTMLTable addCalendarForm(addCalendar.getHTMLForm("addCalendar"));
				stream << addCalendarForm.open();
				stream << addCalendarForm.cell("Nom", addCalendarForm.getForm().getTextInput(CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME, string()));
				stream << addCalendarForm.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB RESULT
			if (openTabContent(stream, TAB_RESULT))
			{
				AdminFunctionRequest<CalendarTemplateAdmin> resultRequest(_request, *this);

				stream << "<h1>Recherche</h1>";

				SearchFormHTMLTable f(resultRequest.getHTMLForm());
				stream << f.open();
				stream << f.cell("Date min", f.getForm().getCalendarInput(PARAMETER_RESULT_START, _resultStartDate));
				stream << f.cell("Date max", f.getForm().getCalendarInput(PARAMETER_RESULT_END, _resultEndDate));
				stream << f.close();

				stream << "<h1>Résultat</h1>";

				Calendar mask(_resultStartDate, _resultEndDate);
				Calendar result(_calendar->getResult(mask));
				BaseCalendarAdmin::Display(stream, result);
			}

			////////////////////////////////////////////////////////////////////
			// TAB DATASOURCE
			if (openTabContent(stream, TAB_DATASOURCE))
			{
				// Source id
				StaticActionRequest<CalendarTemplatePropertiesUpdateAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setCalendar(const_pointer_cast<CalendarTemplate>(_calendar));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_calendar, updateOnlyRequest);
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool CalendarTemplateAdmin::isAuthorized(
			const security::User& user
		) const {
			return user.getProfile()->isAuthorized<CalendarRight>(READ);
		}



		std::string CalendarTemplateAdmin::getTitle() const
		{
			return _calendar.get() ? _calendar->getName() : DEFAULT_TITLE;
		}



		bool CalendarTemplateAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _calendar == static_cast<const CalendarTemplateAdmin&>(other)._calendar;
		}



		void CalendarTemplateAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Données", TAB_SOURCE, true));
			_tabs.push_back(Tab("Résultat", TAB_RESULT, true));
			_tabs.push_back(Tab("Sources de données", TAB_DATASOURCE, true));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks CalendarTemplateAdmin::getSubPages(
			const admin::AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const CalendarTemplatesAdmin*>(&currentPage) ||
				dynamic_cast<const CalendarTemplateAdmin*>(&currentPage)
			){

				// Subpages
				CalendarTemplateTableSync::SearchResult calendars(
					CalendarTemplateTableSync::Search(
						Env::GetOfficialEnv(),
						optional<string>(),
						optional<RegistryKeyType>(),
						true,
						true,
						0,
						optional<size_t>(),
						UP_LINKS_LOAD_LEVEL,
						_calendar->getKey()
				)	);
				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplate>& ct, calendars)
				{
					boost::shared_ptr<CalendarTemplateAdmin> page(
						getNewPage<CalendarTemplateAdmin>()
					);
					page->setCalendar(const_pointer_cast<const CalendarTemplate>(ct));
					links.push_back(page);
				}
			}

			return links;
		}
}	}
