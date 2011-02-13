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
#include "StopAreaTableSync.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SentAlarm.h"
#include "DisplayAdmin.h"
#include "DeparturesTableModule.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "UpdateDisplayScreenAction.h"
#include "DisplayScreenTableSync.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "AddPreselectionPlaceToDisplayScreenAction.h"
#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "UpdateAllStopsDisplayScreenAction.h"
#include "AddDepartureStopToDisplayScreenAction.h"
#include "AddForbiddenPlaceToDisplayScreenAction.h"
#include "DisplayScreenAddDisplayedPlaceAction.h"
#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "DisplaySearchAdmin.h"
#include "DisplayScreenRemoveAction.h"
#include "DisplayScreenContentFunction.h"
#include "ArrivalDepartureTableRight.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMaintenanceRight.h"
#include "05_html/Constants.h"
#include "HTMLList.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "DisplayTypeAdmin.h"
#include "StopPointTableSync.hpp"
#include "LineStopTableSync.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "ArrivalDepartureTableLog.h"
#include "MessageAdmin.h"
#include "SentScenario.h"
#include "MessagesScenarioAdmin.h"
#include "Interface.h"
#include "DisplayScreenCPUAdmin.h"
#include "City.h"
#include "DisplayScreenTransferDestinationAddAction.h"
#include "DisplayScreenTransferDestinationRemoveAction.h"
#include "Profile.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayScreenUpdateDisplayedStopAreaAction.hpp"
#include "PTPlaceAdmin.h"

