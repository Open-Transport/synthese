////////////////////////////////////////////////////////////////////////////////
/// DisplayAdmin class implementation.
///	@file DisplayAdmin.cpp
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

#include "HTMLForm.h"
#include "HTMLTable.h"
#include "HTMLList.h"
#include "PropertiesHTMLTable.h"
#include "ConnectionPlaceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "ActionFunctionRequest.h"
#include "QueryString.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"
#include "SentAlarm.h"
#include "DisplayAdmin.h"
#include "DeparturesTableModule.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "UpdateDisplayScreenAction.h"
#include "DisplayScreenTableSync.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "AddPreselectionPlaceToDisplayScreen.h"
#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "UpdateAllStopsDisplayScreenAction.h"
#include "AddDepartureStopToDisplayScreenAction.h"
#include "AddForbiddenPlaceToDisplayScreen.h"
#include "DisplayScreenAddDisplayedPlace.h"
#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "DisplaySearchAdmin.h"
#include "DisplayScreenRemove.h"
#include "DisplayScreenContentRequest.h"
#include "ArrivalDepartureTableRight.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMaintenanceRight.h"
#include "05_html/Constants.h"
#include "HTMLList.h"
#include "DBLogViewer.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "DisplayTypeAdmin.h"
#include "PhysicalStopTableSync.h"
#include "LineStopTableSync.h"
#include "Line.h"
#include "CommercialLine.h"
#include "ArrivalDepartureTableLog.h"
#include "MessageAdmin.h"
#include "SentScenario.h"
#include "MessagesScenarioAdmin.h"

