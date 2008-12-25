
/** DisplayAdmin class implementation.
	@file DisplayAdmin.cpp

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

#include <vector>
#include <utility>
#include <sstream>

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

		void DisplayAdmin::display(
			std::ostream& stream,
			interfaces::VariablesMap& variables
		) const	{
			////////////////////////////////////////////////////////////////////
			// TECHNICAL TAB
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				// Update request
				ActionFunctionRequest<UpdateDisplayScreenAction,AdminRequest> updateDisplayRequest(_request);
				updateDisplayRequest.getFunction()->setPage<DisplayAdmin>();
				updateDisplayRequest.setObjectId(_request->getObjectId());

				// Update request
				ActionFunctionRequest<UpdateAllStopsDisplayScreenAction,AdminRequest> updateAllDisplayRequest(_request);
				updateAllDisplayRequest.getFunction()->setPage<DisplayAdmin>();
				updateAllDisplayRequest.setObjectId(_request->getObjectId());

				// Add physical request
				ActionFunctionRequest<AddDepartureStopToDisplayScreenAction,AdminRequest> addPhysicalRequest(_request);
				addPhysicalRequest.getFunction()->setPage<DisplayAdmin>();
				addPhysicalRequest.setObjectId(_request->getObjectId());

				// Add preselection request
				ActionFunctionRequest<AddPreselectionPlaceToDisplayScreen,AdminRequest> addPreselRequest(_request);
				addPreselRequest.getFunction()->setPage<DisplayAdmin>();
				addPreselRequest.setObjectId(_request->getObjectId());

				// Add display request
				ActionFunctionRequest<DisplayScreenAddDisplayedPlace,AdminRequest> addDisplayRequest(_request);
				addDisplayRequest.getFunction()->setPage<DisplayAdmin>();
				addDisplayRequest.setObjectId(_request->getObjectId());

				// Add not to serve request
				ActionFunctionRequest<AddForbiddenPlaceToDisplayScreen,AdminRequest> addNSRequest(_request);
				addNSRequest.getFunction()->setPage<DisplayAdmin>();
				addNSRequest.setObjectId(_request->getObjectId());

				// Update preselection request
				ActionFunctionRequest<UpdateDisplayPreselectionParametersAction,AdminRequest> updPreselRequest(_request);
				updPreselRequest.getFunction()->setPage<DisplayAdmin>();
				updPreselRequest.setObjectId(_request->getObjectId());

				// Remove preselection stop request
				ActionFunctionRequest<RemovePreselectionPlaceFromDisplayScreenAction,AdminRequest> rmPreselRequest(_request);
				rmPreselRequest.getFunction()->setPage<DisplayAdmin>();
				rmPreselRequest.setObjectId(_request->getObjectId());

				// Remove physical stop request
				ActionFunctionRequest<DisplayScreenRemovePhysicalStopAction,AdminRequest> rmPhysicalRequest(_request);
				rmPhysicalRequest.getFunction()->setPage<DisplayAdmin>();
				rmPhysicalRequest.setObjectId(_request->getObjectId());

				// Remove displayed place request
				ActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,AdminRequest> rmDisplayedRequest(_request);
				rmDisplayedRequest.getFunction()->setPage<DisplayAdmin>();
				rmDisplayedRequest.setObjectId(_request->getObjectId());

				// Remove Forbidden place request
				ActionFunctionRequest<DisplayScreenRemoveForbiddenPlaceAction,AdminRequest> rmForbiddenRequest(_request);
				rmForbiddenRequest.getFunction()->setPage<DisplayAdmin>();
				rmForbiddenRequest.setObjectId(_request->getObjectId());

				// Delete the screen request
				ActionFunctionRequest<DisplayScreenRemove, AdminRequest> deleteRequest(_request);
				deleteRequest.getFunction()->setPage<DisplaySearchAdmin>();
				deleteRequest.getAction()->setDisplayScreen(_displayScreen);

				FunctionRequest<DisplayScreenContentRequest> viewRequest(_request);
				viewRequest.setObjectId(_displayScreen->getKey());

				// Maps for particular select fields
				vector<pair<int, string> > blinkingDelaysMap;
				blinkingDelaysMap.push_back(make_pair(0, "Pas de clignotement"));
				blinkingDelaysMap.push_back(make_pair(1, "1 minute avant disparition"));
				for (int i=2; i<6; ++i)
					blinkingDelaysMap.push_back(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));

				vector<pair<DeparturesTableDirection, string> > directionMap;
				directionMap.push_back(make_pair(DISPLAY_ARRIVALS, "Arriv�es"));
				directionMap.push_back(make_pair(DISPLAY_DEPARTURES, "D�parts"));

				vector<pair<EndFilter, string> > endFilterMap;
				endFilterMap.push_back(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
				endFilterMap.push_back(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

				vector<pair<int, string> > clearDelayMap;
				for (int i=-5; i<-1; ++i)
					clearDelayMap.push_back(make_pair(i, Conversion::ToString(-i) + " minutes avant le d�part"));
				clearDelayMap.push_back(make_pair(-1, "1 minute avant le d�part"));
				clearDelayMap.push_back(make_pair(0, "heure du d�part"));
				clearDelayMap.push_back(make_pair(1, "1 minute apr�s le d�part"));
				for (int i=2; i<6; ++i)
					clearDelayMap.push_back(make_pair(i, Conversion::ToString(i) + " minutes apr�s le d�part"));

				stream << "<h1>Actions</h1>";

				stream << "<p>";
				if (deleteRequest.isActionFunctionAuthorized())
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous s�r de vouloir supprimer l\\'afficheur " + _displayScreen->getFullName() + " ?", "monitor_delete.png") << " ";
				stream << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler", string(), "monitor_go.png") << " ";
				stream << "</p>";


				stream << "<h1>Propri�t�s</h1>";

				PropertiesHTMLTable t(updateDisplayRequest.getHTMLForm("updateprops"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.title("Emplacement");
				stream << t.cell("Lieu logique", _displayScreen->getLocalization()->getFullName());
				stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_NAME, _displayScreen->getLocalizationComment()));

				stream << t.title("Donn�es techniques");
				stream << t.cell("Type d'afficheur", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_TYPE, DeparturesTableModule::getDisplayTypeLabels(), _displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE));
				stream << t.cell("Code de branchement", t.getForm().getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()));
				stream << t.cell("UID", Conversion::ToString(_displayScreen->getKey()));

				stream << t.title("Apparence");
				stream << t.cell("Titre", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_TITLE, _displayScreen->getTitle()));
				stream << t.cell("Clignotement", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay()));
				stream << t.cell("Affichage num�ro de quai", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM, _displayScreen->getTrackNumberDisplay()));
				stream << t.cell("Affichage num�ro de service", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER, _displayScreen->getServiceNumberDisplay()));
				stream << t.cell("Affichage num�ro d'�quipe", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_TEAM, _displayScreen->getDisplayTeam()));

				stream << t.title("Contenu");
				stream << t.cell("Horaires", t.getForm().getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, directionMap, _displayScreen->getDirection()));
				stream << t.cell("S�lection", t.getForm().getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER, endFilterMap, _displayScreen->getEndFilter()));
				stream << t.cell("D�lai maximum d'affichage", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) + " minutes");
				stream << t.cell("D�lai d'effacement", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY, clearDelayMap, _displayScreen->getClearingDelay()));

				stream << t.close();

				stream << "<p class=\"info\">Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalit�s propos�es. Selon le type de l'afficheur, certains champs peuvent donc �tre sans effet sur l'affichage.</p>";

				// Used physical stops
				stream << "<h1>Arr�ts de desserte</h1>";

				HTMLForm uaf(updateAllDisplayRequest.getHTMLForm("updaall"));
				uaf.addHiddenField(UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE, Conversion::ToString(!_displayScreen->getAllPhysicalStopsDisplayed()));
				stream << "<p>Mode : "	<< (_displayScreen->getAllPhysicalStopsDisplayed() ? "Tous arr�ts (y compris nouveaux)" : "S�lection d'arr�ts");
				stream << " " << uaf.getLinkButton("Passer en mode " + string(_displayScreen->getAllPhysicalStopsDisplayed() ? "S�lection d'arr�ts" : "Tous arr�ts"));
				stream << "</p>";
				HTMLList l;

				if (!_displayScreen->getAllPhysicalStopsDisplayed())
				{
					bool withAddForm(_displayScreen->getPhysicalStops().size() != _displayScreen->getLocalization()->getPhysicalStops().size());
					HTMLForm ap(addPhysicalRequest.getHTMLForm("addphy"));

					// Opening
					if (withAddForm)
						stream << ap.open();
					stream << l.open();

					// Loop on linked stops
					for (PhysicalStops::const_iterator it = _displayScreen->getPhysicalStops().begin(); it != _displayScreen->getPhysicalStops().end(); ++it)
					{
						const PhysicalStop* ps(it->second);
						HTMLForm rs(rmPhysicalRequest.getHTMLForm("rm" + Conversion::ToString(ps->getKey())));
						rs.addHiddenField(DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL, Conversion::ToString(ps->getKey()));

						stream << l.element("broadcastpoint");
						stream << HTMLModule::getHTMLLink(rs.getURL(), HTMLModule::getHTMLImage("delete.png","Supprimer")) << ps->getOperatorCode() << " / " << ps->getName();
					}

					// Add a link
					if (withAddForm)
					{
						stream << l.element("broadcastpoint");
						stream << ap.getImageSubmitButton("add.png", "Ajouter");
						stream << ap.getSelectInput(AddDepartureStopToDisplayScreenAction::PARAMETER_STOP, _displayScreen->getLocalization()->getPhysicalStopLabels(_displayScreen->getPhysicalStops()) , uid(0));
					}

					// Closing
					stream << l.close();
					if (withAddForm)
						stream << ap.close();
				}

				// Intermediate stops to display
				stream << "<h1>Arr�ts interm�diaires � afficher</h1>";

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
				stream << amf.getSelectInput(DisplayScreenAddDisplayedPlace::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()), uid(0));

				stream << l.close();
				stream << amf.close();

				// Forbidden places
				stream << "<h1>Arr�ts ne devant pas �tre desservis par les lignes s�lectionn�es pour l'affichage</h1>";

				HTMLForm ant(addNSRequest.getHTMLForm("addforb"));
				stream << ant.open() << l.open();

				for (DisplayedPlacesList::const_iterator it = _displayScreen->getForbiddenPlaces().begin(); it != _displayScreen->getForbiddenPlaces().end(); ++it)
				{
					HTMLForm ntu(rmForbiddenRequest.getHTMLForm("rmfp"+ Conversion::ToString(it->second->getKey())));
					ntu.addHiddenField(DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));
					stream << l.element("broadcastpoint");
					stream << HTMLModule::getHTMLLink(ntu.getURL(), HTMLModule::getHTMLImage("delete.png","Supprimer")) << it->second->getFullName();
				}

				stream << l.element("broadcastpoint");
				stream << ant.getImageSubmitButton("add.png", "Ajouter");
				stream << ant.getSelectInput(AddForbiddenPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForbiddenPlaces()), uid(0));
				stream << l.close() << ant.close();

				// Preselection
				stream << "<h1>Pr�s�lection</h1>";

				PropertiesHTMLTable tt(updPreselRequest.getHTMLForm("updpresel"));
				tt.getForm().setUpdateRight(tabHasWritePermissions());

				stream << tt.open();
				stream << tt.title("Param�tres de pr�s�lection");
				stream << tt.cell("Activer", tt.getForm().getOuiNonRadioInput(UpdateDisplayPreselectionParametersAction::PARAMETER_ACTIVATE_PRESELECTION, _displayScreen->getGenerationMethod() != DisplayScreen::STANDARD_METHOD));
				stream << tt.cell("D�lai maximum pr�s�lection", tt.getForm().getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY, Conversion::ToString(_displayScreen->getForceDestinationDelay())));
				stream << tt.close();

				if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
				{
					// Additional preselection stops
					stream << "<p>Arr�ts de pr�s�lection</p>";

					HTMLForm psaf(addPreselRequest.getHTMLForm("addpresel"));
					stream << psaf.open() << l.open();

					for (DisplayedPlacesList::const_iterator it = _displayScreen->getForcedDestinations().begin(); it != _displayScreen->getForcedDestinations().end(); ++it)
					{
						HTMLForm psdf(rmPreselRequest.getHTMLForm("rmpres" + Conversion::ToString(it->second->getKey())));
						psdf.addHiddenField(RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));
						stream << l.element("broadcastpoint");
						stream << HTMLModule::getHTMLLink(psdf.getURL(), HTMLModule::getHTMLImage("delete.png","Supprimer")) << it->second->getFullName();
					}

					stream << l.element("broadcastpoint");
					stream << psaf.getImageSubmitButton("add.png", "Ajouter");
					stream << psaf.getSelectInput(AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForcedDestinations()), uid(0));
					stream << l.close() << psaf.close();

					stream << "<p class=\"info\">Les terminus de lignes sont automatiquement pr�s�lectionn�s.</p>";
				}
			}

			////////////////////////////////////////////////////////////////////
			// MAINTENANCE TAB
			if (openTabContent(stream, TAB_MAINTENANCE))
			{
				ActionFunctionRequest<UpdateDisplayMaintenanceAction,AdminRequest> updateRequest(_request);
				updateRequest.getFunction()->setPage<DisplayAdmin>();
				updateRequest.getAction()->setScreenId(_displayScreen->getKey());
				updateRequest.setObjectId(_displayScreen->getKey());

				FunctionRequest<AdminRequest> goToLogRequest(_request);
				goToLogRequest.getFunction()->setPage<DBLogViewer>();
				static_pointer_cast<DBLogViewer,AdminInterfaceElement>(goToLogRequest.getFunction()->getPage())->setLogKey(DisplayMaintenanceLog::FACTORY_KEY);
				goToLogRequest.setObjectId(_request->getObjectId());

				stream << "<h1>Param�tres de maintenance</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream << t.cell("Afficheur d�clar� en service", t.getForm().getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline()));
				stream << t.cell("Message de maintenance", t.getForm().getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 3, 30));
				stream << t.close();

				stream << "<h1>Informations de supervision</h1>";

				HTMLList l;
				stream << l.open();

				stream << l.element() << "Type d'afficheur : ";
				if (_displayScreen->getType() != NULL)
				{
					stream << _displayScreen->getType()->getName();
					stream << l.element() << "Dur�e th�orique entre les contacts de supervision : " << _displayScreen->getType()->getTimeBetweenChecks() << " min";
				}
				else
				{
					stream << "<p class=\"info\">Les terminus de lignes sont automatiquement pr�s�lectionn�s.</p>";

					stream << "ATTENTION : veuillez d�finir le type d'afficheur dans l'�cran de configuration.";
				}

				if (_env.getRegistry<DisplayMonitoringStatus>().empty())
				{
					stream << l.element() << "ATTENTION : Cet afficheur n'est jamais entr� en contact.";
				}
				else
				{
					shared_ptr<DisplayMonitoringStatus> status(_env.getEditableRegistry<DisplayMonitoringStatus>().front());
					if (_displayScreen->getIsOnline() && _now - status->getTime() > _displayScreen->getType()->getTimeBetweenChecks())
					{
						stream << l.element() << "ERREUR : Cet afficheur n'est plus en contact alors qu'il est d�clar� online.";
					}

					stream << l.element() << "Derni�re mesure le " << status->getTime().toString();
					stream << l.element() << "Dernier �tat mesur� : "
						<< DisplayMonitoringStatus::GetStatusString(status->getGlobalStatus());
					stream << l.element() << "Temp�rature : "
						<< status->getTemperatureValue();
					stream << l.element() << "D�tail anomalies : "
						<< status->getDetail();
				}

				stream << l.element("log") << HTMLModule::getHTMLLink(goToLogRequest.getURL(), "Acc�der au journal de maintenance de l'afficheur");

				stream << l.close();
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		void DisplayAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			
			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id, _env);
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
		}

		bool DisplayAdmin::isAuthorized() const
		{
			if (_request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return true;

			try
			{
				Env env;
				shared_ptr<const DisplayScreen> screen(DisplayScreenTableSync::Get(_request->getObjectId(), env, UP_LINKS_LOAD_LEVEL));
				return _request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(screen->getLocalization()->getKey()));
			}
			catch (...)
			{
				return false;
			}
		}

		DisplayAdmin::DisplayAdmin(
		): AdminInterfaceElementTemplate<DisplayAdmin>(),
			_now(TIME_CURRENT)
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
				_tabs.push_back(Tab("Technique", TAB_TECHNICAL, writeRight));
			}

			if (_request->isAuthorized<DisplayMaintenanceRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())))
			{
				bool writeRight(_request->isAuthorized<DisplayMaintenanceRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())));
				_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, writeRight));
			}

			_tabBuilded = true;
		}
	}
}
