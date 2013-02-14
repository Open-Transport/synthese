////////////////////////////////////////////////////////////////////////////////
/// CommercialLineAdmin class implementation.
///	@file CommercialLineAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "CommercialLineAdmin.h"

#include "TransportNetworkAdmin.h"
#include "PTModule.h"
#include "User.h"
#include "NonConcurrencyRuleTableSync.h"
#include "NonConcurrencyRule.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternAdmin.hpp"
#include "JourneyPatternTableSync.hpp"
#include "TransportNetworkRight.h"
#include "AdminInterfaceElement.h"
#include "AdminFunctionRequest.hpp"
#include "NonConcurrencyRuleAddAction.h"
#include "AdminParametersException.h"
#include "SearchFormHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "NonPermanentService.h"
#include "Profile.h"
#include "JourneyPatternAddAction.hpp"
#include "CalendarTemplateTableSync.h"
#include "CommercialLineCalendarTemplateUpdateAction.hpp"
#include "CommercialLineUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "ReservationContact.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineArea.hpp"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "PTRuleUserAdmin.hpp"
#include "ActionResultHTMLTable.h"
#include "AdminModule.h"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "RemoveObjectAction.hpp"
// #include "TridentFileFormat.h"
#include "ExportFunction.hpp"
#include "ImportableAdmin.hpp"
#include "DRTArea.hpp"
#include "JourneyPatternCopy.hpp"
#include "FreeDRTAreaTableSync.hpp"
#include "FreeDRTAreaUpdateAction.hpp"
#include "FreeDRTAreaAdmin.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
//	using namespace data_exchange;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace html;
	using namespace db;
	using namespace calendar;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CommercialLineAdmin>::FACTORY_KEY("CommercialLineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace pt
	{
		const string CommercialLineAdmin::TAB_DATES("da");
		const string CommercialLineAdmin::TAB_ROUTES_FORWARD("rof");
		const string CommercialLineAdmin::TAB_ROUTES_BACKWARD("rob");
		const string CommercialLineAdmin::TAB_FREE_DRT("drt");
		const string CommercialLineAdmin::TAB_NON_CONCURRENCY("nc");
		const string CommercialLineAdmin::TAB_PROPERTIES("pr");
		const string CommercialLineAdmin::TAB_EXPORT("ex");

		const string CommercialLineAdmin::PARAMETER_SEARCH_NAME("na");
		const string CommercialLineAdmin::PARAMETER_DATES_START("ds");
		const string CommercialLineAdmin::PARAMETER_DATES_END("de");
		const string CommercialLineAdmin::PARAMETER_CALENDAR_CONTROL("cc");



		CommercialLineAdmin::CommercialLineAdmin(
		):	_controlCalendar(false)
		{}



		void CommercialLineAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);
			if(!map.getDefault<string>(PARAMETER_DATES_START).empty()
			){
				_startDate = from_string(map.get<string>(PARAMETER_DATES_START));
			}
			if(	!map.getDefault<string>(PARAMETER_DATES_END).empty()
			){
				_endDate = from_string(map.get<string>(PARAMETER_DATES_END));
			}

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 100);

			try
			{
				_cline = Env::GetOfficialEnv().get<CommercialLine>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (...)
			{
				throw AdminParametersException("No such line");
			}

			_controlCalendar = map.getDefault<bool>(PARAMETER_CALENDAR_CONTROL, false);
		}



		util::ParametersMap CommercialLineAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_cline.get()) m.insert(Request::PARAMETER_OBJECT_ID, _cline->getKey());
			m.insert(PARAMETER_CALENDAR_CONTROL, _controlCalendar);
			return m;
		}



		void CommercialLineAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {

			////////////////////////////////////////////////////////////////////
			// TAB FORWARD ROUTES
			if (openTabContent(stream, TAB_ROUTES_FORWARD))
			{
				_displayRoutes(stream, _request, false);
			}


			////////////////////////////////////////////////////////////////////
			// TAB BACKWARD ROUTES
			if (openTabContent(stream, TAB_ROUTES_BACKWARD))
			{
				_displayRoutes(stream, _request, true);
			}


			////////////////////////////////////////////////////////////////////
			// TAB FREE DRT AREAS
			if (openTabContent(stream, TAB_FREE_DRT))
			{
				// Title
				stream << "<h1>Zones TAD libéralisé</h1>";

				// Declarations
				AdminFunctionRequest<FreeDRTAreaAdmin> openRequest(_request);
				AdminActionFunctionRequest<RemoveObjectAction, CommercialLineAdmin> removeRequest(_request, *this);

				// Search for areas
				AdminFunctionRequest<CommercialLineAdmin> searchRequest(_request, *this);
				FreeDRTAreaTableSync::SearchResult areas(
					FreeDRTAreaTableSync::Search(
						Env::GetOfficialEnv(),
						_cline->getKey(),
						_requestParameters.first,
						_requestParameters.maxSize,
						_requestParameters.orderField == PARAMETER_SEARCH_NAME,
						_requestParameters.raisingOrder
				)	);

				// Add request and form
				AdminActionFunctionRequest<FreeDRTAreaUpdateAction,FreeDRTAreaAdmin> addRequest(_request);
				addRequest.setActionWillCreateObject();
				addRequest.getAction()->setLine(const_pointer_cast<CommercialLine>(_cline));
				HTMLForm addForm(addRequest.getHTMLForm("addFreeDRT"));
				stream << addForm.open();

				// Table initialization
				ResultHTMLTable::HeaderVector cols;
				cols.push_back(make_pair(string(), string()));
				cols.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				cols.push_back(make_pair(string(), "Périmètre"));
				cols.push_back(make_pair(string(), string()));
				ResultHTMLTable t(
					cols,
					searchRequest.getHTMLForm(),
					_requestParameters,
					areas
				);
				stream << t.open();

				// Loop on items
				BOOST_FOREACH(const shared_ptr<FreeDRTArea>& area, areas)
				{
					// Declarations
					openRequest.getPage()->setArea(const_pointer_cast<const FreeDRTArea>(area));
					removeRequest.getAction()->setObjectId(area->getKey());

					// New row
					stream << t.row();

					// Open button cell
					stream << t.col();
					stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), FreeDRTAreaAdmin::ICON);

					// Name cell
					stream << t.col();
					stream << area->getName();

					// Perimeter cell
					stream << t.col();
					if(!area->getCities().empty())
					{
						stream << "Localités : ";
						bool first(true);
						BOOST_FOREACH(const FreeDRTArea::Cities::value_type& city, area->getCities())
						{
							if(first)
							{
								first = false;
							}
							else
							{
								stream << ", ";
							}
							stream << city->getName();
						}
					}
					if(!area->getStopAreas().empty())
					{
						if(!area->getCities().empty())
						{
							stream << "<br />";
						}
						stream << "Arrêts : ";
						bool first(true);
						BOOST_FOREACH(const FreeDRTArea::StopAreas::value_type& stopArea, area->getStopAreas())
						{
							if(first)
							{
								first = false;
							}
							else
							{
								stream << ", ";
							}
							stream << stopArea->getFullName();
						}
					}

					// Remove cell
					stream << t.col();
					stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la zone "+ area->getName() + " ?");
				}

				// New row
				stream << t.row();

				// Empty cell
				stream << t.col();

				// Name field
				stream << t.col() << addForm.getTextInput(FreeDRTAreaUpdateAction::PARAMETER_NAME, string(), "(nom)");

				// Empty field
				stream << t.col();

				// Add button
				stream << t.col() << addForm.getSubmitButton("Ajouter");

				// Table closing
				stream << t.close() << addForm.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB HOURS
			if (openTabContent(stream, TAB_DATES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<CommercialLineCalendarTemplateUpdateAction,CommercialLineAdmin> updateCalendarRequest(_request, *this);
				updateCalendarRequest.getAction()->setLine(const_pointer_cast<CommercialLine>(_cline));
				PropertiesHTMLTable t(updateCalendarRequest.getHTMLForm());

				stream << t.open();
				stream << t.cell(
					"Calendrier à respecter",
					t.getForm().getSelectInput(
						CommercialLineCalendarTemplateUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList("Pas de vérification"),
						optional<RegistryKeyType>(_cline->getCalendarTemplate() ? _cline->getCalendarTemplate()->getKey() : 0)
				)	);
				stream << t.close();

				stream << "<h1>Contrôles du calendrier</h1>";

				if(_controlCalendar)
				{
					RunHours _runHours(
						getCommercialLineRunHours(_getEnv(), _cline->getKey(), _startDate, _endDate)
					);

					stream << "<style>td.red {background-color:red;width:8px; height:8px; color:white; text-align:center; } td.green {background-color:#008000;width:10px; height:10px; color:white; text-align:center; }</style>";
					HTMLTable::ColsVector cols;
					cols.push_back("Date");
					for(int i(0); i<=23; ++i)
					{
						cols.push_back(lexical_cast<string>(i));
					}
					optional<date> d;
					int lastHour;
					HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const RunHours::value_type& it, _runHours)
					{
						if(!d || d != it.first.first)
						{
							if (d)
							{
								for(int i(lastHour+1); i<=23; ++i)
								{
									stream << t.col(1, "red") << "0";
								}
								for(d = *d + days(1); *d < it.first.first; d = *d + days(1))
								{
									stream << t.row();
									stream << t.col(1, string(), true) << to_iso_extended_string(*d);
									for(int i(0); i<=23; ++i)
									{
										stream << t.col(1, "red") << "0";
									}
								}
							}
							d = it.first.first;
							stream << t.row();
							stream << t.col(1, string(), true) << to_iso_extended_string(*d);
							lastHour = -1;
						}
						for(int i(lastHour+1); i<it.first.second; ++i)
						{
							stream << t.col(1, "red") << "0";
						}
						stream << t.col(1, "green") << it.second;
						lastHour = it.first.second;
					}
					if(d)
					{
						for(int i(lastHour+1); i<=23; ++i)
						{
							stream << t.col(1, "red") << "0";
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<CommercialLineAdmin> openRequest(_request, *this);
					openRequest.getPage()->setControlCalendar(true);

					stream <<
						"<p class=\"info\">Les contrôle de dates sont désactivées par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(openRequest.getURL(), "Activer les contrôles de date", string(), ICON) <<
						"</p>"
					;
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB NON CONCURRENCY
			if (openTabContent(stream, TAB_NON_CONCURRENCY))
			{
				AdminActionFunctionRequest<NonConcurrencyRuleAddAction,CommercialLineAdmin> addRequest(_request, *this);
				addRequest.getAction()->setHiddenLine(_cline);

				AdminActionFunctionRequest<RemoveObjectAction, CommercialLineAdmin> removeRequest(_request, *this);
				AdminFunctionRequest<CommercialLineAdmin> searchRequest(_request, *this);

				stream << "<h1>Lignes prioritaires</h1>";

				NonConcurrencyRuleTableSync::SearchResult rules(NonConcurrencyRuleTableSync::Search(_getEnv(), _cline->getKey()));
				ActionResultHTMLTable::HeaderVector cols;
				cols.push_back(make_pair(string(),"Réseau"));
				cols.push_back(make_pair(string(),"Ligne"));
				cols.push_back(make_pair(string(),"Délai"));
				cols.push_back(make_pair(string(),"Action"));

				ActionResultHTMLTable t(
					cols,
					searchRequest.getHTMLForm(),
					_requestParameters,
					rules,
					addRequest.getHTMLForm("create")
				);

				stream << t.open();

				BOOST_FOREACH(const shared_ptr<NonConcurrencyRule>& rule, rules)
				{
					removeRequest.getAction()->setObjectId(rule->getKey());

					stream << t.row();
					stream << t.col() << rule->getPriorityLine()->getNetwork()->getName();
					stream << t.col(1, rule->getPriorityLine()->getStyle()) << rule->getPriorityLine()->getShortName();
					stream << t.col() << rule->getDelay().minutes();
					stream << t.col() <<
						HTMLModule::getLinkButton(
							removeRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer la règle de non concurrence avec la ligne " + rule->getPriorityLine()->getShortName() + " ?",
							"lock_delete.png"
						)
					;
				}

				stream << t.row();
				stream << t.col(2);
				stream << t.getActionForm().getSelectInput(
					NonConcurrencyRuleAddAction::PARAMETER_PRIORITY_LINE_ID,
					PTModule::getCommercialLineLabels(
						_request.getUser()->getProfile()->getRightsForModuleClass<TransportNetworkRight>(),
						_request.getUser()->getProfile()->getGlobalPublicRight<TransportNetworkRight>() >= READ,
						READ
					),
					optional<RegistryKeyType>()
				);
				stream << t.col() <<
					t.getActionForm().getSelectNumberInput(
						NonConcurrencyRuleAddAction::PARAMETER_DURATION,
						0, 120
					)
				;

				stream << t.col() <<
					t.getActionForm().getSubmitButton("Ajouter");

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				// The update request
				AdminActionFunctionRequest<CommercialLineUpdateAction,CommercialLineAdmin> updateRequest(_request, *this);
				updateRequest.getAction()->setLine(const_pointer_cast<CommercialLine>(_cline));

				stream << "<h1>Propriétés</h1>";

				// Main properties update form
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Réseau", _cline->getNetwork()->getName());
				stream << t.cell(
					"Parent",
					t.getForm().getSelectInput(
						CommercialLineUpdateAction::PARAMETER_NETWORK_ID,
						_cline->getNetwork()->getSubFoldersLabels(),
						boost::optional<util::RegistryKeyType>(_cline->_getParent()->getKey())
				)	);
				stream << t.title("Nom");
				stream << t.cell("Nom (menu)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_NAME, _cline->getName()));
				stream << t.cell("Nom long (feuille de route)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_LONG_NAME, _cline->getLongName()));
				stream << t.cell("Nom court (cartouche)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_SHORT_NAME, _cline->getShortName()));
				stream << t.title("Apparence");
				stream << t.cell("Image", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_IMAGE, _cline->getImage()));
				stream << t.cell("Style CSS", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_STYLE, _cline->getStyle()));
				stream << t.cell("Couleur (format XML #rrggbb)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_COLOR, _cline->getColor() ? _cline->getColor()->toXMLColor() : string()));
				stream << t.cell(
					"Affichage avant premier départ (minutes)",
					t.getForm().getTextInput(
						CommercialLineUpdateAction::PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE,
						_cline->getDisplayDurationBeforeFirstDeparture().is_not_a_date_time() ? string() : lexical_cast<string>(_cline->getDisplayDurationBeforeFirstDeparture().total_seconds() / 60)
				)	);
				stream << t.title("Réservation");
				stream << t.cell("Centre de contact", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_RESERVATION_CONTACT_ID, _cline->getReservationContact() ? lexical_cast<string>(_cline->getReservationContact()->getKey()) : string()));
				stream << t.title("URL Documents");
				stream << t.cell("Carte", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_MAP_URL, _cline->getMapURL()));
				stream << t.cell("Document", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_DOC_URL, _cline->getDocURL()));
				stream << t.cell("ID Grille horaire", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_TIMETABLE_ID, lexical_cast<string>(_cline->getTimetableId())));
				stream << t.close();

				StaticActionRequest<CommercialLineUpdateAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setLine(const_pointer_cast<CommercialLine>(_cline));

				ImportableAdmin::DisplayDataSourcesTab(stream, *_cline, updateOnlyRequest);

				PTRuleUserAdmin<CommercialLine,CommercialLineAdmin>::Display(stream, _cline, _request);

				stream << "<h1>Changement de réseau</h1>";

				PropertiesHTMLTable t1(updateRequest.getHTMLForm());
				stream << t1.open();
				stream << t1.title("Réseau");
				stream << t1.cell(
					"Réseau",
					t1.getForm().getTextInputAutoCompleteFromTableSync(
						CommercialLineUpdateAction::PARAMETER_NETWORK_ID,
						_cline->getNetwork() ? lexical_cast<string>(_cline->getNetwork()->getKey()) : string(),
						_cline->getNetwork() ? lexical_cast<string>(_cline->getNetwork()->getName()) : string(),
						lexical_cast<string>(TransportNetworkTableSync::TABLE.ID),
						string(),string(),
						false, true, true, true
				)	);
				stream << t1.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB EXPORT
			if (openTabContent(stream, TAB_EXPORT))
			{
/*				shared_ptr<TridentFileFormat::Exporter_> exporter(new TridentFileFormat::Exporter_);
				exporter->setLine(_cline);

				StaticFunctionRequest<ExportFunction> tridentExportFunction(_request, true);
				tridentExportFunction.getFunction()->setExporter(static_pointer_cast<Exporter, TridentFileFormat::Exporter_>(exporter));

				stream << "<h1>Formats Trident</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident standard", string(), "page_white_go.png");
				stream << " ";
				exporter->setWithTisseoExtension(true);
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident Tisséo", string(), "page_white_go.png");
				stream << "</p>";
*/			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}

		bool CommercialLineAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_cline.get() == NULL) return false;
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_cline->getKey()));
		}



		std::string CommercialLineAdmin::getTitle() const
		{
			return _cline.get() ? "<span class=\"linesmall " + _cline->getStyle() +"\">" + _cline->getShortName() + "</span>" : DEFAULT_TITLE;
		}



		AdminInterfaceElement::PageLinks CommercialLineAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				// Journey patterns
				JourneyPatternTableSync::SearchResult routes(
					JourneyPatternTableSync::Search(Env::GetOfficialEnv(), _cline->getKey())
				);
				BOOST_FOREACH(const shared_ptr<JourneyPattern>& line, routes)
				{
					shared_ptr<JourneyPatternAdmin> p(
						getNewPage<JourneyPatternAdmin>()
					);
					p->setLine(line);
					links.push_back(p);
				}

				// Free DRT areas
				FreeDRTAreaTableSync::SearchResult areas(
					FreeDRTAreaTableSync::Search(Env::GetOfficialEnv(), _cline->getKey())
				);
				BOOST_FOREACH(const shared_ptr<FreeDRTArea>& area, areas)
				{
					shared_ptr<FreeDRTAreaAdmin> p(
						getNewPage<FreeDRTAreaAdmin>()
					);
					p->setArea(area);
					links.push_back(p);
				}
			}
			return links;
		}



		void CommercialLineAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Parcours aller", TAB_ROUTES_FORWARD, true, JourneyPatternAdmin::ICON));
			_tabs.push_back(Tab("Parcours retour", TAB_ROUTES_BACKWARD, true, JourneyPatternAdmin::ICON));
			_tabs.push_back(Tab("TAD libéralisés", TAB_FREE_DRT, true, string()));
			_tabs.push_back(Tab("Dates de fonctionnement", TAB_DATES, true, "calendar.png"));
			_tabs.push_back(Tab("Non concurrence", TAB_NON_CONCURRENCY, true, "lock.png"));
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Export", TAB_EXPORT, true, "page_white_go.png"));

			_tabBuilded = true;
		}



		bool CommercialLineAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _cline->getKey() == static_cast<const CommercialLineAdmin&>(other)._cline->getKey();
		}



		AdminInterfaceElement::PageLinks CommercialLineAdmin::_getCurrentTreeBranch() const
		{
			shared_ptr<TransportNetworkAdmin> p(
				getNewPage<TransportNetworkAdmin>()
			);
			p->setNode(*_cline->_getParent());

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(getNewCopiedPage());
			return links;
		}



		void CommercialLineAdmin::_displayRoutes(
			std::ostream& stream,
			const server::Request& request,
			bool wayBack
		) const {
			// Requests
			AdminFunctionRequest<CommercialLineAdmin> searchRequest(request, *this);

			AdminActionFunctionRequest<RemoveObjectAction, CommercialLineAdmin> removeRequest(request, *this);

			// Search form
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"+  lexical_cast<string>(wayBack)));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			stream << s.close();


			// Results display
			stream << "<h1>Parcours de la ligne</h1>";

			JourneyPatternTableSync::SearchResult routes(
				JourneyPatternTableSync::Search(
					Env::GetOfficialEnv(),
					_cline->getKey(),
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder,
					UP_LINKS_LOAD_LEVEL,
					wayBack
			)	);

			AdminActionFunctionRequest<JourneyPatternAddAction,JourneyPatternAdmin> creationRequest(request);
			creationRequest.setActionFailedPage(getNewCopiedPage());
			creationRequest.setActionWillCreateObject();
			creationRequest.getAction()->setCommercialLine(const_pointer_cast<CommercialLine>(_cline));

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), ""));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Origne"));
			h.push_back(make_pair(string(), "Destination"));
			h.push_back(make_pair(string(), "Arrêts"));
			h.push_back(make_pair(string(), "Long."));
			h.push_back(make_pair(string(), HTMLModule::getHTMLImage("/admin/img/car.png", "Services")));
			h.push_back(make_pair(string(), "Source"));
			h.push_back(make_pair(string(), "Actions"));

			ActionResultHTMLTable t(
				h,
				s.getForm(),
				_requestParameters,
				routes,
				creationRequest.getHTMLForm("addline" + lexical_cast<string>(wayBack)),
				JourneyPatternAddAction::PARAMETER_TEMPLATE_ID
			);

			stream << t.open();
			AdminFunctionRequest<JourneyPatternAdmin> lineOpenRequest(request);
			BOOST_FOREACH(const shared_ptr<JourneyPattern>& line, routes)
			{
				// Row initialization
				lineOpenRequest.getPage()->setLine(line);
				removeRequest.getAction()->setObjectId(line->getKey());
				stream << t.row(lexical_cast<string>(line->getKey()));

				// Open button
				stream << t.col();
				stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");

				// Name
				stream << t.col();
				stream << line->getName();

				// Origin and destination
				if(line->getEdges().size() < 2)
				{
					stream << t.col(4) << "Trajet non défini";
				}
				else
				{
					{
						stream << t.col();
						const DesignatedLinePhysicalStop* lineStop(
							dynamic_cast<const DesignatedLinePhysicalStop*>(
								*line->getEdges().begin()
						)	);
						if(lineStop)
						{
							stream << lineStop->getPhysicalStop()->getConnectionPlace()->getFullName();
						}
						const LineArea* lineArea(
							dynamic_cast<const LineArea*>(
								*line->getEdges().begin()
						)	);
						if(lineArea)
						{
							stream << lineArea->getArea()->getName();
						}
					}

					{
						stream << t.col();
						const DesignatedLinePhysicalStop* lineStop(
							dynamic_cast<const DesignatedLinePhysicalStop*>(
								*line->getEdges().rbegin()
						)	);
						if(lineStop)
						{
							stream << lineStop->getPhysicalStop()->getConnectionPlace()->getFullName();
						}
						const LineArea* lineArea(
							dynamic_cast<const LineArea*>(
								*line->getEdges().rbegin()
						)	);
						if(lineArea)
						{
							stream << lineArea->getArea()->getName();
						}
					}

					// Stops number
					stream << t.col();
					stream << line->getEdges().size();

					// Length
					stream << t.col();
					stream << line->getLastEdge()->getMetricOffset();
				}

				// Services number
				stream << t.col();
				size_t servicesNumber(line->getServices().size());
				BOOST_FOREACH(JourneyPatternCopy* subline, line->getSubLines())
				{
					servicesNumber += subline->getServices().size();
				}
				stream << servicesNumber;

				// Datasource
				stream << t.col();
				BOOST_FOREACH(const Importable::DataSourceLinks::value_type& it, line->getDataSourceLinks())
				{
					if(!it.first)
					{ // Unknown data source
						continue;
					}

					string name(it.first->getName());
					if(!it.second.empty())
					{
						name += " (code "+ it.second +")";
					}

					stream <<
						HTMLModule::getHTMLImage(
							"/admin/img/"+
							(	it.first->getIcon().empty() ?
								"note.png" :
								it.first->getIcon()
							),
							name
						);
					if(!it.first->getFormat().empty())
					{
						stream <<
							HTMLModule::getHTMLImage(
								"/admin/img/" + DataSourceAdmin::ICON,
								"Source importée automatiquement, ne pas effectuer d'édition manuelle sur cet itinéraire"
						);
					}
				}

				// Remove button
				stream << t.col();
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le parcours ? Tous les services du parcours seront également supprimés.", "/admin/icon/chart_line_delete.png");
			}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(JourneyPatternAddAction::PARAMETER_NAME, string(), string(), AdminModule::CSS_2DIGIT_INPUT);
			stream << t.col(5) << "Inversion : " << t.getActionForm().getOuiNonRadioInput(JourneyPatternAddAction::PARAMETER_REVERSE_COPY, false);
			stream << t.col(2) << t.getActionForm().getSubmitButton("Créer un itinéraire");

			stream << t.close();

		}
}	}