#include <utility>
#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace db;
	using namespace departurestable;
	using namespace security;
	using namespace time;
	using namespace dblog;
	using namespace messages;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayAdmin>::FACTORY_KEY("display");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplayAdmin>::DEFAULT_TITLE("Afficheur inconnu");
	}

	namespace departurestable
	{
		const string DisplayAdmin::TAB_TECHNICAL("tech");
		const string DisplayAdmin::TAB_MAINTENANCE("maint");
		const string DisplayAdmin::TAB_CONTENT("content");
		const string DisplayAdmin::TAB_APPEARANCE("appear");
		const string DisplayAdmin::TAB_RESULT("result");
		const string DisplayAdmin::TAB_LOG("log");



		void DisplayAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){

		
			uid id(_request->getObjectId());
			if(	id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION) return;

			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id, _env);
				
				_maintenanceLogView.set(map, DisplayMaintenanceLog::FACTORY_KEY, _displayScreen->getKey());
				_generalLogView.set(map, ArrivalDepartureTableLog::FACTORY_KEY, _displayScreen->getKey());

				if(!doDisplayPreparationActions) return;

				if(_displayScreen->getLocalization() != NULL)
				{
					PhysicalStopTableSync::Search(
						_env,
						_displayScreen->getLocalization()->getKey(),
						string("%"),
						0, 0, UP_LINKS_LOAD_LEVEL
					);
					
					BOOST_FOREACH(
						const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it,
						_displayScreen->getLocalization()->getPhysicalStops()
					){
						LineStopTableSync::Search(
							_env,
							UNKNOWN_VALUE,
							it.first,
							0, 0,
							true, true,
							UP_LINKS_LOAD_LEVEL
						);
					}
				}
							}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw AdminParametersException("Display screen not found");
			}
			catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
			{
				throw AdminParametersException("Place not found");
			}

			DisplayMonitoringStatusTableSync::Search(_env, _displayScreen->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);

			// CPU search
			if (_displayScreen->getLocalization() != NULL)
			{
				DisplayScreenCPUTableSync::Search(_env, _displayScreen->getLocalization()->getKey());
			}
		}
		
		
		
		ParametersMap DisplayAdmin::getParametersMap() const
		{
			ParametersMap m(_maintenanceLogView.getParametersMap());
			m.merge(_generalLogView.getParametersMap());
			return m;
		}



		void DisplayAdmin::display(
			std::ostream& stream,
			interfaces::VariablesMap& variables
		) const	{
			// Display screen read in the main environment
			shared_ptr<const DisplayScreen> _prodScreen(
				Env::GetOfficialEnv().getRegistry<DisplayScreen>().get(_displayScreen->getKey())
			);
			
			////////////////////////////////////////////////////////////////////
			// TECHNICAL TAB
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				// Update request
				ActionFunctionRequest<UpdateDisplayScreenAction,AdminRequest> updateDisplayRequest(_request);
				updateDisplayRequest.getFunction()->setSamePage(this);
				updateDisplayRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Delete the screen request
				ActionFunctionRequest<DisplayScreenRemove, AdminRequest> deleteRequest(_request);
				deleteRequest.getFunction()->setPage<DisplaySearchAdmin>();
				deleteRequest.getAction()->setDisplayScreen(_displayScreen);
				
				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateDisplayRequest.getHTMLForm("updateprops"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.title("Emplacement");
				stream << t.cell("Zone d'arrêt", _displayScreen->getLocalization()->getFullName());
				stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_NAME, _displayScreen->getLocalizationComment()));

				stream << t.title("Données techniques");
				stream << t.cell("UID", Conversion::ToString(_displayScreen->getKey()));
				stream <<
					t.cell(
						"Type d'afficheur",
						t.getForm().getSelectInput(
							UpdateDisplayScreenAction::PARAMETER_TYPE,
							DeparturesTableModule::getDisplayTypeLabels(false, _displayScreen->getType() == NULL),
							_displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE
					)	)
				;
				
				stream << t.title("Connexion");
				stream << t.cell("Code de branchement bus RS485", t.getForm().getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()));
				
				if (_displayScreen->getLocalization() != NULL && !_env.getRegistry<DisplayScreenCPU>().empty())
				{
					stream << t.cell("Unité centrale", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_CPU, _env.getRegistry<DisplayScreenCPU>(), (_displayScreen->getCPU() != NULL) ? RegistryKeyType(0) : _displayScreen->getCPU()->getKey()));
				}
				stream <<
					t.cell(
						"Port COM",
						t.getForm().getSelectNumberInput(
							UpdateDisplayScreenAction::PARAMETER_COM_PORT,
							0, 99,
							_displayScreen->getComPort(),
							1,
							"(inutilisé)"
					)	)
				;
				
				stream << t.close();

				stream << "<p class=\"info\">Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalités proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans effet sur l'affichage.</p>";

				if (deleteRequest.isActionFunctionAuthorized())
				{
					stream << "<h1>Suppression</h1>";
					stream << "<p>";
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l\\'afficheur " + _displayScreen->getFullName() + " ?", "monitor_delete.png") << " ";
					stream << "</p>";
				}
			}

			////////////////////////////////////////////////////////////////////
			// MAINTENANCE TAB
			if (openTabContent(stream, TAB_MAINTENANCE))
			{
				// Update action
				ActionFunctionRequest<UpdateDisplayMaintenanceAction,AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setSamePage(this);
				updateRequest.getAction()->setScreenId(_displayScreen->getKey());
				
				// Go to maintenance log
				FunctionRequest<AdminRequest> goToLogRequest(_request);
				goToLogRequest.getFunction()->setPage<DBLogViewer>();
				static_pointer_cast<DBLogViewer,AdminInterfaceElement>(
					goToLogRequest.getFunction()->getPage()
				)->setLogKey(DisplayMaintenanceLog::FACTORY_KEY);
				goToLogRequest.setObjectId(_request->getObjectId());

				// View the display type
				FunctionRequest<AdminRequest> displayTypeRequest(_request);
				displayTypeRequest.getFunction()->setPage<DisplayTypeAdmin>();
				displayTypeRequest.setObjectId(
					_displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE
				);
				
				// Log search
				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				stream << "<h1>Paramètres de maintenance</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream <<
					t.cell(
						"Afficheur déclaré en service",
						t.getForm().getOuiNonRadioInput(
							UpdateDisplayMaintenanceAction::PARAMETER_ONLINE,
							_displayScreen->getIsOnline()
					)	)
				;
				stream <<
					t.cell(
						"Message de maintenance",
						t.getForm().getTextAreaInput(
							UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE,
							_displayScreen->getMaintenanceMessage(),
							3, 30
					)	)
				;
				stream << t.close();

				stream << "<h1>Informations de supervision</h1>";

				bool monitored(
					_displayScreen->getType() != NULL &&
					_displayScreen->getType()->getMonitoringInterface() != NULL &&
					_displayScreen->getType()->getTimeBetweenChecks() > 0
				);
				
				HTMLList l;
				stream << l.open();
				if (_displayScreen->getType() != NULL)
				{
					stream <<
						l.element() <<
						"Type d'afficheur : " <<
						HTMLModule::getHTMLLink(
							displayTypeRequest.getURL(),
							_displayScreen->getType()->getName()
						)
					;
					if(!monitored)
					{
						stream <<
							l.element() <<
							"Ce type d'afficheur n'est pas supervisé."
						;
					} else {
						stream <<
							l.element() <<
							"Durée théorique entre les contacts de supervision : " <<
							_displayScreen->getType()->getTimeBetweenChecks() << " min"
						;
					}
				}
				else
				{
					stream <<
						l.element() <<
						"ATTENTION : veuillez définir le type d'afficheur dans l'écran de configuration."
					;
				}
				
				if(monitored)
				{
					if(_env.getRegistry<DisplayMonitoringStatus>().empty())
					{
						stream << l.element() << "ATTENTION : Cet afficheur n'est jamais entré en contact.";
					}
					else
					{
						shared_ptr<DisplayMonitoringStatus> status(
							_env.getEditableRegistry<DisplayMonitoringStatus>().front()
						);
						if(	_displayScreen->getIsOnline() && _now - status->getTime() >
							_displayScreen->getType()->getTimeBetweenChecks()
						){
							stream <<
								l.element() <<
								"ERREUR : Cet afficheur n'est plus en contact alors qu'il est déclaré online."
							;
						}
	
						stream << l.element() << "Dernière mesure le " << status->getTime().toString();
						stream << l.element() << "Dernier état mesuré : "
							<< DisplayMonitoringStatus::GetStatusString(status->getGlobalStatus());
						stream << l.element() << "Température : "
							<< status->getTemperatureValue();
						stream << l.element() << "Détail anomalies : "
							<< status->getDetail();
					}
				}

				stream << l.close();

				stream << "<h1>Journal de maintenance</h1>";

				_maintenanceLogView.display(
					stream,
					searchRequest
				);
			}

			////////////////////////////////////////////////////////////////////
			// CONTENT TAB
			if (openTabContent(stream, TAB_CONTENT))
			{

				// Update request
				ActionFunctionRequest<UpdateAllStopsDisplayScreenAction,AdminRequest> updateAllDisplayRequest(_request);
				updateAllDisplayRequest.getFunction()->setSamePage(this);

				// Add physical request
				ActionFunctionRequest<AddDepartureStopToDisplayScreenAction,AdminRequest> addPhysicalRequest(_request);
				addPhysicalRequest.getFunction()->setSamePage(this);

				// Add preselection request
				ActionFunctionRequest<AddPreselectionPlaceToDisplayScreen,AdminRequest> addPreselRequest(_request);
				addPreselRequest.getFunction()->setSamePage(this);

				// Add not to serve request
				ActionFunctionRequest<AddForbiddenPlaceToDisplayScreen,AdminRequest> addNSRequest(_request);
				addNSRequest.getFunction()->setSamePage(this);

				// Update preselection request
				ActionFunctionRequest<UpdateDisplayPreselectionParametersAction,AdminRequest> updPreselRequest(_request);
				updPreselRequest.getFunction()->setSamePage(this);
				updPreselRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Remove preselection stop request
				ActionFunctionRequest<RemovePreselectionPlaceFromDisplayScreenAction,AdminRequest> rmPreselRequest(_request);
				rmPreselRequest.getFunction()->setSamePage(this);

				// Remove physical stop request
				ActionFunctionRequest<DisplayScreenRemovePhysicalStopAction,AdminRequest> rmPhysicalRequest(_request);
				rmPhysicalRequest.getFunction()->setSamePage(this);

				// Remove Forbidden place request
				ActionFunctionRequest<DisplayScreenRemoveForbiddenPlaceAction,AdminRequest> rmForbiddenRequest(_request);
				rmForbiddenRequest.getFunction()->setSamePage(this);

				vector<pair<EndFilter, string> > endFilterMap;
				endFilterMap.push_back(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
				endFilterMap.push_back(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

				// Propriétés
				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updPreselRequest.getHTMLForm("updpresel"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.title("Contenu");
				stream << t.cell("Type de contenu", t.getForm().getRadioInput(
					UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_FUNCTION,
					UpdateDisplayPreselectionParametersAction::GetFunctionList(),
					UpdateDisplayPreselectionParametersAction::GetFunction(*_displayScreen)
				));
				stream << t.cell("Terminus", t.getForm().getRadioInput(
					UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_END_FILTER,
					endFilterMap,
					_displayScreen->getEndFilter()
				));
				stream << t.cell("Délai maximum d'affichage", t.getForm().getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) + " minutes");
				stream << t.cell("Délai d'effacement", t.getForm().getSelectInput(
					UpdateDisplayPreselectionParametersAction::PARAMETER_CLEANING_DELAY,
					UpdateDisplayPreselectionParametersAction::GetClearDelaysList(),
					_displayScreen->getClearingDelay()
				));

				if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
				{
					stream << t.title("Présélection");
					stream <<
						t.cell(
							"Délai maximum présélection",
							t.getForm().getTextInput(
								UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY,
								Conversion::ToString(_displayScreen->getForceDestinationDelay())
							) + " minutes"
						)
					;
				}
				
				stream << t.close();

				// Used physical stops
				stream << "<h1>Arrêts de desserte</h1>";

				HTMLForm uaf(updateAllDisplayRequest.getHTMLForm("updaall"));
				uaf.addHiddenField(UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE, Conversion::ToString(!_displayScreen->getAllPhysicalStopsDisplayed()));
				stream << "<p>Mode : "	<< (_displayScreen->getAllPhysicalStopsDisplayed() ? "Tous arrêts (y compris nouveaux)" : "Sélection d'arrêts");
				stream << " " << uaf.getLinkButton("Passer en mode " + string(_displayScreen->getAllPhysicalStopsDisplayed() ? "Sélection d'arrêts" : "Tous arrêts"));
				stream << "</p>";
				HTMLList l;

				if (!_displayScreen->getAllPhysicalStopsDisplayed())
				{
					if(_displayScreen->getLocalization() == NULL)
					{
						stream << "Afficheur non localisé, aucun arrêt à sélectionner.";
					}
					else
					{
						HTMLTable t(4, ResultHTMLTable::CSS_CLASS);
						stream << t.open();
						stream << t.row();
						stream << t.col(1, string(), true) << "Nom";
						stream << t.col(1, string(), true) << "Code exploitant";
						stream << t.col(1, string(), true) << "Lignes";
						stream << t.col(1, string(), true) << "Affiché";
						BOOST_FOREACH(
							const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it,
							_displayScreen->getLocalization()->getPhysicalStops()
						){
							stream << t.row();
							stream << t.col() << it.second->getName();
							stream << t.col() << it.second->getOperatorCode();
							
							// Lines column
							stream << t.col();
							set<const CommercialLine*> lines;
							BOOST_FOREACH(const Edge* edge, it.second->getDepartureEdges())
							{
								lines.insert(
									static_cast<const LineStop*>(edge)->getLine()->getCommercialLine()
								);
							}
							BOOST_FOREACH(const CommercialLine* line, lines)
							{
								stream <<
									"<span class=\"line " << line->getStyle() << "\">" <<
									line->getShortName() <<
									"</span>"
								;
							}
							
							// Activated column
							stream << t.col();							
							if(	_displayScreen->getPhysicalStops().find(it.first) ==
								_displayScreen->getPhysicalStops().end()
							){
								addPhysicalRequest.getAction()->setStopId(it.first);
								stream <<
									HTMLModule::getHTMLLink(
										addPhysicalRequest.getHTMLForm().getURL(),
										HTMLModule::getHTMLImage(
											"cross.png",
											"Arrêt non affiché, cliquer pour afficher"
									)	)
								;
							} else {
								rmPhysicalRequest.getAction()->setStopId(it.first);
								stream <<
									HTMLModule::getHTMLLink(
										rmPhysicalRequest.getHTMLForm().getURL(),
										HTMLModule::getHTMLImage(
											"tick.png",
											"Arrêt affiché, cliquer pour enlever"
									)	)
								;
							}
						}
						stream << t.close();
					}
				}

				// Forbidden places
				stream <<
					"<h1>Arrêts ne devant pas être desservis par les lignes sélectionnées pour l'affichage</h1>"
				;

				HTMLForm ant(addNSRequest.getHTMLForm("addforb"));
				stream << ant.open() << l.open();

				BOOST_FOREACH(
					const DisplayedPlacesList::value_type& it,
					_displayScreen->getForbiddenPlaces()
				){
					HTMLForm ntu(
						rmForbiddenRequest.getHTMLForm("rmfp"+ Conversion::ToString(it.second->getKey()))
					);
					ntu.addHiddenField(
						DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE,
						Conversion::ToString(it.second->getKey())
					);
					stream << l.element("broadcastpoint");
					stream <<
						HTMLModule::getHTMLLink(
							ntu.getURL(),
							HTMLModule::getHTMLImage("delete.png","Supprimer")
						) <<
						it.second->getFullName()
					;
				}

				stream << l.element("broadcastpoint");
				stream << ant.getImageSubmitButton("add.png", "Ajouter");
				stream << ant.getSelectInput(
					AddForbiddenPlaceToDisplayScreen::PARAMETER_PLACE,
					_prodScreen->getSortedAvaliableDestinationsLabels(
						_displayScreen->getForbiddenPlaces()
					),
					uid(0)
				);
				stream << l.close() << ant.close();

				if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
				{
					// Additional preselection stops
					stream << "<h1>Arrêts de présélection</h1>";

					HTMLForm psaf(addPreselRequest.getHTMLForm("addpresel"));
					stream << psaf.open() << l.open();

					BOOST_FOREACH(
						const DisplayedPlacesList::value_type& it,
						_displayScreen->getForcedDestinations()
					){
						HTMLForm psdf(
							rmPreselRequest.getHTMLForm("rmpres" + Conversion::ToString(it.second->getKey()))
						);
						psdf.addHiddenField(
							RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE,
							Conversion::ToString(it.second->getKey())
						);
						stream << l.element("broadcastpoint");
						stream <<
							HTMLModule::getHTMLLink(
								psdf.getURL(),
								HTMLModule::getHTMLImage("delete.png","Supprimer")
							) <<
							it.second->getFullName()
						;
					}

					stream << l.element("broadcastpoint");
					stream << psaf.getImageSubmitButton("add.png", "Ajouter");
					stream <<
						psaf.getSelectInput(
							AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE,
							_prodScreen->getSortedAvaliableDestinationsLabels(
								_displayScreen->getForcedDestinations()
							),
							uid(0)
						)
					;
					stream << l.close() << psaf.close();

					stream << "<p class=\"info\">Les terminus de lignes sont automatiquement présélectionnés.</p>";
				}
			}

			////////////////////////////////////////////////////////////////////
			// APPEARANCE TAB
			if (openTabContent(stream, TAB_APPEARANCE))
			{

				// Add display request
				ActionFunctionRequest<DisplayScreenAddDisplayedPlace,AdminRequest> addDisplayRequest(_request);
				addDisplayRequest.getFunction()->setSamePage(this);

				// Remove displayed place request
				ActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,AdminRequest> rmDisplayedRequest(_request);
				rmDisplayedRequest.getFunction()->setSamePage(this);

				// Properties Update request
				ActionFunctionRequest<DisplayScreenAppearanceUpdateAction,AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setSamePage(this);
				updateRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Maps for particular select fields
				vector<pair<int, string> > blinkingDelaysMap;
				blinkingDelaysMap.push_back(make_pair(0, "Pas de clignotement"));
				blinkingDelaysMap.push_back(make_pair(1, "1 minute avant disparition"));
				for (int i=2; i<6; ++i)
				{
					blinkingDelaysMap.push_back(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));
				}

				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm());

				stream << t.open();
				stream << t.cell("Titre", t.getForm().getTextInput(DisplayScreenAppearanceUpdateAction::PARAMETER_TITLE, _displayScreen->getTitle()));
				stream << t.cell("Clignotement", t.getForm().getSelectInput(DisplayScreenAppearanceUpdateAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay()));
				stream << t.cell("Affichage numéro de quai", t.getForm().getOuiNonRadioInput(DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_PLATFORM, _displayScreen->getTrackNumberDisplay()));
				stream << t.cell("Affichage numéro de service", t.getForm().getOuiNonRadioInput(DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_SERVICE_NUMBER, _displayScreen->getServiceNumberDisplay()));
				stream << t.cell("Affichage numéro d'équipe", t.getForm().getOuiNonRadioInput(DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_TEAM, _displayScreen->getDisplayTeam()));
				stream <<
					t.cell(
						"Affichage horloge",
						t.getForm().getOuiNonRadioInput(
							DisplayScreenAppearanceUpdateAction::PARAMETER_DISPLAY_CLOCK,
							_displayScreen->getDisplayClock()
					)	)
				;

				stream << t.close();

				HTMLList l;

				// Intermediate stops to display
				stream << "<h1>Affichage arrêts intermédiaires</h1>";

				HTMLForm amf(addDisplayRequest.getHTMLForm("adddispl"));

				stream << amf.open();
				stream << l.open();

				for (DisplayedPlacesList::const_iterator it(_displayScreen->getDisplayedPlaces().begin()); it != _displayScreen->getDisplayedPlaces().end(); ++it)
				{
					HTMLForm mf(rmDisplayedRequest.getHTMLForm("rmdp" + Conversion::ToString(it->second->getKey())));
					mf.addHiddenField(DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));

					stream << l.element("broadcastpoint");
					stream << HTMLModule::getHTMLLink(mf.getURL(), HTMLModule::getHTMLImage("delete.png","Supprimer")) << it->second->getFullName();
				}

				stream << l.element("broadcastpoint");
				stream << amf.getImageSubmitButton("add.png", "Ajouter");
				stream << amf.getSelectInput(DisplayScreenAddDisplayedPlace::PARAMETER_PLACE, _prodScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()), uid(0));

				stream << l.close();
				stream << amf.close();

			}

			////////////////////////////////////////////////////////////////////
			// RESULT TAB
			if (openTabContent(stream, TAB_RESULT))
			{
				// Requests
				FunctionRequest<DisplayScreenContentRequest> viewRequest(_request);
				viewRequest.setObjectId(_displayScreen->getKey());

				FunctionRequest<AdminRequest> viewMessageRequest(_request);
				viewMessageRequest.getFunction()->setPage<MessageAdmin>();

				FunctionRequest<AdminRequest> viewScenarioRequest(_request);
				viewScenarioRequest.getFunction()->setPage<MessagesScenarioAdmin>();

				// Output
				stream << "<h1>Contenus actifs</h1>";

				int priority(1);
				HTMLTable::ColsVector h;
				h.push_back("Priorité");
				h.push_back("Contenu");
				h.push_back("Contenu");
				h.push_back("Date fin");
				h.push_back("Admin");
				HTMLTable t(h, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				if (!_displayScreen->getIsOnline())
				{
					stream << t.row();
					stream << t.col() << priority++;
					stream << t.col() << HTMLModule::getHTMLImage("cross.png", "Afficheur désactivé pour maintenance");
					stream << t.col() << "Afficheur désactivé pour maintenance";
					stream << t.col() << "(inconnu)";
					stream << t.col() << getTabLinkButton(TAB_MAINTENANCE);
				}

				vector<shared_ptr<SentAlarm> > alarms(DisplayScreenTableSync::GetCurrentDisplayedMessage(_env, _displayScreen->getKey()));
				BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
				{
					stream << t.row();
					stream << t.col() << priority++;
					stream << t.col() << HTMLModule::getHTMLImage((alarm->getLevel() == ALARM_LEVEL_WARNING) ? "full_screen_message_display.png" : "partial_message_display.png",	"Message : " + alarm->getShortMessage());
					stream << t.col() << "Message : " + alarm->getShortMessage();
					stream << t.col() <<
						(alarm->getScenario()->getPeriodEnd().isUnknown() ? "(illimité)" : alarm->getScenario()->getPeriodEnd().toString())
					;
					stream << t.col();

					viewScenarioRequest.setObjectId(alarm->getKey());
					stream << HTMLModule::getLinkButton(viewScenarioRequest.getURL(), "Editer", string(), "note.png");
				}

				if (DisplayScreenTableSync::GetIsAtLeastALineDisplayed(_displayScreen->getKey()))
				{
					stream << t.row();
					stream << t.col() << priority++;
					stream << t.col() << HTMLModule::getHTMLImage("times_display.png", "Affichage de lignes");
					stream << t.col() << "Horaires " << ((_displayScreen->getDirection() == DISPLAY_DEPARTURES) ? "de départ" : "d'arrivée")
						<< ((_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD) ? " avec présélection" : " chronologiques");
					stream << t.col() << "(illimité)";
					stream << t.col() << getTabLinkButton(TAB_CONTENT);
				}
				stream << t.close();

				stream << "<h1>Contenus en attente</h1>";

				vector<shared_ptr<SentAlarm> > futures(DisplayScreenTableSync::GetFutureDisplayedMessages(
					_env,
					_displayScreen->getKey()
				)	);
				if(!futures.empty())
				{
					HTMLTable::ColsVector h2;
					h2.push_back("Contenu");
					h2.push_back("Contenu");
					h2.push_back("Date début");
					h2.push_back("Date fin");
					h2.push_back("Admin");
					HTMLTable t2(h2, ResultHTMLTable::CSS_CLASS);
					stream << t2.open();
					BOOST_FOREACH(shared_ptr<SentAlarm> alarm, futures)
					{
						stream << t2.row();
						stream << t2.col() << HTMLModule::getHTMLImage((alarm->getLevel() == ALARM_LEVEL_WARNING) ? "full_screen_message_display.png" : "partial_message_display.png",	"Message : " + alarm->getShortMessage());
						stream << t2.col() << "Message : " + alarm->getShortMessage();
						stream << t2.col() << alarm->getScenario()->getPeriodStart().toString();
						stream << t2.col() << (alarm->getScenario()->getPeriodEnd().isUnknown() ? "(illimité)" : alarm->getScenario()->getPeriodEnd().toString());
						stream << t2.col();

						viewScenarioRequest.setObjectId(alarm->getKey());
						stream << HTMLModule::getLinkButton(viewScenarioRequest.getURL(), "Editer", string(), "note.png");
					}
					stream << t2.close();
				}
				else
				{
					stream << "<p>Aucun contenu en attente</p>";
				}

				stream << "<h1>Visualisation</h1>";

				stream << "<p>";
				stream << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "monitor_go.png") << " ";
				stream << "</p>";
			}
			
			
			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				// Log search
				FunctionRequest<AdminRequest> searchRequest(_request);
				searchRequest.getFunction()->setSamePage(this);

				_generalLogView.display(
					stream,
					searchRequest
				);
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool DisplayAdmin::isAuthorized() const
		{
			if (_displayScreen.get() == NULL) return false;
			if (_displayScreen->getLocalization() == NULL) return  _request->isAuthorized<ArrivalDepartureTableRight>(READ) || _request->isAuthorized<DisplayMaintenanceRight>(READ);
			return
				_request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())) ||
				_request->isAuthorized<DisplayMaintenanceRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey()));
		}

		DisplayAdmin::DisplayAdmin(
		): AdminInterfaceElementTemplate<DisplayAdmin>(),
			_now(TIME_CURRENT),
			_maintenanceLogView(TAB_MAINTENANCE),
			_generalLogView(TAB_LOG)
		{
		}

		AdminInterfaceElement::PageLinks DisplayAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if (parentLink.factoryKey == DisplaySearchAdmin::FACTORY_KEY)
			{
				if (currentPage.getFactoryKey() == FACTORY_KEY)
				{
					links.push_back(currentPage.getPageLink());
				}
			}

			return links;
		}

		
		AdminInterfaceElement::PageLinks DisplayAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}

		std::string DisplayAdmin::getTitle() const
		{
			return _displayScreen.get() ? _displayScreen->getFullName() : DEFAULT_TITLE;
		}

		std::string DisplayAdmin::getParameterName() const
		{
			return _displayScreen.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string DisplayAdmin::getParameterValue() const
		{
			return _displayScreen.get() ? Conversion::ToString(_displayScreen->getKey()) : string();
		}



		void DisplayAdmin::_buildTabs(
		) const {
			_tabs.clear();

			if (_request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())))
			{
				bool writeRight(_request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())));
				_tabs.push_back(Tab("Technique", TAB_TECHNICAL, writeRight, "cog.png"));
			}

			if (_request->isAuthorized<DisplayMaintenanceRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())))
			{
				bool writeRight(_request->isAuthorized<DisplayMaintenanceRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())));
				_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, writeRight, "wrench.png"));
			}

			if (_request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())))
			{
				bool writeRight(_request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())));
				_tabs.push_back(Tab("Sélection", TAB_CONTENT, writeRight, "times_display.png"));
				_tabs.push_back(Tab("Apparence", TAB_APPEARANCE, writeRight, "font.png"));
				_tabs.push_back(Tab("Résultat", TAB_RESULT, writeRight, "zoom.png"));
				_tabs.push_back(Tab("Journal", TAB_LOG, writeRight, "book.png"));
			}

			_tabBuilded = true;
		}
	}
}
