////////////////////////////////////////////////////////////////////////////////
/// CommercialLineAdmin class implementation.
///	@file CommercialLineAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
#include "NonConcurrencyRuleTableSync.h"
#include "NonConcurrencyRule.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "Line.h"
#include "LineAdmin.h"
#include "LineTableSync.h"
#include "TransportNetworkRight.h"
#include "TridentExportFunction.h"
#include "AdminInterfaceElement.h"
#include "AdminFunctionRequest.hpp"
#include "NonConcurrencyRuleAddAction.h"
#include "NonConcurrencyRuleRemoveAction.h"
#include "AdminParametersException.h"
#include "SearchFormHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "NonPermanentService.h"
#include "Profile.h"
#include "LineAddAction.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLineCalendarTemplateUpdateAction.hpp"
#include "CommercialLineUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "ReservationContact.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PTRuleUserAdmin.hpp"
#include "ActionResultHTMLTable.h"
#include "AdminModule.h"
#include "DataSource.h"
#include "PTImportAdmin.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace html;
	using namespace pt;
	using namespace calendar;
	

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
		const string CommercialLineAdmin::TAB_ROUTES("ro");
		const string CommercialLineAdmin::TAB_NON_CONCURRENCY("nc");
		const string CommercialLineAdmin::TAB_PROPERTIES("pr");
		const string CommercialLineAdmin::TAB_EXPORT("ex");
		const string CommercialLineAdmin::PARAMETER_SEARCH_NAME("na");
		const string CommercialLineAdmin::PARAMETER_DATES_START("ds");
		const string CommercialLineAdmin::PARAMETER_DATES_END("de");
		const string CommercialLineAdmin::PARAMETER_CALENDAR_CONTROL("cc");

		
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
		
		
		
		server::ParametersMap CommercialLineAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_cline.get()) m.insert(Request::PARAMETER_OBJECT_ID, _cline->getKey());
			m.insert(PARAMETER_CALENDAR_CONTROL, _controlCalendar);
			return m;
		}



		void CommercialLineAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const {
			////////////////////////////////////////////////////////////////////
			// TAB ROUTES
			if (openTabContent(stream, TAB_ROUTES))
			{
				// Requests
				AdminFunctionRequest<CommercialLineAdmin> searchRequest(_request);
				
				// Search form
				stream << "<h1>Recherche</h1>";
				
				SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
				stream << s.open();
				stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
				stream << s.close();


				// Results display
				stream << "<h1>Parcours de la ligne</h1>";

				LineTableSync::SearchResult routes(
					LineTableSync::Search(
						Env::GetOfficialEnv(),
						_cline->getKey(),
						optional<RegistryKeyType>(),
						_requestParameters.first,
						_requestParameters.maxSize,
						_requestParameters.orderField == PARAMETER_SEARCH_NAME,
						_requestParameters.raisingOrder
				)	);
				
				AdminActionFunctionRequest<LineAddAction,LineAdmin> creationRequest(_request);
				creationRequest.getFunction()->setActionFailedPage(getNewPage());
				creationRequest.setActionWillCreateObject();
				creationRequest.getAction()->setCommercialLine(const_pointer_cast<CommercialLine>(_cline));

				ResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				h.push_back(make_pair(string(), "Origne"));
				h.push_back(make_pair(string(), "Destination"));
				h.push_back(make_pair(string(), "Arrêts"));
				h.push_back(make_pair(string(), "Long."));
				h.push_back(make_pair(string(), HTMLModule::getHTMLImage("car.png", "Services")));
				h.push_back(make_pair(string(), "Source"));
				h.push_back(make_pair(string(), "Actions"));
				
				ActionResultHTMLTable t(
					h,
					s.getForm(),
					_requestParameters,
					routes,
					creationRequest.getHTMLForm("addline"),
					LineAddAction::PARAMETER_TEMPLATE_ID
				);

				stream << t.open();
				AdminFunctionRequest<LineAdmin> lineOpenRequest(_request);
 				BOOST_FOREACH(shared_ptr<Line> line, routes)
				{
					lineOpenRequest.getPage()->setLine(line);
					stream << t.row(lexical_cast<string>(line->getKey()));
					stream << t.col();
					stream << line->getName();

					if(line->getEdges().size() < 2)
					{
						stream << t.col(4) << "Trajet non défini";
					}
					else
					{
						stream << t.col();
						stream << line->getLineStop(0)->getPhysicalStop()->getConnectionPlace()->getFullName();
						stream << t.col();
						stream << line->getLineStop(line->getEdges().size()-1)->getPhysicalStop()->getConnectionPlace()->getFullName();
						stream << t.col();
						stream << line->getEdges().size();
						stream << t.col();
						stream << line->getLineStop(line->getEdges().size()-1)->getMetricOffset();
					}

					stream << t.col();
					stream << line->getServices().size();

					stream << t.col();
					if(line->getDataSource())
					{
						stream <<
							HTMLModule::getHTMLImage(
								line->getDataSource()->getIcon().empty() ?
								"note.png" :
								line->getDataSource()->getIcon(),
								line->getDataSource()->getName()
							);
						if(!line->getDataSource()->getFormat().empty())
						{
							stream << HTMLModule::getHTMLImage(PTImportAdmin::ICON, "Source importée automatiquement, ne pas effectuer d'édition manuelle sur cet itinéraire");
						}
					}

					stream << t.col();
					stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");
				}

				stream << t.row(string());
				stream << t.col() << t.getActionForm().getTextInput(LineAddAction::PARAMETER_NAME, string(), string(), AdminModule::CSS_2DIGIT_INPUT);
				stream << t.col(5) << "Inversion : " << t.getActionForm().getOuiNonRadioInput(LineAddAction::PARAMETER_REVERSE_COPY, false);
				stream << t.col() << t.getActionForm().getSubmitButton("Créer un itinéraire");

				stream << t.close();
			}
			////////////////////////////////////////////////////////////////////
			// TAB HOURS
			if (openTabContent(stream, TAB_DATES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<CommercialLineCalendarTemplateUpdateAction,CommercialLineAdmin> updateCalendarRequest(_request);
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
						cols.push_back(Conversion::ToString(i));
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
					AdminFunctionRequest<CommercialLineAdmin> openRequest(_request);
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
				AdminActionFunctionRequest<NonConcurrencyRuleAddAction,CommercialLineAdmin> addRequest(_request);
				addRequest.getAction()->setHiddenLine(_cline);
				AdminActionFunctionRequest<NonConcurrencyRuleRemoveAction, CommercialLineAdmin> removeRequest(_request);
				AdminFunctionRequest<CommercialLineAdmin> searchRequest(_request);

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

				BOOST_FOREACH(shared_ptr<NonConcurrencyRule> rule, rules)
				{
					removeRequest.getAction()->setRule(const_pointer_cast<const NonConcurrencyRule>(rule));

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
				);

				stream << t.col() <<
					t.getActionForm().getSubmitButton("Ajouter");

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<CommercialLineUpdateAction,CommercialLineAdmin> updateRequest(_request);
				updateRequest.getAction()->setLine(const_pointer_cast<CommercialLine>(_cline));

				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.title("Réseau");
				stream << t.cell("Réseau", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_NETWORK_ID, _cline->getNetwork() ? lexical_cast<string>(_cline->getNetwork()->getKey()) : string()));
				stream << t.cell("ID vu par le réseau", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_CREATOR_ID, _cline->getCreatorId()));
				stream << t.title("Nom");
				stream << t.cell("Nom (menu)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_NAME, _cline->getName()));
				stream << t.cell("Nom long (feuille de route)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_LONG_NAME, _cline->getLongName()));
				stream << t.cell("Nom court (cartouche)", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_SHORT_NAME, _cline->getShortName()));
				stream << t.title("Apparence");
				stream << t.cell("Image", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_IMAGE, _cline->getImage()));
				stream << t.cell("Style CSS", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_STYLE, _cline->getStyle()));
				stream << t.cell("Couleur", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_COLOR, _cline->getColor() ? _cline->getColor()->toString() : string()));
				stream << t.title("Réservation");
				stream << t.cell("Centre de contact", t.getForm().getTextInput(CommercialLineUpdateAction::PARAMETER_RESERVATION_CONTACT_ID, _cline->getReservationContact() ? lexical_cast<string>(_cline->getReservationContact()->getKey()) : string()));
				stream << t.close();

				PTRuleUserAdmin<CommercialLine,CommercialLineAdmin>::Display(stream, _cline, _request);
			}

			////////////////////////////////////////////////////////////////////
			// TAB EXPORT
			if (openTabContent(stream, TAB_EXPORT))
			{
				StaticFunctionRequest<TridentExportFunction> tridentExportFunction(_request, true);
				tridentExportFunction.getFunction()->setCommercialLine(_cline);
				stream << "<h1>Formats Trident</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident standard", string(), "page_white_go.png");
				stream << " ";
				tridentExportFunction.getFunction()->setWithTisseoExtension(true);
				stream << HTMLModule::getLinkButton(tridentExportFunction.getURL(), "Export Trident Tisséo", string(), "page_white_go.png");
				stream << "</p>";
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}

		bool CommercialLineAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_cline.get() == NULL) return false;
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}
		
		
		std::string CommercialLineAdmin::getTitle() const
		{
			return _cline.get() ? "<span class=\"linesmall " + _cline->getStyle() +"\">" + _cline->getShortName() + "</span>" : DEFAULT_TITLE;
		}

		AdminInterfaceElement::PageLinks CommercialLineAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				LineTableSync::SearchResult routes(
					LineTableSync::Search(*_env, _cline->getKey())
				);
				BOOST_FOREACH(shared_ptr<Line> line, routes)
				{
					shared_ptr<LineAdmin> p(
						getNewOtherPage<LineAdmin>()
					);
					p->setLine(line);
					links.push_back(p);
				}
			}
			return links;
		}

		void CommercialLineAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Parcours", TAB_ROUTES, true, LineAdmin::ICON));
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
			PageLinks links;

			shared_ptr<TransportNetworkAdmin> p(
				getNewOtherPage<TransportNetworkAdmin>()
			);
			p->setNetwork(Env::GetOfficialEnv().getSPtr(_cline->getNetwork()));

			links.push_back(p);
			return links;
		}
	}
}
