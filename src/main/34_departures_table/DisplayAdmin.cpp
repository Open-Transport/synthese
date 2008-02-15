
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

#include "01_util/Constants.h"

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"
#include "05_html/PropertiesHTMLTable.h"

#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/PhysicalStop.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "34_departures_table/DisplayAdmin.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"
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
#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/DisplayScreenRemove.h"
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/ArrivalDepartureTableRight.h"

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
		void DisplayAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Rights
			bool writeRight(request->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_displayScreen->getLocalization()->getKey())));

			// Update request
			ActionFunctionRequest<UpdateDisplayScreenAction,AdminRequest> updateDisplayRequest(request);
			updateDisplayRequest.getFunction()->setPage<DisplayAdmin>();
			updateDisplayRequest.setObjectId(request->getObjectId());

			// Update request
			ActionFunctionRequest<UpdateAllStopsDisplayScreenAction,AdminRequest> updateAllDisplayRequest(request);
			updateAllDisplayRequest.getFunction()->setPage<DisplayAdmin>();
			updateAllDisplayRequest.setObjectId(request->getObjectId());

			// Add physical request
			ActionFunctionRequest<AddDepartureStopToDisplayScreenAction,AdminRequest> addPhysicalRequest(request);
			addPhysicalRequest.getFunction()->setPage<DisplayAdmin>();
			addPhysicalRequest.setObjectId(request->getObjectId());

			// Add preselection request
			ActionFunctionRequest<AddPreselectionPlaceToDisplayScreen,AdminRequest> addPreselRequest(request);
			addPreselRequest.getFunction()->setPage<DisplayAdmin>();
			addPreselRequest.setObjectId(request->getObjectId());

			// Add display request
			ActionFunctionRequest<DisplayScreenAddDisplayedPlace,AdminRequest> addDisplayRequest(request);
			addDisplayRequest.getFunction()->setPage<DisplayAdmin>();
			addDisplayRequest.setObjectId(request->getObjectId());

			// Add not to serve request
			ActionFunctionRequest<AddForbiddenPlaceToDisplayScreen,AdminRequest> addNSRequest(request);
			addNSRequest.getFunction()->setPage<DisplayAdmin>();
			addNSRequest.setObjectId(request->getObjectId());

			// Update preselection request
			ActionFunctionRequest<UpdateDisplayPreselectionParametersAction,AdminRequest> updPreselRequest(request);
			updPreselRequest.getFunction()->setPage<DisplayAdmin>();
			updPreselRequest.setObjectId(request->getObjectId());

			// Remove preselection stop request
			ActionFunctionRequest<RemovePreselectionPlaceFromDisplayScreenAction,AdminRequest> rmPreselRequest(request);
			rmPreselRequest.getFunction()->setPage<DisplayAdmin>();
			rmPreselRequest.setObjectId(request->getObjectId());

			// Remove physical stop request
			ActionFunctionRequest<DisplayScreenRemovePhysicalStopAction,AdminRequest> rmPhysicalRequest(request);
			rmPhysicalRequest.getFunction()->setPage<DisplayAdmin>();
			rmPhysicalRequest.setObjectId(request->getObjectId());

			// Remove displayed place request
			ActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,AdminRequest> rmDisplayedRequest(request);
			rmDisplayedRequest.getFunction()->setPage<DisplayAdmin>();
			rmDisplayedRequest.setObjectId(request->getObjectId());

			// Remove Forbidden place request
			ActionFunctionRequest<DisplayScreenRemoveForbiddenPlaceAction,AdminRequest> rmForbiddenRequest(request);
			rmForbiddenRequest.getFunction()->setPage<DisplayAdmin>();
			rmForbiddenRequest.setObjectId(request->getObjectId());

			// Delete the screen request
			ActionFunctionRequest<DisplayScreenRemove, AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage<DisplaySearchAdmin>();
			deleteRequest.getAction()->setDisplayScreen(_displayScreen);

			FunctionRequest<DisplayScreenContentRequest> viewRequest(request);
			viewRequest.setObjectId(_displayScreen->getKey());

			FunctionRequest<AdminRequest> maintRequest(request);
			maintRequest.getFunction()->setPage<DisplayMaintenanceAdmin>();
			maintRequest.setObjectId(_displayScreen->getKey());

			// Maps for particular select fields
			vector<pair<int, string> > blinkingDelaysMap;
			blinkingDelaysMap.push_back(make_pair(0, "Pas de clignotement"));
			blinkingDelaysMap.push_back(make_pair(1, "1 minute avant disparition"));
			for (int i=2; i<6; ++i)
				blinkingDelaysMap.push_back(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));

			vector<pair<DeparturesTableDirection, string> > directionMap;
			directionMap.push_back(make_pair(DISPLAY_ARRIVALS, "Arrivées"));
			directionMap.push_back(make_pair(DISPLAY_DEPARTURES, "Départs"));

			vector<pair<EndFilter, string> > endFilterMap;
			endFilterMap.push_back(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
			endFilterMap.push_back(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

			vector<pair<int, string> > clearDelayMap;
			for (int i=-5; i<-1; ++i)
				clearDelayMap.push_back(make_pair(i, Conversion::ToString(-i) + " minutes avant le départ"));
			clearDelayMap.push_back(make_pair(-1, "1 minute avant le départ"));
			clearDelayMap.push_back(make_pair(0, "heure du départ"));
			clearDelayMap.push_back(make_pair(1, "1 minute après le départ"));
			for (int i=2; i<6; ++i)
				clearDelayMap.push_back(make_pair(i, Conversion::ToString(i) + " minutes après le départ"));

			stream << "<h1>Actions</h1>";

			if (deleteRequest.isActionFunctionAuthorized())
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l\\'afficheur " + _displayScreen->getFullName() + " ?", "monitor_delete.png") << " ";
			stream << HTMLModule::getLinkButton(viewRequest.getURL(), "Simuler", string(), "monitor_go.png") << " ";
			stream << HTMLModule::getLinkButton(maintRequest.getURL(), "Supervision", string(), "monitor_lightning.png") << " ";


			stream << "<h1>Propriétés</h1>";

			PropertiesHTMLTable t(updateDisplayRequest.getHTMLForm("updateprops"));
			t.getForm().setUpdateRight(writeRight);

			stream << t.open();
			stream << t.title("Emplacement");
			stream << t.cell("Lieu logique", _displayScreen->getLocalization()->getFullName());
			stream << t.cell("Nom", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_NAME, _displayScreen->getLocalizationComment()));
			
			stream << t.title("Données techniques");
			stream << t.cell("Type d'afficheur", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_TYPE, DeparturesTableModule::getDisplayTypeLabels(), _displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE));
			stream << t.cell("Code de branchement", t.getForm().getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode()));
			stream << t.cell("UID", Conversion::ToString(_displayScreen->getKey()));

			stream << t.title("Apparence");
			stream << t.cell("Titre", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_TITLE, _displayScreen->getTitle()));
			stream << t.cell("Clignotement", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay()));
			stream << t.cell("Affichage numéro de quai", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM, _displayScreen->getTrackNumberDisplay()));
			stream << t.cell("Affichage numéro de service", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER, _displayScreen->getServiceNumberDisplay()));
			stream << t.cell("Affichage numéro d'équipe", t.getForm().getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_TEAM, _displayScreen->getDisplayTeam()));

			stream << t.title("Contenu");
			stream << t.cell("Horaires", t.getForm().getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, directionMap, _displayScreen->getDirection()));
			stream << t.cell("Sélection", t.getForm().getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER, endFilterMap, _displayScreen->getEndFilter()));
			stream << t.cell("Délai maximum d'affichage", t.getForm().getTextInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) + " minutes");
			stream << t.cell("Délai d'effacement", t.getForm().getSelectInput(UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY, clearDelayMap, _displayScreen->getClearingDelay()));

			stream << t.close();

			// Used physical stops
			stream << "<h1>Arrêts de desserte</h1>";
			
			HTMLTable st;
			stream << st.open();
			
			HTMLForm uaf(updateAllDisplayRequest.getHTMLForm("updaall"));
			uaf.addHiddenField(UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE, Conversion::ToString(!_displayScreen->getAllPhysicalStopsDisplayed()));
			stream << st.row();
			stream << st.col() << "Mode : "	<< (_displayScreen->getAllPhysicalStopsDisplayed() ? "Tous arrêts (y compris nouveaux)" : "Sélection d'arrêts");
			stream << st.col() << uaf.getLinkButton("Passer en mode " + string(_displayScreen->getAllPhysicalStopsDisplayed() ? "Sélection d'arrêts" : "Tous arrêts"));
			
			if (!_displayScreen->getAllPhysicalStopsDisplayed())
			{
				stream << st.row();
				stream << st.col() << "Arrêt";
				stream << st.col() << "Action";

				for (PhysicalStops::const_iterator it = _displayScreen->getPhysicalStops().begin(); it != _displayScreen->getPhysicalStops().end(); ++it)
				{
					const PhysicalStop* ps(it->second);
					HTMLForm rs(rmPhysicalRequest.getHTMLForm("rm" + Conversion::ToString(ps->getKey())));
					rs.addHiddenField(DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL, Conversion::ToString(ps->getKey()));

					stream << st.row();
					stream << st.col() << ps->getOperatorCode() << " / " << ps->getName();
					stream << st.col() << rs.getLinkButton("Supprimer");
				}
				if (_displayScreen->getPhysicalStops().size() != _displayScreen->getLocalization()->getPhysicalStops().size())
				{
					HTMLForm ap(addPhysicalRequest.getHTMLForm("addphy"));
					stream << st.row();
					stream << st.col(2) << ap.open();
					stream << ap.getSelectInput(AddDepartureStopToDisplayScreenAction::PARAMETER_STOP, _displayScreen->getLocalization()->getPhysicalStopLabels(_displayScreen->getPhysicalStops()) , uid(0));
					stream << ap.getSubmitButton("Ajouter");
					stream << ap.close();
				}
			}
			stream << st.close();

			// Intermediate stops to display
			stream << "<h1>Arrêts intermédiaires à afficher</h1>";

			HTMLTable::ColsVector mtv;
			mtv.push_back("Arrêt");
			mtv.push_back("Action");
			HTMLTable mt(mtv);;
			stream << mt.open();
			
			for (DisplayedPlacesList::const_iterator it(_displayScreen->getDisplayedPlaces().begin()); it != _displayScreen->getDisplayedPlaces().end(); ++it)
			{
				HTMLForm mf(rmDisplayedRequest.getHTMLForm("rmdp" + Conversion::ToString(it->second->getKey())));
				mf.addHiddenField(DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));

				stream << mt.row();
				stream << mt.col() << it->second->getFullName();
				stream << mt.col() << mf.getLinkButton("Supprimer");
			}

			HTMLForm amf(addDisplayRequest.getHTMLForm("adddispl"));
			stream << mt.row();
			stream << mt.col(2) << amf.open();
			stream << amf.getSelectInput(DisplayScreenAddDisplayedPlace::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()), uid(0));
			stream << amf.getSubmitButton("Ajouter");
			stream << amf.close();

			stream << mt.close();
		
			// Forbidden places
			stream << "<h1>Arrêts ne devant pas être desservis par les lignes sélectionnées pour l'affichage</h1>";
			HTMLTable::ColsVector ntv;
			ntv.push_back("Arrêt");
			ntv.push_back("Action");
			HTMLTable nt(ntv);
			stream << nt.open();

			for (DisplayedPlacesList::const_iterator it = _displayScreen->getForbiddenPlaces().begin(); it != _displayScreen->getForbiddenPlaces().end(); ++it)
			{
				HTMLForm ntu(rmForbiddenRequest.getHTMLForm("rmfp"+ Conversion::ToString(it->second->getKey())));
				ntu.addHiddenField(DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));
				stream << nt.row();
				stream << nt.col() << it->second->getFullName();
				stream << nt.col() << ntu.getLinkButton("Supprimer");
			}
			HTMLForm ant(addNSRequest.getHTMLForm("addforb"));
			stream << nt.row();
			stream << nt.col(2) << ant.open();
			stream << ant.getSelectInput(AddForbiddenPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForbiddenPlaces()), uid(0));
			stream << ant.getSubmitButton("Ajouter");
			stream << ant.close();
			
			stream << nt.close();
			
			// Preselection
			stream << "<h1>Présélection</h1>";

			HTMLForm tf(updPreselRequest.getHTMLForm("updpresel"));
			HTMLTable tt;
			stream << tf.open() << tt.open();
			
			stream << tt.row();
			stream << tt.col() << "Activer";
			stream << tt.col() << tf.getOuiNonRadioInput(UpdateDisplayPreselectionParametersAction::PARAMETER_ACTIVATE_PRESELECTION, _displayScreen->getGenerationMethod() != DisplayScreen::STANDARD_METHOD);
			
			stream << tt.row();
			stream << tt.col() << "Délai maximum présélection";
			stream << tt.col() << tf.getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY, Conversion::ToString(_displayScreen->getForceDestinationDelay()));

			stream << tt.row();
			stream << tt.col(2) << tf.getSubmitButton("Enregister les paramètres de présélection");

			stream << tt.close() << tf.close();

			if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
			{
				// Additional preselection stops
				stream << "<h1>Arrêts de présélection (les terminus de lignes sont automatiquement présélectionnés)</h1>";

				HTMLTable::ColsVector pstv;
				pstv.push_back("Arrêt");
				pstv.push_back("Action");
				HTMLTable pst(pstv);
				stream << pst.open();

				for (DisplayedPlacesList::const_iterator it = _displayScreen->getForcedDestinations().begin(); it != _displayScreen->getForcedDestinations().end(); ++it)
				{
					HTMLForm psdf(rmPreselRequest.getHTMLForm("rmpres" + Conversion::ToString(it->second->getKey())));
					psdf.addHiddenField(RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE, Conversion::ToString(it->second->getKey()));
					stream << pst.row();
					stream << pst.col() << it->second->getFullName();
					stream << pst.col() << psdf.getLinkButton("Supprimer");
				}

				HTMLForm psaf(addPreselRequest.getHTMLForm("addpresel"));
				stream << pst.row();
				stream << pst.col(2) << psaf.open();
				stream << psaf.getSelectInput(AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForcedDestinations()), uid(0));
				stream << psaf.getSubmitButton("Ajouter");
				stream << psaf.close();

				stream << pst.close();
			}

			stream
				<< "<p>NB : Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalités proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans effet sur l'affichage.</p>";
		}

		void DisplayAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			
			if (id == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return;

			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id, GET_AUTO, true);
				_pageLink.name = _displayScreen->getFullName();
				_pageLink.parameterName = QueryString::PARAMETER_OBJECT_ID;
				_pageLink.parameterValue = Conversion::ToString(id);

			}
			catch (DisplayScreen::ObjectNotFoundException& e)
			{
				throw AdminParametersException("Display screen not found");
			}
			catch (PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
			{
				throw AdminParametersException("Place not found");
			}
		}

		bool DisplayAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			if (request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				return true;

			try
			{
				shared_ptr<const DisplayScreen> screen(DisplayScreenTableSync::Get(request->getObjectId(), GET_AUTO, true));
				return request->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(screen->getLocalization()->getKey()));
			}
			catch (...)
			{
				return false;
			}
		}

		DisplayAdmin::DisplayAdmin()
			: AdminInterfaceElementTemplate<DisplayAdmin>()
		{

		}

		AdminInterfaceElement::PageLinks DisplayAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if (parentLink.factoryKey == DisplaySearchAdmin::FACTORY_KEY)
			{
				if (currentPage.getFactoryKey() == FACTORY_KEY)
				{
					links.push_back(currentPage.getPageLink());
				}

				if (currentPage.getFactoryKey() == DisplayMaintenanceAdmin::FACTORY_KEY)
				{
					const DisplayMaintenanceAdmin& currentSPage(static_cast<const DisplayMaintenanceAdmin&>(currentPage));
					shared_ptr<const DisplayScreen> screen(currentSPage.getDisplayScreen());

					AdminInterfaceElement::PageLink link(_pageLink);
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(screen->getKey());
					link.name = screen->getFullName();
					links.push_back(link);
				}
			}

			return links;
		}

		
		AdminInterfaceElement::PageLinks DisplayAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			const DisplayMaintenanceAdmin& currentSPage(static_cast<const DisplayMaintenanceAdmin&>(currentPage));
			shared_ptr<const DisplayScreen> screen(currentSPage.getDisplayScreen());

			AdminInterfaceElement::PageLinks links;
			AdminInterfaceElement::PageLink link;
			link.factoryKey = DisplayMaintenanceAdmin::FACTORY_KEY;
			link.icon = DisplayMaintenanceAdmin::ICON;
			link.name = DisplayMaintenanceAdmin::DEFAULT_TITLE;
			link.parameterName = QueryString::PARAMETER_OBJECT_ID;
			link.parameterValue = Conversion::ToString(screen->getKey());
			links.push_back(link);
			return links;
		}
	}
}