#include <utility>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	using namespace html;
	using namespace db;
	using namespace departure_boards;
	using namespace security;
	using namespace dblog;
	using namespace messages;
	using namespace graph;
	using namespace pt;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayAdmin>::FACTORY_KEY("display");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DisplayAdmin>::DEFAULT_TITLE("Afficheur inconnu");
	}

	namespace departure_boards
	{
		const string DisplayAdmin::TAB_TECHNICAL("tech");
		const string DisplayAdmin::TAB_MAINTENANCE("maint");
		const string DisplayAdmin::TAB_CONTENT("content");
		const string DisplayAdmin::TAB_APPEARANCE("appear");
		const string DisplayAdmin::TAB_RESULT("result");
		const string DisplayAdmin::TAB_LOG("log");



		void DisplayAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_displayScreen = Env::GetOfficialEnv().get<DisplayScreen>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
				
				_status = DisplayMonitoringStatusTableSync::GetStatus(
					_displayScreen->getKey()
				);
				_maintenanceLogView.set(map, DisplayMaintenanceLog::FACTORY_KEY, _displayScreen->getKey());
				_generalLogView.set(map, ArrivalDepartureTableLog::FACTORY_KEY, _displayScreen->getKey());
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw AdminParametersException("Display screen not found");
			}
			catch (ObjectNotFoundException<StopArea>&)
			{
				throw AdminParametersException("Place not found");
			}
		}
		
		
		
		ParametersMap DisplayAdmin::getParametersMap() const
		{
			ParametersMap m(_maintenanceLogView.getParametersMap());
			m.merge(_generalLogView.getParametersMap());
			if(_displayScreen.get()) m.insert(Request::PARAMETER_OBJECT_ID, _displayScreen->getKey());
			return m;
		}



		void DisplayAdmin::display(
			std::ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			vector<pair<optional<DisplayScreen::SubScreenType>, string> > subscreenTypeFilterMap;
			subscreenTypeFilterMap.push_back(make_pair(DisplayScreen::SUB_CONTENT, DisplayScreen::GetSubScreenTypeLabel(DisplayScreen::SUB_CONTENT)));
			subscreenTypeFilterMap.push_back(make_pair(DisplayScreen::CONTINUATION_TRANSFER, DisplayScreen::GetSubScreenTypeLabel(DisplayScreen::CONTINUATION_TRANSFER)));

			////////////////////////////////////////////////////////////////////
			// TECHNICAL TAB
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				// Update request
				AdminActionFunctionRequest<UpdateDisplayScreenAction,DisplayAdmin> updateDisplayRequest(_request);
				updateDisplayRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Delete the screen request
				AdminActionFunctionRequest<DisplayScreenRemoveAction, DisplaySearchAdmin> deleteRequest(_request);
				deleteRequest.getAction()->setDisplayScreen(_displayScreen);

				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateDisplayRequest.getHTMLForm("updateprops"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.title("Emplacement");
				if(_displayScreen->getLocation())
				{
					stream << t.cell("Localisation", _displayScreen->getLocation()->getFullName());
				}
				else
				{
					stream << t.cell("Localisation","Stock");
				}
				if(_displayScreen->getParent())
				{
					stream << t.cell("Fils de", _displayScreen->getParent()->getFullName());

					stream <<
						t.cell(
							"Rôle",
							t.getForm().getSelectInput(
								UpdateDisplayScreenAction::PARAMETER_SUB_SCREEN_TYPE,
								subscreenTypeFilterMap,
								optional<DisplayScreen::SubScreenType>(_displayScreen->getSubScreenType())
						)	)
					;
				}
				stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_NAME, _displayScreen->getName()));

				stream << t.title("Données techniques");
				stream << t.cell("UID", Conversion::ToString(_displayScreen->getKey()));
				stream <<
					t.cell(
						"Type d'afficheur",
						t.getForm().getSelectInput(
							UpdateDisplayScreenAction::PARAMETER_TYPE,
							DeparturesTableModule::getDisplayTypeLabels(false, _displayScreen->getType() == NULL),
							_displayScreen->getType() ? _displayScreen->getType()->getKey() : optional<RegistryKeyType>()
					)	)
				;
				stream << t.cell("Adresse MAC", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_MAC_ADDRESS, _displayScreen->getMacAddress()));
				
				stream << t.title("Connexion");
				
				if (_displayScreen->getLocation() != NULL && _displayScreen->getParent() == NULL)
				{
					// CPU search
					DisplayScreenCPUTableSync::SearchResult cpus(
						DisplayScreenCPUTableSync::Search(
							Env::GetOfficialEnv(),
							_displayScreen->getLocation()->getKey()
					)	);
					
					if(!cpus.empty())
					{
						AdminFunctionRequest<DisplayScreenCPUAdmin> goCPURequest(_request);
						if(_displayScreen->getRoot<DisplayScreenCPU>())
						{
							goCPURequest.getPage()->setCPU(
								Env::GetOfficialEnv().getSPtr(_displayScreen->getRoot<DisplayScreenCPU>())
							);
						}
						
						stream << t.cell(
							"Unité centrale",
							t.getForm().getSelectInput(
								UpdateDisplayScreenAction::PARAMETER_CPU,
								cpus,
								optional<shared_ptr<DisplayScreenCPU> >(
									Env::GetOfficialEnv().getEditableSPtr(
										const_cast<DisplayScreenCPU*>(_displayScreen->getRoot<DisplayScreenCPU>())
								)	),
								"(pas d'unité centrale)"
							) + " " +(
								_displayScreen->getRoot<DisplayScreenCPU>() ?
								goCPURequest.getHTMLForm().getLinkButton(
									"Ouvrir",
									string(),
									"server.png"
								) :
								string()
							)
						);
					}
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
				stream << t.cell("Code de branchement bus RS485", t.getForm().getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()));

				stream << t.close();

				stream << "<p class=\"info\">Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalités proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans effet sur l'affichage.</p>";

				if (deleteRequest.isActionFunctionAuthorized())
				{
					stream << "<h1>Suppression</h1>";
					stream << "<p>";
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'afficheur " + _displayScreen->getFullName() + " ?", "monitor_delete.png") << " ";
					stream << "</p>";
				}
			}

			////////////////////////////////////////////////////////////////////
			// MAINTENANCE TAB
			if (openTabContent(stream, TAB_MAINTENANCE))
			{
				// Update action
				AdminActionFunctionRequest<UpdateDisplayMaintenanceAction,DisplayAdmin> updateRequest(_request);
				updateRequest.getAction()->setScreenId(_displayScreen->getKey());
				
				// View the display type
				AdminFunctionRequest<DisplayTypeAdmin> displayTypeRequest(_request);
				displayTypeRequest.getPage()->setType(
					Env::GetOfficialEnv().getSPtr(_displayScreen->getType())
				);
				
				// Log search
				AdminFunctionRequest<DisplayAdmin> searchRequest(_request);

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
							3, 60, false
					)	)
				;
				stream << t.close();

				stream << "<h1>Informations de supervision</h1>";

				bool monitored(_displayScreen->isMonitored());
				
				HTMLList l;
				stream << l.open();


				stream << l.element();
				if(_displayScreen->getType() == NULL)
				{
					stream <<
						HTMLModule::getHTMLImage("error.png", "Erreur") <<
						" KO : Veuillez définir le type d'afficheur dans l'écran de configuration."
					;
				}
				else
				{
					stream <<
						"Type d'afficheur : " <<
						HTMLModule::getHTMLLink(
							displayTypeRequest.getURL(),
							_displayScreen->getType()->getName()
						)
					;
				}


				
				if(!monitored)
				{
					stream <<
						l.element() <<
						HTMLModule::getHTMLImage("help.png", "Information") <<
						" Ce type d'afficheur n'est pas supervisé."
					;
				} else {
					stream <<
						l.element() <<
						"Durée théorique entre les contacts de supervision : " <<
						_displayScreen->getType()->getTimeBetweenChecks() << " min"
					;

					if(_status.get() == NULL)
					{
						stream <<
							l.element() <<
							HTMLModule::getHTMLImage("exclamation.png", "Statut KO") <<
							" KO : Cet afficheur n'est jamais entré en contact.";
					}
					else
					{
						if(	_displayScreen->isDown(*_status)
						){
							stream <<
								l.element() <<
								HTMLModule::getHTMLImage("exclamation.png", "Statut KO") <<
								" KO : Cet afficheur n'est plus en contact alors qu'il est déclaré online."
							;
						}
					}
				}

				if(_status.get() != NULL)
				{
					DisplayMonitoringStatus::Status globalStatus(_status->getGlobalStatus());
					stream << l.element() << "Dernière mesure le " << to_simple_string(_status->getTime());
					stream << l.element() << "Dernier état mesuré : " <<
						HTMLModule::getHTMLImage(
							DisplayMonitoringStatus::GetStatusIcon(globalStatus),
							DisplayMonitoringStatus::GetStatusString(globalStatus)
						) <<
						" " <<
						DisplayMonitoringStatus::GetStatusString(globalStatus)
					;
					stream << l.element() << "Température : "
						<< *_status->getTemperatureValue();
					stream << l.element() << "Détail : "
						<< _status->getDetail();
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
				// Add display request
				AdminActionFunctionRequest<DisplayScreenAddDisplayedPlaceAction,DisplayAdmin> addDisplayRequest(_request);
				addDisplayRequest.getAction()->setScreen(_displayScreen);

				// Remove displayed place request
				AdminActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,DisplayAdmin> rmDisplayedRequest(_request);
				rmDisplayedRequest.getAction()->setScreen(_displayScreen->getKey());

				// Update request
				AdminActionFunctionRequest<UpdateAllStopsDisplayScreenAction,DisplayAdmin> updateAllDisplayRequest(_request);
				updateAllDisplayRequest.getAction()->setScreen(_displayScreen);

				// Add physical request
				AdminActionFunctionRequest<AddDepartureStopToDisplayScreenAction,DisplayAdmin> addPhysicalRequest(_request);
				addPhysicalRequest.getAction()->setScreen(_displayScreen);

				// Add preselection request
				AdminActionFunctionRequest<AddPreselectionPlaceToDisplayScreenAction,DisplayAdmin> addPreselRequest(_request);
				addPreselRequest.getAction()->setScreen(_displayScreen);

				// Add not to serve request
				AdminActionFunctionRequest<AddForbiddenPlaceToDisplayScreenAction,DisplayAdmin> addNSRequest(_request);
				addNSRequest.getAction()->setScreen(_displayScreen);

				// Update preselection request
				AdminActionFunctionRequest<UpdateDisplayPreselectionParametersAction,DisplayAdmin> updPreselRequest(_request);
				updPreselRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Remove preselection stop request
				AdminActionFunctionRequest<RemovePreselectionPlaceFromDisplayScreenAction,DisplayAdmin> rmPreselRequest(_request);
				rmPreselRequest.getAction()->setScreen(_displayScreen);

				// Remove physical stop request
				AdminActionFunctionRequest<DisplayScreenRemovePhysicalStopAction,DisplayAdmin> rmPhysicalRequest(_request);
				rmPhysicalRequest.getAction()->setScreen(_displayScreen);

				// Remove Forbidden place request
				AdminActionFunctionRequest<DisplayScreenRemoveForbiddenPlaceAction,DisplayAdmin> rmForbiddenRequest(_request);
				rmForbiddenRequest.getAction()->setScreen(_displayScreen);

				// Change displayed stop area request
				AdminActionFunctionRequest<DisplayScreenUpdateDisplayedStopAreaAction,DisplayAdmin> updateDisplayedPlaceRequest(_request);
				updateDisplayedPlaceRequest.getAction()->setScreen(const_pointer_cast<DisplayScreen>(_displayScreen));

				vector<pair<optional<EndFilter>, string> > endFilterMap;
				endFilterMap.push_back(make_pair(WITH_PASSING, "Origines/Terminus et passages"));
				endFilterMap.push_back(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));

				{	// Properties
					stream << "<h1>Propriétés</h1>";

					PropertiesHTMLTable t(updPreselRequest.getHTMLForm("updpresel"));
					t.getForm().setUpdateRight(tabHasWritePermissions());

					stream << t.open();
					stream << t.title("Contenu");
					stream << t.cell("Type de contenu", t.getForm().getRadioInputCollection(
						UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_FUNCTION,
						UpdateDisplayPreselectionParametersAction::GetFunctionList(),
						optional<UpdateDisplayPreselectionParametersAction::DisplayFunction>(UpdateDisplayPreselectionParametersAction::GetFunction(*_displayScreen)),
						true
						)	);
					stream << t.cell("Terminus", t.getForm().getRadioInputCollection(
						UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_END_FILTER,
						endFilterMap,
						optional<EndFilter>(_displayScreen->getEndFilter()),
						true
						)	);
					stream << t.cell("Délai maximum d'affichage", t.getForm().getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) + " minutes");
					stream << t.cell("Délai d'effacement", t.getForm().getSelectInput(
						UpdateDisplayPreselectionParametersAction::PARAMETER_CLEANING_DELAY,
						UpdateDisplayPreselectionParametersAction::GetClearDelaysList(),
						optional<int>(_displayScreen->getClearingDelay())
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
				}

				{	// Displayed stop area
					stream << "<h1>Arrêt de départ</h1>";
					PropertiesHTMLTable t(updateDisplayedPlaceRequest.getHTMLForm("stopareachange"));
					t.getForm().setUpdateRight(tabHasWritePermissions());
					stream << t.open();
					stream << t.cell("Localité", t.getForm().getTextInput(DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_CITY_NAME, _displayScreen->getDisplayedPlace() ? _displayScreen->getDisplayedPlace()->getCity()->getName() : string()));
					stream << t.cell("Arrêt", t.getForm().getTextInput(DisplayScreenUpdateDisplayedStopAreaAction::PARAMETER_PLACE_NAME, _displayScreen->getDisplayedPlace() ? _displayScreen->getDisplayedPlace()->getName() : string()));
					stream << t.close();
				}

				

				if(_displayScreen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING)
				{
					stream << "<h1>Arrêts de destination</h1>";

					if(_displayScreen->getDisplayedPlace() == NULL)
					{
						stream << "Arrêt de départ non spécifié, aucune destination ne peut être sélectionnée.";
					}
					else
					{
						HTMLForm f(addDisplayRequest.getHTMLForm("addplace"));

						HTMLTable::ColsVector c;
						c.push_back("Commune");
						c.push_back("Arrêt");
						c.push_back("Actions");
						HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
						stream << f.open() << t.open();

						BOOST_FOREACH(const DisplayedPlacesList::value_type& it, _displayScreen->getDisplayedPlaces())
						{
							rmDisplayedRequest.getAction()->setPlace(it.second->getKey());
							stream << t.row();
							stream << t.col() << it.second->getCity()->getName();
							stream << t.col() << it.second->getName();
							stream << t.col() << rmDisplayedRequest.getHTMLForm().getLinkButton(
								"Supprimer",
								"Etes-vous sûr de vouloir supprimer la destination sélectionnée ?",
								"delete.png"
							);

						}

						stream << t.row();
						stream << t.col() << f.getTextInput(DisplayScreenAddDisplayedPlaceAction::PARAMETER_CITY_NAME, string());
						stream << t.col() << f.getTextInput(DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE_NAME, string());
						stream << t.col() << f.getSubmitButton("Ajouter");

						stream << t.close() << f.close();
					}
				}
				else
				{
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
						if(_displayScreen->getDisplayedPlace() == NULL)
						{
							stream << "Arrêt de départ non spécifié, aucun arrêt à sélectionner.";
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
								_displayScreen->getDisplayedPlace()->getPhysicalStops()
							){
								stream << t.row();
								stream << t.col() << it.second->getName();
								stream << t.col() << it.second->getCodeBySource();
								
								// Lines column
								stream << t.col();
								set<const CommercialLine*> lines;
								BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
								{
									lines.insert(
										static_cast<const LineStop*>(edge.second)->getLine()->getCommercialLine()
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
							lexical_cast<string>(it.second->getKey())
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
						AddForbiddenPlaceToDisplayScreenAction::PARAMETER_PLACE,
						_displayScreen->getSortedAvaliableDestinationsLabels(
							_displayScreen->getForbiddenPlaces()
						),
						optional<RegistryKeyType>(0)
					);
					stream << l.close() << ant.close();
				}

				if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
				{
					// Additional preselection stops
					stream << "<h1>Arrêts de présélection</h1>";

					HTMLList l;
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
							AddPreselectionPlaceToDisplayScreenAction::PARAMETER_PLACE,
							_displayScreen->getSortedAvaliableDestinationsLabels(
								_displayScreen->getForcedDestinations()
							),
							optional<RegistryKeyType>(0)
						)
					;
					stream << l.close() << psaf.close();

					stream << "<p class=\"info\">Les terminus de lignes sont automatiquement présélectionnés.</p>";
				}

				stream << "<h1>Contenus inclus</h1>";

				AdminFunctionRequest<DisplayAdmin> displayRequest(_request);

				AdminFunctionRequest<PTPlaceAdmin> displayPlaceRequest(_request);

				AdminActionFunctionRequest<CreateDisplayScreenAction,DisplayAdmin> createDisplayRequest(
					_request
				);
				createDisplayRequest.setActionWillCreateObject();
				createDisplayRequest.getAction()->setUp(_displayScreen);

				AdminActionFunctionRequest<DisplayScreenRemoveAction,DisplayAdmin> removeDisplayRequest(
					_request
				);

				HTMLTable::ColsVector c;
				c.push_back("Nom");
				c.push_back("Rôle");
				c.push_back("Lieu");
				c.push_back("ID");
				c.push_back("Actions");
				c.push_back("Actions");
				HTMLTable td(c, ResultHTMLTable::CSS_CLASS);
				HTMLForm f(createDisplayRequest.getHTMLForm("create_sub_string"));
				stream << f.open() << td.open();
				BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, _displayScreen->getChildren())
				{
					const DisplayScreen& screen(*it.second);
					displayRequest.getPage()->setScreen(Env::GetOfficialEnv().getSPtr(&screen));
					removeDisplayRequest.getAction()->setDisplayScreen(Env::GetOfficialEnv().getSPtr(&screen));

					stream << td.row();
					stream << td.col() << HTMLModule::getHTMLLink(displayRequest.getHTMLForm().getURL(), screen.getName());
					stream << td.col() << DisplayScreen::GetSubScreenTypeLabel(screen.getSubScreenType());
					
					// Displayed place
					stream << td.col();
					if(screen.getDisplayedPlace())
					{
						displayPlaceRequest.getPage()->setConnectionPlace(Env::GetOfficialEnv().getSPtr(screen.getDisplayedPlace()));
						stream << HTMLModule::getHTMLLink(displayRequest.getHTMLForm().getURL(), screen.getDisplayedPlace()->getFullName());
					}
					stream << td.col() << screen.getKey();
					stream << td.col() << displayRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "monitor.png");
					stream << td.col() << removeDisplayRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr de vouloir supprimer le contenu "+ screen.getName() + " ?", "monitor_delete.png");
				}

				// Creation form
				stream << td.row();
				stream << td.col() << f.getTextInput(CreateDisplayScreenAction::PARAMETER_NAME, string(), "(nom)");
				stream << td.col() << f.getSelectInput(
					CreateDisplayScreenAction::PARAMETER_SUB_SCREEN_TYPE,
					subscreenTypeFilterMap,
					optional<DisplayScreen::SubScreenType>(DisplayScreen::SUB_CONTENT)
				);
				stream << td.col(2);
				stream << td.col() << f.getSubmitButton("Ajouter");
				stream << td.close() << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// APPEARANCE TAB
			if (openTabContent(stream, TAB_APPEARANCE))
			{
				// Properties Update request
				AdminActionFunctionRequest<DisplayScreenAppearanceUpdateAction,DisplayAdmin> updateRequest(_request);
				updateRequest.getAction()->setScreenId(_displayScreen->getKey());

				// Maps for particular select fields
				vector<pair<optional<int>, string> > blinkingDelaysMap;
				blinkingDelaysMap.push_back(make_pair(0, "Pas de clignotement"));
				blinkingDelaysMap.push_back(make_pair(1, "1 minute avant disparition"));
				for (int i=2; i<6; ++i)
				{
					blinkingDelaysMap.push_back(make_pair(i, lexical_cast<string>(i) + " minutes avant disparition"));
				}

				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.cell("Titre", t.getForm().getTextInput(DisplayScreenAppearanceUpdateAction::PARAMETER_TITLE, _displayScreen->getTitle()));
				stream << t.cell(
					"Clignotement",
					t.getForm().getSelectInput(
						DisplayScreenAppearanceUpdateAction::PARAMETER_BLINKING_DELAY,
						blinkingDelaysMap,
						optional<int>(_displayScreen->getBlinkingDelay())
				)	);
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

				// Intermediate stops to display
				if(_displayScreen->getGenerationMethod() != DisplayScreen::ROUTE_PLANNING)
				{
					// Add display request
					AdminActionFunctionRequest<DisplayScreenAddDisplayedPlaceAction,DisplayAdmin> addDisplayRequest(_request);
					addDisplayRequest.getAction()->setScreen(_displayScreen);

					// Remove displayed place request
					AdminActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,DisplayAdmin> rmDisplayedRequest(_request);
					rmDisplayedRequest.getAction()->setScreen(_displayScreen->getKey());

					stream << "<h1>Affichage arrêts intermédiaires</h1>";

					HTMLForm f(addDisplayRequest.getHTMLForm("addplace"));

					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Arrêt");
					c.push_back("Actions");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << f.open() << t.open();

					BOOST_FOREACH(const DisplayedPlacesList::value_type& it, _displayScreen->getDisplayedPlaces())
					{
						rmDisplayedRequest.getAction()->setPlace(it.second->getKey());
						stream << t.row();
						stream << t.col() << it.second->getCity()->getName();
						stream << t.col() << it.second->getName();
						stream << t.col() << rmDisplayedRequest.getHTMLForm().getLinkButton(
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer la destination sélectionnée ?",
							"delete.png"
						);
					}

					stream << t.row();
					stream << t.col(2) << f.getSelectInput(
						DisplayScreenAddDisplayedPlaceAction::PARAMETER_PLACE,
						_displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()),
						optional<RegistryKeyType>(0)
					);
						
					stream << t.col() << f.getSubmitButton("Ajouter");

					stream << t.close() << f.close();


					// Add transfer request
					AdminActionFunctionRequest<DisplayScreenTransferDestinationAddAction,DisplayAdmin> addTransferRequest(_request);
					addTransferRequest.getAction()->setScreen(_displayScreen);

					// Remove transfer place request
					AdminActionFunctionRequest<DisplayScreenTransferDestinationRemoveAction,DisplayAdmin> rmTransferRequest(_request);
					rmTransferRequest.getAction()->setScreen(_displayScreen);

					stream << "<h1>Affichage de correspondances</h1>";
					HTMLForm ft(addTransferRequest.getHTMLForm("addtransfer"));

					HTMLTable::ColsVector ct;
					ct.push_back("Arrêt de correspondance");
					ct.push_back("Arrêt de correspondance");
					ct.push_back("Destination");
					ct.push_back("Destination");
					ct.push_back("Actions");
					HTMLTable tt(ct, ResultHTMLTable::CSS_CLASS);
					stream << ft.open() << tt.open();

					BOOST_FOREACH(const TransferDestinationsList::value_type& it, _displayScreen->getTransferdestinations())
					{
						rmTransferRequest.getAction()->setTransferPlace(StopAreaTableSync::Get(it.first->getKey(), Env::GetOfficialEnv()));

						BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it.second)
						{
							rmTransferRequest.getAction()->setDestinationPlace(StopAreaTableSync::Get(it2->getKey(), Env::GetOfficialEnv()));

							stream << tt.row();
							stream << tt.col() << it.first->getCity()->getName();
							stream << tt.col() << it.first->getName();
							stream << tt.col() << it2->getCity()->getName();
							stream << tt.col() << it2->getName();
							stream << tt.col() << rmTransferRequest.getHTMLForm().getLinkButton(
								"Supprimer",
								"Etes-vous sûr de vouloir supprimer la destination sélectionnée ?",
								"delete.png"
							);
					}	}

					stream << tt.row();
					stream << tt.col(2) << ft.getSelectInput(
						DisplayScreenTransferDestinationAddAction::PARAMETER_TRANSFER_PLACE_ID,
						_displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()),
						optional<RegistryKeyType>(0)
					);
					stream << tt.col() << ft.getTextInput(
						DisplayScreenTransferDestinationAddAction::PARAMETER_DESTINATION_PLACE_CITY_NAME,
						string(),
						"(commune)"
					);
					stream << tt.col() << ft.getTextInput(
						DisplayScreenTransferDestinationAddAction::PARAMETER_DESTINATION_PLACE_NAME,
						string(),
						"(arrêt)"
					);

					stream << tt.col() << ft.getSubmitButton("Ajouter");

					stream << tt.close() << ft.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// RESULT TAB
			if (openTabContent(stream, TAB_RESULT))
			{
				// Requests
				StaticFunctionRequest<DisplayScreenContentFunction> viewRequest(_request, true);
				viewRequest.getFunction()->setScreen(_displayScreen);
				if(	_displayScreen->getType() &&
					_displayScreen->getType()->getDisplayInterface() &&
					!_displayScreen->getType()->getDisplayInterface()->getDefaultClientURL().empty()
				){
					viewRequest.setClientURL(_displayScreen->getType()->getDisplayInterface()->getDefaultClientURL());
				}

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

				vector<shared_ptr<SentAlarm> > alarms(DisplayScreenTableSync::GetCurrentDisplayedMessage(Env::GetOfficialEnv(), _displayScreen->getKey()));
				AdminFunctionRequest<MessageAdmin> viewMessageRequest(_request);
				BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
				{
					// Avoid malformed message
					if(alarm->getScenario() == NULL) continue;

					stream << t.row();
					stream << t.col() << priority++;
					stream << t.col() << HTMLModule::getHTMLImage((alarm->getLevel() == ALARM_LEVEL_WARNING) ? "full_screen_message_display.png" : "partial_message_display.png",	"Message : " + alarm->getShortMessage());
					stream << t.col() << "Message : " + alarm->getShortMessage();
					stream << t.col() <<
						(alarm->getScenario()->getPeriodEnd().is_not_a_date_time() ? "(illimité)" : to_simple_string(alarm->getScenario()->getPeriodEnd()))
					;
					stream << t.col();

					viewMessageRequest.getPage()->setMessage(alarm);
					stream << HTMLModule::getLinkButton(viewMessageRequest.getURL(), "Ouvrir", string(), "note.png");
				}

				if (DisplayScreenTableSync::GetIsAtLeastALineDisplayed(_displayScreen->getKey()))
				{
					stream << t.row();
					stream << t.col() << priority++;
					stream << t.col() << HTMLModule::getHTMLImage("times_display.png", "Affichage de lignes");
					stream << t.col();
					if(_displayScreen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING)
					{
						stream << "Calcul d'itinéraires " << (_displayScreen->getRoutePlanningWithTransfer() ? "avec" : "sans") << " correspondances";
					}
					else
					{
						stream << "Horaires " << ((_displayScreen->getDirection() == DISPLAY_DEPARTURES) ? "de départ" : "d'arrivée")
							<< ((_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD) ? " avec présélection" : " chronologiques");
					}
					stream << t.col() << "(illimité)";
					stream << t.col() << getTabLinkButton(TAB_CONTENT);
				}
				stream << t.close();

				stream << "<h1>Contenus en attente</h1>";

				vector<shared_ptr<SentAlarm> > futures(DisplayScreenTableSync::GetFutureDisplayedMessages(
					Env::GetOfficialEnv(),
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
						stream << t2.col() << alarm->getScenario()->getPeriodStart();
						stream << t2.col() << (alarm->getScenario()->getPeriodEnd().is_not_a_date_time() ? "(illimité)" : to_simple_string(alarm->getScenario()->getPeriodEnd()));
						stream << t2.col();

						viewMessageRequest.getPage()->setMessage(alarm);
						stream << HTMLModule::getLinkButton(viewMessageRequest.getURL(), "Ouvrir", string(), "note.png");
					}
					stream << t2.close();
				}
				else
				{
					stream << "<p>Aucun contenu en attente</p>";
				}

				stream << "<h1>Visualisation en direct</h1>";

				stream << "<p>";
				stream << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "monitor_go.png") << " ";
				stream << "</p>";

				stream << "<h1>Simulation</h1>";

				stream << "<p>";
				HTMLForm viewForm(viewRequest.getHTMLForm("simulate"));
				ptime now(second_clock::local_time());
				stream << viewForm.open();
				stream << viewForm.getCalendarInput(DisplayScreenContentFunction::PARAMETER_DATE, now) << " ";
				stream << viewForm.getSubmitButton("Simuler");
				stream << viewForm.close();
				stream << "</p>";
			}
			
			
			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				_generalLogView.display(stream, AdminFunctionRequest<DisplayAdmin>(_request));
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool DisplayAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_displayScreen.get() == NULL) return false;
			if (_displayScreen->getLocation() == NULL) return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ) || user.getProfile()->isAuthorized<DisplayMaintenanceRight>(READ);
			return
				user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocation()->getKey())) ||
				user.getProfile()->isAuthorized<DisplayMaintenanceRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocation()->getKey()));
		}

		DisplayAdmin::DisplayAdmin(
		): AdminInterfaceElementTemplate<DisplayAdmin>(),
			_maintenanceLogView(TAB_MAINTENANCE),
			_generalLogView(TAB_LOG)
		{
		}


		std::string DisplayAdmin::getTitle() const
		{
			return _displayScreen.get() ? _displayScreen->getName() : DEFAULT_TITLE;
		}



		void DisplayAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			if(	_displayScreen->getLocation() &&
				profile.isAuthorized<ArrivalDepartureTableRight>(
					READ,
					UNKNOWN_RIGHT_LEVEL,
					lexical_cast<string>(_displayScreen->getLocation()->getKey())
				) ||
				!_displayScreen->getLocation() &&
				profile.isAuthorized<ArrivalDepartureTableRight>(
					READ,
					UNKNOWN_RIGHT_LEVEL
			)	){
				bool writeRight(
					_displayScreen->getLocation() ?
					profile.isAuthorized<ArrivalDepartureTableRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL,
						Conversion::ToString(_displayScreen->getLocation()->getKey())
					) :
					profile.isAuthorized<ArrivalDepartureTableRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL
				)	);
				_tabs.push_back(Tab("Technique", TAB_TECHNICAL, writeRight, "cog.png"));
			}

			if(	_displayScreen->getLocation() &&
				profile.isAuthorized<DisplayMaintenanceRight>(
					READ,
					UNKNOWN_RIGHT_LEVEL,
					lexical_cast<string>(_displayScreen->getLocation()->getKey())
				) ||
				!_displayScreen->getLocation() &&
				profile.isAuthorized<DisplayMaintenanceRight>(READ, UNKNOWN_RIGHT_LEVEL)
			){
				bool writeRight(
					_displayScreen->getLocation() ?
					profile.isAuthorized<DisplayMaintenanceRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL,
						lexical_cast<string>(_displayScreen->getLocation()->getKey())
					) :
					profile.isAuthorized<DisplayMaintenanceRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL
				)	);
				_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, writeRight, "wrench.png"));
			}

			if (_displayScreen->getLocation() &&
				profile.isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_displayScreen->getLocation()->getKey())) ||
				!_displayScreen->getLocation() &&
				profile.isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL)
			){
				bool writeRight(
					_displayScreen->getLocation() ?
					profile.isAuthorized<ArrivalDepartureTableRight>(
						WRITE,
						UNKNOWN_RIGHT_LEVEL,
						lexical_cast<string>(_displayScreen->getLocation()->getKey())
					) :
					profile.isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL)
				);
				_tabs.push_back(Tab("Sélection", TAB_CONTENT, writeRight, "times_display.png"));
				if(_displayScreen->getGenerationMethod() != DisplayScreen::DISPLAY_CHILDREN_ONLY)
				{
					_tabs.push_back(Tab("Apparence", TAB_APPEARANCE, writeRight, "font.png"));
				}
				_tabs.push_back(Tab("Résultat", TAB_RESULT, writeRight, "zoom.png"));

				if (ArrivalDepartureTableLog::IsAuthorized(profile, READ))
				{
					_tabs.push_back(Tab("Journal", TAB_LOG, writeRight, "book.png"));
				}
			}

			_tabBuilded = true;
		}
		
		bool DisplayAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _displayScreen == static_cast<const DisplayAdmin&>(other)._displayScreen;
		}

		void DisplayAdmin::setScreen(boost::shared_ptr<const DisplayScreen> value)
		{
			_displayScreen = value;
		}
		
		boost::shared_ptr<const DisplayScreen> DisplayAdmin::getScreen() const
		{
			return _displayScreen;
		}



		AdminInterfaceElement::PageLinks DisplayAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			const DisplayAdmin* da(dynamic_cast<const DisplayAdmin*>(&currentPage));

			if(	da &&
				(da->getScreen() == _displayScreen || da->getScreen()->isChildOf(*_displayScreen))
			){
				BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, _displayScreen->getChildren())
				{
					shared_ptr<DisplayAdmin> p(
						getNewPage<DisplayAdmin>()
					);
					p->setScreen(Env::GetOfficialEnv().getSPtr(it.second));
					links.push_back(p);
				}
			}

			return links;
		}
	}
}
