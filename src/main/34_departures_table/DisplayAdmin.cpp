
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

#include <map>
#include <sstream>

#include "01_util/Html.h"
#include "01_util/Constants.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/EnvModule.h"

#include "32_admin/AdminParametersException.h"

#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/UpdateDisplayPreselectionParametersAction.h"
#include "34_departures_table/AddPreselectionPlaceToDisplayScreen.h"
#include "34_departures_table/RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "34_departures_table/UpdateAllStopsDisplayScreenAction.h"
#include "34_departures_table/AddDepartureStopToDisplayScreenAction.h"
#include "34_departures_table/AddForbiddenPlaceToDisplayScreen.h"
#include "34_departures_table/DisplayScreenAddDisplayedPlace.h"
#include "34_departures_table/DisplayScreenRemovePhysicalStopAction.h"
#include "34_departures_table/DisplayScreenRemoveDisplayedPlaceAction.h"
#include "34_departures_table/DisplayScreenRemoveForbiddenPlaceAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;
	using namespace env;

	namespace departurestable
	{
		const std::string DisplayAdmin::PARAMETER_PLACE = "pp";

		DisplayAdmin::DisplayAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _place(NULL)
		{}


		std::string DisplayAdmin::getTitle() const
		{
			return _displayScreen->getFullName();
		}

		void DisplayAdmin::display(std::ostream& stream, const AdminRequest* request /*= NULL*/ ) const
		{
			// Update request
			AdminRequest* updateDisplayRequest = Factory<Request>::create<AdminRequest>();
			updateDisplayRequest->copy(request);
			updateDisplayRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updateDisplayRequest->setAction(Factory<Action>::create<UpdateDisplayScreenAction>());
			updateDisplayRequest->setObjectId(request->getObjectId());

			// Update request
			AdminRequest* updateAllDisplayRequest = Factory<Request>::create<AdminRequest>();
			updateAllDisplayRequest->copy(request);
			updateAllDisplayRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updateAllDisplayRequest->setAction(Factory<Action>::create<UpdateAllStopsDisplayScreenAction>());
			updateAllDisplayRequest->setObjectId(request->getObjectId());

			// Add physical request
			AdminRequest* addPhysicalRequest = Factory<Request>::create<AdminRequest>();
			addPhysicalRequest->copy(request);
			addPhysicalRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addPhysicalRequest->setAction(Factory<Action>::create<AddDepartureStopToDisplayScreenAction>());
			addPhysicalRequest->setObjectId(request->getObjectId());

			// Add preselection request
			AdminRequest* addPreselRequest = Factory<Request>::create<AdminRequest>();
			addPreselRequest->copy(request);
			addPreselRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addPreselRequest->setAction(Factory<Action>::create<AddPreselectionPlaceToDisplayScreen>());
			addPreselRequest->setObjectId(request->getObjectId());

			// Add display request
			AdminRequest* addDisplayRequest = Factory<Request>::create<AdminRequest>();
			addDisplayRequest->copy(request);
			addDisplayRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addDisplayRequest->setAction(Factory<Action>::create<DisplayScreenAddDisplayedPlace>());
			addDisplayRequest->setObjectId(request->getObjectId());

			// Add not to serve request
			AdminRequest* addNSRequest = Factory<Request>::create<AdminRequest>();
			addNSRequest->copy(request);
			addNSRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addNSRequest->setAction(Factory<Action>::create<AddForbiddenPlaceToDisplayScreen>());
			addNSRequest->setObjectId(request->getObjectId());

			// Update preselection request
			AdminRequest* updPreselRequest = Factory<Request>::create<AdminRequest>();
			updPreselRequest->copy(request);
			updPreselRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updPreselRequest->setAction(Factory<Action>::create<UpdateDisplayPreselectionParametersAction>());
			updPreselRequest->setObjectId(request->getObjectId());

			// Remove preselection stop request
			AdminRequest* rmPreselRequest = Factory<Request>::create<AdminRequest>();
			rmPreselRequest->copy(request);
			rmPreselRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmPreselRequest->setAction(Factory<Action>::create<RemovePreselectionPlaceFromDisplayScreenAction>());
			rmPreselRequest->setObjectId(request->getObjectId());

			// Remove physical stop request
			AdminRequest* rmPhysicalRequest = Factory<Request>::create<AdminRequest>();
			rmPhysicalRequest->copy(request);
			rmPhysicalRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmPhysicalRequest->setAction(Factory<Action>::create<DisplayScreenRemovePhysicalStopAction>());
			rmPhysicalRequest->setObjectId(request->getObjectId());

			// Remove displayed place request
			AdminRequest* rmDisplayedRequest = Factory<Request>::create<AdminRequest>();
			rmDisplayedRequest->copy(request);
			rmDisplayedRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmDisplayedRequest->setAction(Factory<Action>::create<DisplayScreenRemoveDisplayedPlaceAction>());
			rmDisplayedRequest->setObjectId(request->getObjectId());

			// Remove Forbidden place request
			AdminRequest* rmForbiddenRequest = Factory<Request>::create<AdminRequest>();
			rmForbiddenRequest->copy(request);
			rmForbiddenRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmForbiddenRequest->setAction(Factory<Action>::create<DisplayScreenRemoveForbiddenPlaceAction>());
			rmForbiddenRequest->setObjectId(request->getObjectId());

			// Maps for particular select fields
			map<int, string> blinkingDelaysMap;
			blinkingDelaysMap.insert(make_pair(0, "Pas de clignotement"));
			blinkingDelaysMap.insert(make_pair(1, "1 minute avant disparition"));
			for (int i=2; i<6; ++i)
				blinkingDelaysMap.insert(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));

			map<DeparturesTableDirection, string> directionMap;
			directionMap.insert(make_pair(DISPLAY_ARRIVALS, "Arrivées"));
			directionMap.insert(make_pair(DISPLAY_DEPARTURES, "Départs"));

			map<EndFilter, string> endFilterMap;
			endFilterMap.insert(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
			endFilterMap.insert(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

			map<int, string> clearDelayMap;
			for (i=-5; i<-1; ++i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(-i) + " minutes avant le départ"));
			clearDelayMap.insert(make_pair(-1, "1 minute avant le départ"));
			clearDelayMap.insert(make_pair(0, "heure du départ"));
			clearDelayMap.insert(make_pair(1, "1 minute après le départ"));
			for (i=2; i<6; ++i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(i) + " minutes après le départ"));

			// Filling of the stream
			stream
				<< updateDisplayRequest->getHTMLFormHeader("update")
				<< "<h1>Emplacement</h1>"
				<< "<table><tr><td>Lieu logique</td>"
				<< "<td>" 
				<< Html::getSelectInput(PARAMETER_PLACE, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(), _place ? _place->getKey() : 0)
				<< "</td></tr>"
				<< "<tr><td>Lieu physique</td>"
				<< "<td>";
			
			if (_place == NULL)
				stream << "(Sélectionnez un lieu logique en premier)";
			else
				stream << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_ID, DeparturesTableModule::getBroadcastPointLabels(_place, false), _displayScreen->getLocalization() ? _displayScreen->getLocalization()->getKey() : 0);
			
			stream
				<< "</td></tr>"
				<< "<tr><td>Complément de précision</td>"
				<< "<td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_COMMENT, _displayScreen->getLocalizationComment()) << "</td></tr>"
				<< "</table>"

				// Technical data
				<< "<h1>Données techniques</h1>"
				<< updateDisplayRequest->getHTMLFormHeader("updateprops")
				<< "<table>"
				<< "<tr><td>Type d'afficheur</td>"
				<< "<td>" << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_TYPE, DeparturesTableModule::getDisplayTypeLabels(), _displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE) << "</td></tr>"
				<< "<tr><td>Code de branchement</td>"
				<< "<td>" << Html::getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()) << "</td></tr>"
				<< "<tr><td>UID</td><td>" << _displayScreen->getKey() << "</td></tr>"
				<< "</table>"

				// Appearance
				<< "<h1>Apparence</h1>"
				<< "<table><tr><td>Titre</td><td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_TITLE, _displayScreen->getTitle()) << "</td></tr>"
				<< "<tr><td>Clignotement</td><td>" << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay()) << "</td></tr>"
				<< "<tr><td>Affichage numéro de quai</td><td>" << Html::getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM, _displayScreen->getTrackNumberDisplay()) << "</td></tr>"
				<< "<tr><td>Affichage numéro de service</td><td>" << Html::getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER, _displayScreen->getServiceNumberDisplay()) << "</td></tr>"
				<< "</table>"

				// Content
				<< "<h1>Contenu</h1>"
				<< "<table>"
				<< "<tr><td>Horaires</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, directionMap, _displayScreen->getDirection()) << "</td></tr>"
				<< "<tr><td>Sélection</td><td>" << Html::getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER, endFilterMap, _displayScreen->getEndFilter()) << "</td></tr>"
				<< "<tr><td>Délai maximum d'affichage</td><td>" << Html::getTextInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) << " minutes</td></tr>"
				<< "<tr><td>Délai d'effacement</td><td>" << Html::getSelectInput(UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY, clearDelayMap, _displayScreen->getClearingDelay()) << "</td></tr>"
				<< "</table>"
				<< Html::getSubmitButton("Enregistrer les modifications des propriétés")
				<< "</form>";

			if (_displayScreen->getLocalization())
			{
				stream
					// Used physical stops
					<< "<h1>Arrêts de desserte</h1>"
					<< "<table>"
					<< "<tr><td>Mode : "
					<< (_displayScreen->getAllPhysicalStopsDisplayed() ? "Tous arrêts (y compris nouveaux)" : "Sélection d'arrêts")
					<< "</td><td>"
					<< updateAllDisplayRequest->getHTMLFormHeader("updaall")
					<< Html::getHiddenInput(UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE, Conversion::ToString(!_displayScreen->getAllPhysicalStopsDisplayed()))
					<< Html::getSubmitButton("Passer en mode " + string(_displayScreen->getAllPhysicalStopsDisplayed() ? "Sélection d'arrêts" : "Tous arrêts"))
					<< "</form></td></tr>";

				if (!_displayScreen->getAllPhysicalStopsDisplayed())
				{
					stream << "<tr><th>Arrêt</th><th>Action</th></tr>";
					for (PhysicalStopsSet::const_iterator it = _displayScreen->getPhysicalStops().begin(); it != _displayScreen->getPhysicalStops().end(); ++it)
					{
						const PhysicalStop* ps = *it;
						stream
							<< "<tr>"
							<< "<td>" << ps->getName() << "</td>"
							<< "<td>"
							<< rmPhysicalRequest->getHTMLFormHeader("rm" + Conversion::ToString(ps->getKey()))
							<< Html::getHiddenInput(DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL, Conversion::ToString(ps->getKey()))
							<< Html::getSubmitButton("Supprimer")
							<< "</form></td></tr>";
					}
					if (_displayScreen->getPhysicalStops().size() != _displayScreen->getLocalization()->getConnectionPlace()->getPhysicalStops().size())
					{
						stream
							<< addPhysicalRequest->getHTMLFormHeader("addphy")
							<< "<tr><td>"
							<< Html::getSelectInput(AddDepartureStopToDisplayScreenAction::PARAMETER_STOP, _displayScreen->getLocalization()->getConnectionPlace()->getPhysicalStopLabels(_displayScreen->getPhysicalStops()) , uid(0))
							<< "</td><td>"
							<< Html::getSubmitButton("Ajouter")
							<< "</td></tr></form>";	
					}									
				}
				stream << "</table>"

					// Intermediate stops to display
					<< "<h1>Arrêts intermédiaires à afficher</h1>"
					<< "<table>"
					<< "<tr><th>Arrêt</th><th>Action</th></tr>";

				for (DisplayedPlacesList::const_iterator it = _displayScreen->getDisplayedPlaces().begin(); it != _displayScreen->getDisplayedPlaces().end(); ++it)
				{
					stream
						<< "<tr><td>" << (*it)->getFullName() << "</td>"
						<< "<td>"
						<< rmDisplayedRequest->getHTMLFormHeader("rmdp" + Conversion::ToString((*it)->getKey()))
						<< Html::getHiddenInput(DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()))
						<< Html::getSubmitButton("Supprimer")
						<< "</form></td></tr>";
				}

				stream
					<< addDisplayRequest->getHTMLFormHeader("adddispl")
					<< "<tr><td>" << Html::getSortedSelectInput(DisplayScreenAddDisplayedPlace::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()), uid(0))
					<< "</td><td>" << Html::getSubmitButton("Ajouter") << "</td></tr></form>"
					<< "</table>"
			
					<< "<h1>Arrêts ne devant pas être desservis par les lignes sélectionnées pour l'affichage</h1>"
					<< "<table>"
					<< "<tr><th>Arrêt</th><th>Action</th></tr>";

				for (DisplayedPlacesList::const_iterator it = _displayScreen->getForbiddenPlaces().begin(); it != _displayScreen->getForbiddenPlaces().end(); ++it)
				{
					stream
						<< "<tr><td>" << (*it)->getFullName() << "</td><td>"
						<< rmForbiddenRequest->getHTMLFormHeader("rmfp"+ Conversion::ToString((*it)->getKey()))
						<< Html::getHiddenInput(DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()))
						<< Html::getSubmitButton("Supprimer")
						<< "</form></td></tr>";
				}
				stream
					<< addNSRequest->getHTMLFormHeader("addforb")
					<< "<tr><td>" << Html::getSortedSelectInput(AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForbiddenPlaces()), uid(0))
					<< "</td><td>" << Html::getSubmitButton("Ajouter") << "</td></tr></form>"
					<< "</table>"


					// Preselection
					<< "<h1>Présélection</h1>"

					<< updPreselRequest->getHTMLFormHeader("updpresel")
					<< "<table>"
					<< "<tr><td>Activer</td>"
					<< "<td>" << Html::getOuiNonRadioInput(UpdateDisplayPreselectionParametersAction::PARAMETER_ACTIVATE_PRESELECTION, _displayScreen->getGenerationMethod() == DisplayScreen::STANDARD_METHOD) << "</td></tr>"
					<< "<tr><td>Délai maximum présélection</td><td>" << Html::getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY, Conversion::ToString(_displayScreen->getForceDestinationDelay())) << "</td></tr>"
					<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregister les paramètres de présélection") << "</td></tr>"
					<< "</table></form>"

					// Additional preselection stops
					<< "<h1>Arrêts de présélection (les terminus de lignes sont automatiquement présélectionnés)</h1>"
					
					<< "<table>"
					<< "<tr><th>Arrêt</th><th>Action</th></tr>";
				
				for (DisplayedPlacesList::const_iterator it = _displayScreen->getForcedDestinations().begin(); it != _displayScreen->getForcedDestinations().end(); ++it)
				{
					stream
						<< "<tr><td>" << (*it)->getFullName() << "</td><td>" 
						<< rmPreselRequest->getHTMLFormHeader("rmpres" + Conversion::ToString((*it)->getKey()))
						<< Html::getHiddenInput(RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()))
						<< Html::getSubmitButton("Supprimer") 
						<< "</form></td></tr>";
				}

				stream
					<< addPreselRequest->getHTMLFormHeader("addpresel")
					<< "<tr><td>" << Html::getSortedSelectInput(AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForcedDestinations()), uid(0))
					<< "</td><td>" << Html::getSubmitButton("Ajouter") << "</td></tr></form>"
					<< "</table>";
			}

			stream
				<< "<p>NB : Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalités proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans effet sur l'affichage.</p>";

			// Cleaning
			delete updateDisplayRequest;
			delete addPreselRequest;
			delete addNSRequest;
			delete updPreselRequest;
			delete updateAllDisplayRequest;
			delete rmPreselRequest;
			delete addDisplayRequest;
			delete rmPhysicalRequest;
		}

		void DisplayAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw AdminParametersException("Display screen not specified");
			
			if (Conversion::ToLongLong(it->second) == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_displayScreen = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(it->second));
				if (_displayScreen->getLocalization())
					_place = _displayScreen->getLocalization()->getConnectionPlace();
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw AdminParametersException("Display screen not found");
			}

			if (_place == NULL)
			{
				it = map.find(PARAMETER_PLACE);
				if (it != map.end())
				{
					try
					{
						_place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(it->second));
					}
					catch (ConnectionPlace::RegistryKeyException e)
					{
						throw AdminParametersException("Place not found");
					}
				}
			}
		}
	}
}
