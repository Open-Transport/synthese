
/** DisplaySearchAdmin class implementation.
	@file DisplaySearchAdmin.cpp

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

#include "DisplaySearchAdmin.h"

#include "SentScenario.h"
#include "AdvancedSelectTableSync.h"
#include "DisplayScreenTableSync.h"
#include "User.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayAdmin.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "BroadcastPointsAdmin.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "ActionResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminModule.h"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "Alarm.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUAdmin.h"
#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPUTableSync.h"
#include "Profile.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace html;
	using namespace departure_boards;
	using namespace security;
	using namespace messages;
	using namespace geography;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,DisplaySearchAdmin>::FACTORY_KEY("0displays");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::ICON("computer.png");
		template<> const string AdminInterfaceElementTemplate<DisplaySearchAdmin>::DEFAULT_TITLE("Recherche d'équipements");
	}

	namespace departure_boards
	{
		const string DisplaySearchAdmin::TAB_CPU("cp");
		const string DisplaySearchAdmin::TAB_DISPLAY_SCREENS("ds");
		const string DisplaySearchAdmin::PARAMETER_SEARCH_CITY = "dsascity";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STOP = "dsaslid";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_NAME = "dsasloc";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LINE_ID = "dsasli";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_TYPE_ID = "dsasti";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_STATE = "dsass";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_MESSAGE = "dsasm";
		const string DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID("dsapsli");

		DisplaySearchAdmin::DisplaySearchAdmin()
			: AdminInterfaceElementTemplate<DisplaySearchAdmin>()
		{}

		void DisplaySearchAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			setPlace(map.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LOCALIZATION_ID));
			if (!_place)
			{
				_searchCity = map.getDefault<string>(PARAMETER_SEARCH_CITY);
				_searchStop = map.getDefault<string>(PARAMETER_SEARCH_STOP);
				_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);
				_searchLineId = map.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LINE_ID);
				_searchTypeId = map.getOptional<RegistryKeyType>(PARAMETER_SEARCH_TYPE_ID);
				_searchState = map.getOptional<int>(PARAMETER_SEARCH_STATE);
				_searchMessage = map.getOptional<int>(PARAMETER_SEARCH_MESSAGE);
			}

			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_CITY, 30);
		}



		util::ParametersMap DisplaySearchAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_place)
			{
				m.insert(PARAMETER_SEARCH_LOCALIZATION_ID, _place->get() ? (*_place)->getKey() : 0);
			} else {
				m.insert(PARAMETER_SEARCH_CITY, _searchCity);
				m.insert(PARAMETER_SEARCH_STOP, _searchStop);
				m.insert(PARAMETER_SEARCH_NAME, _searchName);
				m.insert(PARAMETER_SEARCH_LINE_ID, _searchLineId.get_value_or(0));
			}
			if(_searchTypeId)
			{
				m.insert(PARAMETER_SEARCH_TYPE_ID, _searchTypeId.get_value_or(0));
			}
			if(_searchState)
			{
				m.insert(PARAMETER_SEARCH_STATE, _searchState.get_value_or(0));
			}
			if(_searchMessage)
			{
				m.insert(PARAMETER_SEARCH_MESSAGE, _searchMessage.get_value_or(0));
			}
			return m;
		}


		void DisplaySearchAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			RightsOfSameClassMap rights(_request.getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>());

			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::Search(
					Env::GetOfficialEnv(),
					rights
					, _request.getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
					, READ
					, optional<RegistryKeyType>()
					, _place ? (_place->get() ? (*_place)->getKey() : 0) : optional<RegistryKeyType>()
					, _searchLineId
					, _searchTypeId
					, _searchCity
					, _searchStop
					, _searchName
					, _searchState
					, _searchMessage
					, _requestParameters.first
					, _requestParameters.maxSize
					, false
					, _requestParameters.orderField == PARAMETER_SEARCH_CITY
					, _requestParameters.orderField == PARAMETER_SEARCH_STOP
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.orderField == PARAMETER_SEARCH_TYPE_ID
					, _requestParameters.orderField == PARAMETER_SEARCH_STATE
					, _requestParameters.orderField == PARAMETER_SEARCH_MESSAGE
					, _requestParameters.raisingOrder
			)	);

			DisplayScreenCPUTableSync::SearchResult cpus(
				DisplayScreenCPUTableSync::Search(
					Env::GetOfficialEnv(),
					_place ? (_place->get() ? (*_place)->getKey() : 0) : optional<RegistryKeyType>(),
					optional<string>(),
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);


			///////////////////////////////////////////////
			/// TAB SCREENS
			if (openTabContent(stream, TAB_DISPLAY_SCREENS))
			{
				AdminActionFunctionRequest<CreateDisplayScreenAction,DisplayAdmin> createDisplayRequest(
					_request
				);
				createDisplayRequest.setActionFailedPage<DisplaySearchAdmin>();
				if(_place)
				{
					createDisplayRequest.getAction()->setPlace(_place->get());
				}
				createDisplayRequest.setActionWillCreateObject();


				AdminFunctionRequest<DisplaySearchAdmin> searchRequest(_request, *this);

				AdminFunctionRequest<DisplayAdmin> updateRequest(_request);

				StaticFunctionRequest<DisplayScreenContentFunction> viewRequest(_request, true);

				if (!_place)
				{
					stream << "<h1>Recherche</h1>";

					stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,	 _searchLineId, _searchTypeId, _searchState, _searchMessage);
				}

				stream << "<h1>" << (_place ? "Afficheurs" : "Résultats de la recherche") << "</h1>";

				ActionResultHTMLTable::HeaderVector v;
				if (!_place)
				{
					v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
					v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arrêt"));
				}
				v.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				v.push_back(make_pair(PARAMETER_SEARCH_TYPE_ID, "Type"));
				v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
				v.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Contenu"));
				v.push_back(make_pair(string(), "Actions"));
				v.push_back(make_pair(string(), "Actions"));

				ActionResultHTMLTable t(
					v
					, searchRequest.getHTMLForm()
					, _requestParameters
					, screens
					, createDisplayRequest.getHTMLForm("create")
					, CreateDisplayScreenAction::PARAMETER_TEMPLATE_ID
				);
				t.getActionForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();

				BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
				{
					if(screen->getParent(true))
					{
						continue;
					}
					updateRequest.getPage()->setScreen(screen);
					viewRequest.getFunction()->setScreen(screen);

					vector<boost::shared_ptr<Alarm> > alarms(
						DisplayScreenTableSync::GetCurrentDisplayedMessage(_getEnv(), screen->getKey(), 1)
					);
					boost::shared_ptr<Alarm> alarm(alarms.empty() ? boost::shared_ptr<Alarm>() : alarms.front());

					stream << t.row(lexical_cast<string>(screen->getKey()));
					if (!_place || !_place->get())
					{
						stream <<
							t.col() <<
							(	screen->getLocation() ?
								screen->getLocation()->getCity()->getName() :
								"(indéterminé)"
							)
						;
						stream <<
							t.col() <<
							(	screen->getLocation() ?
								screen->getLocation()->getName() :
								"(indéterminé)"
							)
						;
					}
					stream << t.col() << screen->get<BroadCastPointComment>();
					stream <<
						t.col() <<
						(	screen->get<DisplayTypePtr>() ?
							screen->get<DisplayTypePtr>()->get<Name>() :
							HTMLModule::getHTMLImage("/admin/img/error.png", "Type non défini")
						)
					;

					// Monitoring status
					bool monitored(screen->isMonitored());
					stream <<
						t.col();

					if(screen->get<DisplayTypePtr>().get_ptr() == NULL)
					{
						stream <<
							HTMLModule::getHTMLImage(
								"/admin/img/error.png",
								"Veuillez définir le type d'afficheur dans l'écran de configuration."
							)
						;
					}
					else if(!screen->get<MaintenanceIsOnline>())
					{
						stream <<
							HTMLModule::getHTMLImage(
								"/admin/img/cross.png",
								"Désactivé par la maintenance : "+ screen->get<MaintenanceMessage>()
							)
						;
					}
					else if(!monitored)
					{
						stream << HTMLModule::getHTMLImage(
								"/admin/img/help.png",
								"Ce type d'afficheur n'est pas supervisé. Voir la définition du type."
							)
						;
					}
					else
					{
						boost::shared_ptr<DisplayMonitoringStatus> status(
							DisplayMonitoringStatusTableSync::GetStatus(screen->getKey())
						);

						if(status.get() == NULL)
						{
							stream <<
								HTMLModule::getHTMLImage("/admin/img/exclamation.png", "Cet afficheur n'est jamais entré en contact.")
							;
						}
						else if(screen->isDown(*status)
						){
							stream <<
								HTMLModule::getHTMLImage("/admin/img/exclamation.png", "Cet afficheur n'est plus en contact alors qu'il est déclaré online.")
							;
						}
						else
						{
							stream <<
								HTMLModule::getHTMLImage(
									"/admin/img/" + DisplayMonitoringStatus::GetStatusIcon(status->getGlobalStatus()),
									DisplayMonitoringStatus::GetStatusString(status->getGlobalStatus())
								)
							;
						}
					}

					// Content
					stream << t.col();
					if (!screen->get<MaintenanceIsOnline>())
					{
						stream <<
							HTMLModule::getHTMLImage(
								"/admin/img/cross.png",
								"Désactivé par la maintenance : "+ screen->get<MaintenanceMessage>()
							)
						;
					}
					else
					{
						if (alarm.get() != NULL)
						{
							stream <<
								HTMLModule::getHTMLImage(
									string("/admin/img/") +
									string((alarm->getLevel() == ALARM_LEVEL_WARNING) ?
											"full_screen_message_display.png" :
											"partial_message_display.png"
									),
									"Message : " + alarm->getShortMessage()
								)
							;
						}
						else
						{
							if (DisplayScreenTableSync::GetIsAtLeastALineDisplayed(screen->getKey()))
							{
								stream <<
									HTMLModule::getHTMLImage("/admin/img/times_display.png", "Affichage d'horaires en cours")
								;
							}
							else
							{
								stream <<
									HTMLModule::getHTMLImage("/admin/img/empty_display.png", "Aucune ligne affichée, écran vide")
								;
							}
						}
					}
					stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Ouvrir", string(), "/admin/img/monitor_edit.png");
					stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler", string(), "/admin/img/monitor_go.png");
				}

				if (tabHasWritePermissions())
				{
					stream << t.row();
					stream << t.col(_place.get() ? 4 : 6) << "(sélectionner un afficheur existant pour copier ses&nbsp;propriétés dans le nouvel élément)";
					stream << t.col(3) << t.getActionForm().getSubmitButton("Créer un nouvel afficheur");
				}

				stream << t.close();
			}

			///////////////////////////////////////////////
			/// TAB CPU
			if (openTabContent(stream, TAB_CPU))
			{
				AdminActionFunctionRequest<DisplayScreenCPUCreateAction,DisplayScreenCPUAdmin> createCPURequest(
					_request
				);
				createCPURequest.setActionFailedPage<DisplaySearchAdmin>();
				if(_place)
				{
					createCPURequest.getAction()->setPlace(*_place);
				}
				createCPURequest.setActionWillCreateObject();


				AdminFunctionRequest<DisplaySearchAdmin> searchRequest(_request, *this);

				AdminFunctionRequest<DisplayScreenCPUAdmin> updateRequest(_request);

				if (!_place)
				{
					stream << "<h1>Recherche</h1>";

					stream << getHtmlSearchForm(searchRequest.getHTMLForm(), _searchCity, _searchStop, _searchName,	 _searchLineId, _searchTypeId, _searchState, _searchMessage);
				}

				stream << "<h1>" << (_place && _place->get() ? "Unités centrales" : "Résultats de la recherche") << "</h1>";

				ActionResultHTMLTable::HeaderVector v;
				if (!_place)
				{
					v.push_back(make_pair(PARAMETER_SEARCH_CITY, "Commune"));
					v.push_back(make_pair(PARAMETER_SEARCH_STOP, "Arrêt"));
				}
				v.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
				v.push_back(make_pair(PARAMETER_SEARCH_STATE, "Etat"));
				v.push_back(make_pair(string(), "Actions"));

				ActionResultHTMLTable t(
					v
					, searchRequest.getHTMLForm()
					, _requestParameters
					, cpus
					, createCPURequest.getHTMLForm("createCPU")
					, DisplayScreenCPUCreateAction::PARAMETER_TEMPLATE_ID
				);
				t.getActionForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();

				BOOST_FOREACH(const boost::shared_ptr<DisplayScreenCPU>& cpu, cpus)
				{
					updateRequest.getPage()->setCPU(cpu);

					stream << t.row(lexical_cast<string>(cpu->getKey()));
					if (!_place)
					{
						stream <<
							t.col() <<
							(	cpu->getPlace() ?
							cpu->getPlace()->getCity()->getName() :
						"(indéterminé)"
							)
							;
						stream <<
							t.col() <<
							(	cpu->getPlace() ?
							cpu->getPlace()->getName() :
						"(indéterminé)"
							)
							;
					}
					stream << t.col() << cpu->getName();

					stream << t.col();

					stream << t.col() << HTMLModule::getLinkButton(updateRequest.getURL(), "Ouvrir", string(), "/admin/img/monitor_edit.png");

				}

				if (tabHasWritePermissions())
				{
					stream << t.row();
					stream << t.col(_place.get() ? 4 : 6) << "(sélectionner une unité centrale existante pour copier ses&nbsp;propriétés dans le nouvel élément)";
					stream << t.col(3) << t.getActionForm().getSubmitButton("Créer une nouvelle unité centrale");
				}

				stream << t.close();

			}
			closeTabContent(stream);
		}

		std::string DisplaySearchAdmin::getHtmlSearchForm(const HTMLForm& form
			, const std::string& cityName
			, const std::string& stopName
			, const std::string& displayName
			, optional<RegistryKeyType> lineUid,
			optional<RegistryKeyType> typeUid,
			optional<int> state,
			optional<int> message
		){
			vector<pair<optional<int>, string> > states;
			states.push_back(make_pair(optional<int>(), "(tous)"));
			states.push_back(make_pair(1, "OK"));
			states.push_back(make_pair(2, "Warning"));
			states.push_back(make_pair(3, "Warning+Error"));
			states.push_back(make_pair(4, "Error"));

			vector<pair<optional<int>, string> > messages;
			messages.push_back(make_pair(optional<int>(), "(tous)"));
			messages.push_back(make_pair(1, "Un message"));
			messages.push_back(make_pair(2, "Conflit"));
			messages.push_back(make_pair(3, "Messages"));

			stringstream stream;
			SearchFormHTMLTable s(form);
			stream << s.open();
			stream << s.cell("Commune", s.getForm().getTextInput(PARAMETER_SEARCH_CITY, cityName));
			stream << s.cell("Arrêt", s.getForm().getTextInput(PARAMETER_SEARCH_STOP, stopName));
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, displayName));
			stream << s.cell("Ligne", s.getForm().getSelectInput(PARAMETER_SEARCH_LINE_ID, DeparturesTableModule::getCommercialLineWithBroadcastLabels(true), lineUid));
			stream << s.cell(
				"Type",
				s.getForm().getSelectInput(
					PARAMETER_SEARCH_TYPE_ID,
					DeparturesTableModule::getDisplayTypeLabels(true, true),
					typeUid
			)	);
			stream << s.cell(
				"Etat",
				s.getForm().getSelectInput(PARAMETER_SEARCH_TYPE_ID, states, state)
			);
			stream << s.cell("Message", s.getForm().getSelectInput(PARAMETER_SEARCH_MESSAGE, messages, message));
			stream << s.close();

			return stream.str();
		}

		bool DisplaySearchAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}

		AdminInterfaceElement::PageLinks DisplaySearchAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			// General search page
			if(	dynamic_cast<const DeparturesTableModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				// General search
				boost::shared_ptr<DisplaySearchAdmin> p1(getNewPage<DisplaySearchAdmin>());
				p1->_place = optional<boost::shared_ptr<const StopArea> >();
				links.push_back(p1);

				// Stock
				boost::shared_ptr<DisplaySearchAdmin> p2(getNewPage<DisplaySearchAdmin>());
				p2->_place = boost::shared_ptr<const StopArea>();
				links.push_back(p2);
			}

			return links;
		}



		AdminInterfaceElement::PageLinks DisplaySearchAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {

			AdminInterfaceElement::PageLinks links;

			const DisplaySearchAdmin* sa(dynamic_cast<const DisplaySearchAdmin*>(&currentPage));
			const DisplayAdmin* da(dynamic_cast<const DisplayAdmin*>(&currentPage));
			const DisplayScreenCPUAdmin* ca(dynamic_cast<const DisplayScreenCPUAdmin*>(&currentPage));

			if( _place &&
				(	(sa && sa->_place == _place) ||
					((da && da->getScreen()->getRoot<PlaceWithDisplayBoards>() &&
					 da->getScreen()->getRoot<PlaceWithDisplayBoards>()->getPlace() == _place->get())) ||
					(ca && ca->getCPU()->getPlace() == _place->get())
			)	){
				DisplayScreenCPUTableSync::SearchResult cpus(
					DisplayScreenCPUTableSync::Search(
						Env::GetOfficialEnv(),
						_place->get() ? (*_place)->getKey() : 0,
						optional<string>()
				)	);
				BOOST_FOREACH(const boost::shared_ptr<DisplayScreenCPU>& cpu, cpus)
				{
					boost::shared_ptr<DisplayScreenCPUAdmin> p(getNewPage<DisplayScreenCPUAdmin>());
					p->setCPU(cpu);
					links.push_back(p);
				}
				RightsOfSameClassMap rights(request.getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>());
				DisplayScreenTableSync::SearchResult screens(
					DisplayScreenTableSync::Search(
						Env::GetOfficialEnv(),
						rights,
						request.getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ,
						READ,
						optional<RegistryKeyType>(),
						_place->get() ? (*_place)->getKey() : 0
				)	);
				BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
				{
					if(	screen->getRoot<DisplayScreenCPU>() ||
						screen->getParent() != NULL
					){
						continue;
					}
					boost::shared_ptr<DisplayAdmin> p(getNewPage<DisplayAdmin>());
					p->setScreen(screen);
					links.push_back(p);
				}
			}

			return links;
		}


		std::string DisplaySearchAdmin::getTitle() const
		{
			return
				_place ?
				(	_place->get() ?
					(*_place)->getFullName() :
					"Equipements en stock"
				):
				DEFAULT_TITLE
			;
		}



		void DisplaySearchAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			bool writeRight(
				_place ?
				profile.isAuthorized<ArrivalDepartureTableRight>(
					WRITE,
					UNKNOWN_RIGHT_LEVEL,
					(_place->get() ? lexical_cast<string>((*_place)->getKey()) : string("0"))) :
				false
			);
			_tabs.clear();
			_tabs.push_back(Tab("Afficheurs", TAB_DISPLAY_SCREENS, writeRight, "monitor.png"));
			_tabs.push_back(Tab("Unités centrales", TAB_CPU, writeRight, "server.png"));
			_tabBuilded = true;
		}



		void DisplaySearchAdmin::setPlace(
			optional<util::RegistryKeyType> id
		){
			if(!id)
			{
				return;
			}
			if(*id == 0)
			{
				_place = boost::shared_ptr<const StopArea>();
			}
			else
			{
				try
				{
					_place = Env::GetOfficialEnv().get<StopArea>(*id);
				}
				catch (ObjectNotFoundException<StopArea>&)
				{
					throw AdminParametersException("Specified place not found");
				}
			}
		}



		bool DisplaySearchAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _place == static_cast<const DisplaySearchAdmin&>(other)._place;
		}
}	}
