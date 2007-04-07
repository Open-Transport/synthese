
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

#include "01_util/Constants.h"

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/EnvModule.h"

#include "30_server/ActionFunctionRequest.h"

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
	using namespace html;

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

		void DisplayAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request /*= NULL*/ ) const
		{
			// Update request
			ActionFunctionRequest<UpdateDisplayScreenAction,AdminRequest> updateDisplayRequest(request);
			updateDisplayRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updateDisplayRequest.setObjectId(request->getObjectId());

			// Update request
			ActionFunctionRequest<UpdateAllStopsDisplayScreenAction,AdminRequest> updateAllDisplayRequest(request);
			updateAllDisplayRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updateAllDisplayRequest.setObjectId(request->getObjectId());

			// Add physical request
			ActionFunctionRequest<AddDepartureStopToDisplayScreenAction,AdminRequest> addPhysicalRequest(request);
			addPhysicalRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addPhysicalRequest.setObjectId(request->getObjectId());

			// Add preselection request
			ActionFunctionRequest<AddPreselectionPlaceToDisplayScreen,AdminRequest> addPreselRequest(request);
			addPreselRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addPreselRequest.setObjectId(request->getObjectId());

			// Add display request
			ActionFunctionRequest<DisplayScreenAddDisplayedPlace,AdminRequest> addDisplayRequest(request);
			addDisplayRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addDisplayRequest.setObjectId(request->getObjectId());

			// Add not to serve request
			ActionFunctionRequest<AddForbiddenPlaceToDisplayScreen,AdminRequest> addNSRequest(request);
			addNSRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			addNSRequest.setObjectId(request->getObjectId());

			// Update preselection request
			ActionFunctionRequest<UpdateDisplayPreselectionParametersAction,AdminRequest> updPreselRequest(request);
			updPreselRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			updPreselRequest.setObjectId(request->getObjectId());

			// Remove preselection stop request
			ActionFunctionRequest<RemovePreselectionPlaceFromDisplayScreenAction,AdminRequest> rmPreselRequest(request);
			rmPreselRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmPreselRequest.setObjectId(request->getObjectId());

			// Remove physical stop request
			ActionFunctionRequest<DisplayScreenRemovePhysicalStopAction,AdminRequest> rmPhysicalRequest(request);
			rmPhysicalRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmPhysicalRequest.setObjectId(request->getObjectId());

			// Remove displayed place request
			ActionFunctionRequest<DisplayScreenRemoveDisplayedPlaceAction,AdminRequest> rmDisplayedRequest(request);
			rmDisplayedRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmDisplayedRequest.setObjectId(request->getObjectId());

			// Remove Forbidden place request
			ActionFunctionRequest<DisplayScreenRemoveForbiddenPlaceAction,AdminRequest> rmForbiddenRequest(request);
			rmForbiddenRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayAdmin>());
			rmForbiddenRequest.setObjectId(request->getObjectId());

			// Maps for particular select fields
			map<int, string> blinkingDelaysMap;
			blinkingDelaysMap.insert(make_pair(0, "Pas de clignotement"));
			blinkingDelaysMap.insert(make_pair(1, "1 minute avant disparition"));
			for (int i=2; i<6; ++i)
				blinkingDelaysMap.insert(make_pair(i, Conversion::ToString(i) + " minutes avant disparition"));

			map<DeparturesTableDirection, string> directionMap;
			directionMap.insert(make_pair(DISPLAY_ARRIVALS, "Arriv�es"));
			directionMap.insert(make_pair(DISPLAY_DEPARTURES, "D�parts"));

			map<EndFilter, string> endFilterMap;
			endFilterMap.insert(make_pair(ENDS_ONLY, "Origines/Terminus seulement"));
			endFilterMap.insert(make_pair(WITH_PASSING, "Origines/Terminus et passages"));

			map<int, string> clearDelayMap;
			for (int i=-5; i<-1; ++i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(-i) + " minutes avant le d�part"));
			clearDelayMap.insert(make_pair(-1, "1 minute avant le d�part"));
			clearDelayMap.insert(make_pair(0, "heure du d�part"));
			clearDelayMap.insert(make_pair(1, "1 minute apr�s le d�part"));
			for (int i=2; i<6; ++i)
				clearDelayMap.insert(make_pair(i, Conversion::ToString(i) + " minutes apr�s le d�part"));

			// Filling of the stream
			stream << "<h1>Emplacement</h1>";

			HTMLForm uf(updateDisplayRequest.getHTMLForm("update"));
			HTMLTable t;
			
			stream << uf.open() << t.open();
			stream << t.row();
			stream << t.col() << "Lieu logique";
			stream << t.col() << uf.getSelectInput(PARAMETER_PLACE, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(), _place ? _place->getKey() : 0);

			stream << t.row();
			stream << t.col() << "Lieu physique";
			stream << t.col();
			if (_place == NULL)
				stream << "(S�lectionnez un lieu logique en premier)";
			else
				stream << uf.getSelectInput(UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_ID, DeparturesTableModule::getBroadcastPointLabels(_place, false), _displayScreen->getLocalization() ? _displayScreen->getLocalization()->getKey() : 0);
			
			stream << t.row();
			stream << t.col() << "Compl�ment de pr�cision";
			stream << t.col() << uf.getTextInput(UpdateDisplayScreenAction::PARAMETER_LOCALIZATION_COMMENT, _displayScreen->getLocalizationComment());

			stream << t.row();
			stream << t.col(2) << uf.getSubmitButton("Enregistrer");

			stream << t.close() << uf.close();

			// Technical data
			stream << "<h1>Donn�es techniques</h1>";

			HTMLForm pf(updateDisplayRequest.getHTMLForm("updateprops"));
			HTMLTable pt;

			stream << pf.open() << pt.open();

			stream << pt.row();
			stream << pt.col() << "Type d'afficheur";
			stream << pt.col() << pf.getSelectInput(UpdateDisplayScreenAction::PARAMETER_TYPE, DeparturesTableModule::getDisplayTypeLabels(), _displayScreen->getType() ? _displayScreen->getType()->getKey() : UNKNOWN_VALUE);

			stream << pt.row();
			stream << pt.col() << "Code de branchement";
			stream << pt.col() << pf.getSelectNumberInput(UpdateDisplayScreenAction::PARAMETER_WIRING_CODE, 0, 99, _displayScreen->getWiringCode());
			
			stream << pt.row();
			stream << pt.col() << "UID";
			stream << pt.col() << _displayScreen->getKey();

			stream << pt.close();

			// Appearance
			stream << "<h1>Apparence</h1>";

			HTMLTable dt;
			stream << dt.open();

			stream << dt.row();
			stream << dt.col() << "Titre";
			stream << dt.col() << pf.getTextInput(UpdateDisplayScreenAction::PARAMETER_TITLE, _displayScreen->getTitle());

			stream << dt.row();
			stream << dt.col() << "Clignotement";
			stream << dt.col() << pf.getSelectInput(UpdateDisplayScreenAction::PARAMETER_BLINKING_DELAY, blinkingDelaysMap, _displayScreen->getBlinkingDelay());

			stream << dt.row();
			stream << dt.col() << "Affichage num�ro de quai";
			stream << dt.col() << pf.getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_PLATFORM, _displayScreen->getTrackNumberDisplay());
			
			stream << dt.row();
			stream << dt.col() << "Affichage num�ro de service";
			stream << dt.col() << pf.getOuiNonRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_SERVICE_NUMBER, _displayScreen->getServiceNumberDisplay());

			stream << dt.close();

			// Content
			stream << "<h1>Contenu</h1>";

			HTMLTable ct;
			stream << ct.open();

			stream << ct.row();
			stream << ct.col() << "Horaires";
			stream << ct.col() << pf.getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_DEPARTURE_ARRIVAL, directionMap, _displayScreen->getDirection());

			stream << ct.row();
			stream << ct.col() << "S�lection";
			stream << ct.col() << pf.getRadioInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_END_FILTER, endFilterMap, _displayScreen->getEndFilter());

			stream << ct.row();
			stream << ct.col() << "D�lai maximum d'affichage";
			stream << ct.col() << pf.getTextInput(UpdateDisplayScreenAction::PARAMETER_DISPLAY_MAX_DELAY, Conversion::ToString(_displayScreen->getMaxDelay())) << " minutes";

			stream << ct.row();
			stream << ct.col() << "D�lai d'effacement";
			stream << ct.col() << pf.getSelectInput(UpdateDisplayScreenAction::PARAMETER_CLEANING_DELAY, clearDelayMap, _displayScreen->getClearingDelay());

			stream << ct.close();

			stream << pf.getSubmitButton("Enregistrer les modifications des propri�t�s");
			stream << pf.close();

			if (_displayScreen->getLocalization())
			{
				// Used physical stops
				stream << "<h1>Arr�ts de desserte</h1>";
				
				HTMLTable st;
				stream << st.open();
				
				HTMLForm uaf(updateAllDisplayRequest.getHTMLForm("updaall"));
				uaf.addHiddenField(UpdateAllStopsDisplayScreenAction::PARAMETER_VALUE, Conversion::ToString(!_displayScreen->getAllPhysicalStopsDisplayed()));
				stream << st.row();
				stream << st.col() << "Mode : "	<< (_displayScreen->getAllPhysicalStopsDisplayed() ? "Tous arr�ts (y compris nouveaux)" : "S�lection d'arr�ts");
				stream << st.col() << uaf.getLinkButton("Passer en mode " + string(_displayScreen->getAllPhysicalStopsDisplayed() ? "S�lection d'arr�ts" : "Tous arr�ts"));
				
				if (!_displayScreen->getAllPhysicalStopsDisplayed())
				{
					stream << st.row();
					stream << st.col() << "Arr�t";
					stream << st.col() << "Action";

					for (PhysicalStopsSet::const_iterator it = _displayScreen->getPhysicalStops().begin(); it != _displayScreen->getPhysicalStops().end(); ++it)
					{
						const PhysicalStop* ps = *it;
						HTMLForm rs(rmPhysicalRequest.getHTMLForm("rm" + Conversion::ToString(ps->getKey())));
						rs.addHiddenField(DisplayScreenRemovePhysicalStopAction::PARAMETER_PHYSICAL, Conversion::ToString(ps->getKey()));

						stream << st.row();
						stream << st.col() << ps->getName();
						stream << st.col() << rs.getLinkButton("Supprimer");
					}
					if (_displayScreen->getPhysicalStops().size() != _displayScreen->getLocalization()->getConnectionPlace()->getPhysicalStops().size())
					{
						HTMLForm ap(addPhysicalRequest.getHTMLForm("addphy"));
						stream << st.row();
						stream << st.col(2) << ap.open();
						stream << ap.getSelectInput(AddDepartureStopToDisplayScreenAction::PARAMETER_STOP, _displayScreen->getLocalization()->getConnectionPlace()->getPhysicalStopLabels(_displayScreen->getPhysicalStops()) , uid(0));
						stream << ap.getSubmitButton("Ajouter");
						stream << ap.close();
					}									
				}
				stream << st.close();

				// Intermediate stops to display
				stream << "<h1>Arr�ts interm�diaires � afficher</h1>";

				HTMLTable::ColsVector mtv;
				mtv.push_back("Arr�t");
				mtv.push_back("Action");
				HTMLTable mt(mtv);;
				stream << mt.open();
				
				for (DisplayedPlacesList::const_iterator it = _displayScreen->getDisplayedPlaces().begin(); it != _displayScreen->getDisplayedPlaces().end(); ++it)
				{
					HTMLForm mf(rmDisplayedRequest.getHTMLForm("rmdp" + Conversion::ToString((*it)->getKey())));
					mf.addHiddenField(DisplayScreenRemoveDisplayedPlaceAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()));

					stream << mt.row();
					stream << mt.col() << (*it)->getFullName();
					stream << mt.col() << mf.getLinkButton("Supprimer");
				}

				HTMLForm amf(addDisplayRequest.getHTMLForm("adddispl"));
				stream << mt.row();
				stream << mt.col(2) << amf.open();
				stream << amf.getSortedSelectInput(DisplayScreenAddDisplayedPlace::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getDisplayedPlaces()), uid(0));
				stream << amf.getSubmitButton("Ajouter");
				stream << amf.close();

				stream << mt.close();
			
				// Forbidden places
				stream << "<h1>Arr�ts ne devant pas �tre desservis par les lignes s�lectionn�es pour l'affichage</h1>";
				HTMLTable::ColsVector ntv;
				ntv.push_back("Arr�t");
				ntv.push_back("Action");
				HTMLTable nt(ntv);
				stream << nt.open();

				for (DisplayedPlacesList::const_iterator it = _displayScreen->getForbiddenPlaces().begin(); it != _displayScreen->getForbiddenPlaces().end(); ++it)
				{
					HTMLForm ntu(rmForbiddenRequest.getHTMLForm("rmfp"+ Conversion::ToString((*it)->getKey())));
					ntu.addHiddenField(DisplayScreenRemoveForbiddenPlaceAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()));
					stream << nt.row();
					stream << nt.col() << (*it)->getFullName();
					stream << nt.col() << ntu.getLinkButton("Supprimer");
				}
				HTMLForm ant(addNSRequest.getHTMLForm("addforb"));
				stream << nt.row();
				stream << nt.col(2) << ant.open();
				stream << ant.getSortedSelectInput(AddForbiddenPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForbiddenPlaces()), uid(0));
				stream << ant.getSubmitButton("Ajouter");
				stream << ant.close();
				
				stream << nt.close();
				
				// Preselection
				stream << "<h1>Pr�s�lection</h1>";

				HTMLForm tf(updPreselRequest.getHTMLForm("updpresel"));
				HTMLTable tt;
				stream << tf.open() << tt.open();
				
				stream << tt.row();
				stream << tt.col() << "Activer";
				stream << tt.col() << tf.getOuiNonRadioInput(UpdateDisplayPreselectionParametersAction::PARAMETER_ACTIVATE_PRESELECTION, _displayScreen->getGenerationMethod() == DisplayScreen::STANDARD_METHOD);
				
				stream << tt.row();
				stream << tt.col() << "D�lai maximum pr�s�lection";
				stream << tt.col() << tf.getTextInput(UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY, Conversion::ToString(_displayScreen->getForceDestinationDelay()));

				stream << tt.row();
				stream << tt.col(2) << tf.getSubmitButton("Enregister les param�tres de pr�s�lection");

				stream << tt.close() << tf.close();

				if (_displayScreen->getGenerationMethod() == DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD)
				{
					// Additional preselection stops
					stream << "<h1>Arr�ts de pr�s�lection (les terminus de lignes sont automatiquement pr�s�lectionn�s)</h1>";

					HTMLTable::ColsVector pstv;
					pstv.push_back("Arr�t");
					pstv.push_back("Action");
					HTMLTable pst(pstv);
					stream << pst.open();

					for (DisplayedPlacesList::const_iterator it = _displayScreen->getForcedDestinations().begin(); it != _displayScreen->getForcedDestinations().end(); ++it)
					{
						HTMLForm psdf(rmPreselRequest.getHTMLForm("rmpres" + Conversion::ToString((*it)->getKey())));
						psdf.addHiddenField(RemovePreselectionPlaceFromDisplayScreenAction::PARAMETER_PLACE, Conversion::ToString((*it)->getKey()));
						stream << pst.row();
						stream << pst.col() << (*it)->getFullName();
						stream << pst.col() << psdf.getLinkButton("Supprimer");
					}

					HTMLForm psaf(addPreselRequest.getHTMLForm("addpresel"));
					stream << pst.row();
					stream << pst.col(2) << psaf.open();
					stream << psaf.getSortedSelectInput(AddPreselectionPlaceToDisplayScreen::PARAMETER_PLACE, _displayScreen->getSortedAvaliableDestinationsLabels(_displayScreen->getForcedDestinations()), uid(0));
					stream << psaf.getSubmitButton("Ajouter");
					stream << psaf.close();

					stream << pst.close();
				}
			}

			stream
				<< "<p>NB : Certains types d'afficheurs ne prennent pas en charge toutes les fonctionnalit�s propos�es. Selon le type de l'afficheur, certains champs peuvent donc �tre sans effet sur l'affichage.</p>";
		}

		void DisplayAdmin::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
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
