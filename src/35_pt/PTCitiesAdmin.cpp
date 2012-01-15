
//////////////////////////////////////////////////////////////////////////
/// PTCitiesAdmin class implementation.
///	@file PTCitiesAdmin.cpp
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTCitiesAdmin.h"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "CityAddAction.h"
#include "FrenchSentence.h"
#include "GeographyModule.h"
#include "House.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTPlaceAdmin.h"
#include "PTPlacesAdmin.h"
#include "PTRoadsAdmin.h"
#include "RemoveObjectAction.hpp"
#include "ResultHTMLTable.h"
#include "RoadPlace.h"
#include "StopArea.hpp"
#include "TransportNetworkRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace geography;
	using namespace lexical_matcher;
	using namespace html;
	using namespace pt;
	using namespace pt_website;
	using namespace road;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTCitiesAdmin>::FACTORY_KEY("PTCitiesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::DEFAULT_TITLE("Lieux");
	}

	namespace pt
	{
		PTCitiesAdmin::PTCitiesAdmin():
			AdminInterfaceElementTemplate<PTCitiesAdmin>()
		{}



		void PTCitiesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_placesListService._setFromParametersMap(map);
			_placesListService.setNumber(10);
			_placesListService.setOutputFormat(string());
		}



		ParametersMap PTCitiesAdmin::getParametersMap() const
		{
			ParametersMap m;

			m.merge(_placesListService._getParametersMap());

			return m;
		}



		bool PTCitiesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTCitiesAdmin::display(
			ostream& stream,
			const AdminRequest& request
		) const	{

			// The search Form
			AdminFunctionRequest<PTCitiesAdmin> searchRequest(request);
			stream << "<h1>Recherche</h1>";
			PropertiesHTMLTable t(searchRequest.getHTMLForm());
			stream << t.open();
			stream << t.cell(
				"Nom",
				t.getForm().getTextInput(
					PlacesListService::PARAMETER_TEXT,
					_placesListService.getText()
			)	);
			stream << t.cell(
				"Communes avec arrêt uniquement",
				t.getForm().getOuiNonRadioInput(
					PlacesListService::PARAMETER_CITIES_WITH_AT_LEAST_A_STOP,
					_placesListService.getCitiesWithAtLeastAStop()
			)	);
			stream << t.close();

			// Search service
			ParametersMap pm(
				_placesListService.runWithoutOutput()
			);

			// Informations about the query
			if(!_placesListService.getText().empty())
			{
				// Phonetic string
				stream <<
					"<p>Phonétique du texte recherché : " <<
					FrenchSentence(_placesListService.getText()).getPhoneticString() <<
					"<br />Tests formats normalisés :"
				;

				// XML response
				StaticFunctionRequest<PlacesListService> testRequest;
				testRequest.getFunction()->_setFromParametersMap(_placesListService._getParametersMap());
				testRequest.getFunction()->setOutputFormat(PlacesListService::VALUE_XML);
				stream << HTMLModule::getLinkButton(testRequest.getURL(), "Réponse XML");

				// JSON response
				testRequest.getFunction()->setOutputFormat(PlacesListService::VALUE_JSON);
				stream << HTMLModule::getLinkButton(testRequest.getURL(), "Réponse JSON");

				// Best match
				PlacesListService::PlaceResult placeResult(
					_placesListService.getPlaceFromBestResult(pm)
				);
				stream << "<br />Meilleure correspondance : " << placeResult.key.getSource() << "</p>";
			}

			// Remove request (common for place class)
			AdminActionFunctionRequest<RemoveObjectAction, PTCitiesAdmin> removeRequest(request);


			//////////////////////////////////////////////////////////////////////////
			// Cities
			{
				stream << "<h1>Localités</h1>";

				// Requests
				AdminFunctionRequest<PTPlacesAdmin> openCityRequest(request);
				AdminActionFunctionRequest<CityAddAction,PTPlacesAdmin> creationRequest(request);
				creationRequest.setActionWillCreateObject();
				creationRequest.getFunction()->setActionFailedPage(getNewCopiedPage());
				HTMLForm creationForm(creationRequest.getHTMLForm("create_city"));

				// The table
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Localité");
				c.push_back("Code");
				c.push_back("Phonétique");
				c.push_back("Score");
				c.push_back("Levenshtein");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << creationForm.open() << t.open();
				const ParametersMap& citiesPM(
					**pm.getSubMaps(PlacesListService::DATA_CITIES).begin()
				);
				if(citiesPM.hasSubMaps(PlacesListService::DATA_CITY))
				{
					BOOST_FOREACH(
						shared_ptr<ParametersMap> item,
						citiesPM.getSubMaps(PlacesListService::DATA_CITY)
					){
						// New row
						stream << t.row();

						// City load
						shared_ptr<const City> city(
							Env::GetOfficialEnv().get<City>(
								item->get<RegistryKeyType>(City::DATA_CITY_ID)
						)	);

						// Open button
						openCityRequest.getPage()->setCity(city);
						stream << t.col() <<
							HTMLModule::getLinkButton(
								openCityRequest.getURL(),
								"Ouvrir",
								string(),
								PTPlacesAdmin::ICON
							)
						;

						// Key
						stream << t.col() << item->get<string>(PlacesListService::DATA_KEY);
						stream << t.col() << item->get<string>(City::DATA_CITY_CODE);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_STRING);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_SCORE);
						stream << t.col() << item->get<string>(PlacesListService::DATA_LEVENSHTEIN);
						
						// Remove button
						stream << t.col();
						if(city->empty())
						{
							removeRequest.getAction()->setObjectId(city->getKey());
							stream << HTMLModule::getLinkButton(
								removeRequest.getURL(),
								"Supprimer",
								"Etes-vous sûr de vouloir supprimer la localité "+ city->getName() +" ?"
							);
						}
				}	}

				// Creation row
				stream << t.row();
				stream << t.col();
				stream << t.col() << creationForm.getTextInput(CityAddAction::PARAMETER_NAME, string(), "(nom de la localité)");
				stream << t.col() << creationForm.getTextInput(CityAddAction::PARAMETER_CODE, string(), "(code de la localité)");
				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << t.col() << creationForm.getSubmitButton("Ajouter");

				// Table and form closing
				stream << t.close() << creationForm.close();
			}


			//////////////////////////////////////////////////////////////////////////
			// Stops
			{
				stream << "<h1>Arrêts</h1>";

				// Requests
				AdminFunctionRequest<PTPlaceAdmin> openStopRequest(request);
				
				// The table
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Localité");
				c.push_back("Arrêt");
				c.push_back("Phonétique");
				c.push_back("Score");
				c.push_back("Levenshtein");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				const ParametersMap& stopsPM(
					**pm.getSubMaps(PlacesListService::DATA_STOPS).begin()
				);
				if(stopsPM.hasSubMaps(PlacesListService::DATA_STOP))
				{
					BOOST_FOREACH(
						shared_ptr<ParametersMap> item,
						stopsPM.getSubMaps(PlacesListService::DATA_STOP)
					){
						// New row
						stream << t.row();

						// Load of the stop area
						shared_ptr<const StopArea> stopArea(
							Env::GetOfficialEnv().get<StopArea>(
								item->get<RegistryKeyType>(StopArea::DATA_STOP_ID)
						)	);

						// Open button
						openStopRequest.getPage()->setConnectionPlace(stopArea);
						stream << t.col() <<
							HTMLModule::getLinkButton(
								openStopRequest.getURL(),
								"Ouvrir",
								string(),
								PTPlaceAdmin::ICON
							)
						;

						// Key
						stream << t.col() << item->get<string>(StopArea::DATA_CITY_NAME);
						stream << t.col() << item->get<string>(StopArea::DATA_STOP_NAME);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_STRING);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_SCORE);
						stream << t.col() << item->get<string>(PlacesListService::DATA_LEVENSHTEIN);

						// Remove button only if no stops inside
						stream << t.col();
						if(stopArea->getPhysicalStops().empty())
						{
							removeRequest.getAction()->setObjectId(stopArea->getKey());
							stream << HTMLModule::getLinkButton(
								removeRequest.getURL(),
								"Supprimer",
								"Etes-vous sûr de vouloir supprimer la zone d'arrêt "+ stopArea->getFullName() +" ?"
							);
						}
				}	}

				// Table and form closing
				stream << t.close();
			}


			//////////////////////////////////////////////////////////////////////////
			// Road places
			if(pm.hasSubMaps(PlacesListService::DATA_ROADS))
			{
				stream << "<h1>Rues</h1>";

				// Requests
				AdminFunctionRequest<PTRoadsAdmin> openRoadRequest(request);
				
				// The table
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Localité");
				c.push_back("Rue");
				c.push_back("Phonétique");
				c.push_back("Score");
				c.push_back("Levenshtein");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				const ParametersMap& roadsPM(
					**pm.getSubMaps(PlacesListService::DATA_ROADS).begin()
				);
				if(roadsPM.hasSubMaps(PlacesListService::DATA_ROAD))
				{
					BOOST_FOREACH(
						shared_ptr<ParametersMap> item,
						roadsPM.getSubMaps(PlacesListService::DATA_ROAD)
					){
						stream << t.row();

						// Open button
						openRoadRequest.getPage()->setRoadPlace(
							Env::GetOfficialEnv().get<RoadPlace>(
								item->get<RegistryKeyType>(RoadPlace::DATA_ID)
						)	);
						stream << t.col() <<
							HTMLModule::getLinkButton(
								openRoadRequest.getURL(),
								"Ouvrir",
								string(),
								PTRoadsAdmin::ICON
							)
						;

						// Key
						stream << t.col() << item->get<string>(City::DATA_CITY_NAME);
						stream << t.col() << item->get<string>(RoadPlace::DATA_NAME);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_STRING);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_SCORE);
						stream << t.col() << item->get<string>(PlacesListService::DATA_LEVENSHTEIN);
				}	}

				// Table and form closing
				stream << t.close();
			}


			//////////////////////////////////////////////////////////////////////////
			// Addresses
			if(pm.hasSubMaps(PlacesListService::DATA_ADDRESSES))
			{
				stream << "<h1>Rues</h1>";

				// Requests
				AdminFunctionRequest<PTRoadsAdmin> openRoadRequest(request);
				
				// The table
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Localité");
				c.push_back("Rue");
				c.push_back("Numéro");
				c.push_back("Phonétique");
				c.push_back("Score");
				c.push_back("Levenshtein");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				const ParametersMap& roadsPM(
					**pm.getSubMaps(PlacesListService::DATA_ADDRESSES).begin()
				);
				if(roadsPM.hasSubMaps(PlacesListService::DATA_ADDRESS))
				{
					BOOST_FOREACH(
						shared_ptr<ParametersMap> item,
						roadsPM.getSubMaps(PlacesListService::DATA_ADDRESS)
					){
						stream << t.row();

						// Open button
						openRoadRequest.getPage()->setRoadPlace(
							Env::GetOfficialEnv().get<RoadPlace>(
								item->get<RegistryKeyType>(House::DATA_ROAD_PREFIX + RoadPlace::DATA_ID)
						)	);
						stream << t.col() <<
							HTMLModule::getLinkButton(
								openRoadRequest.getURL(),
								"Ouvrir",
								string(),
								PTRoadsAdmin::ICON
							)
						;

						// Key
						stream << t.col() << item->get<string>(House::DATA_ROAD_PREFIX + City::DATA_CITY_NAME);
						stream << t.col() << item->get<string>(House::DATA_ROAD_PREFIX + RoadPlace::DATA_NAME);
						stream << t.col() << item->get<string>(House::DATA_NUMBER);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_STRING);
						stream << t.col() << item->get<string>(PlacesListService::DATA_PHONETIC_SCORE);
						stream << t.col() << item->get<string>(PlacesListService::DATA_LEVENSHTEIN);
				}	}

				// Table and form closing
				stream << t.close();
			}
		}



		AdminInterfaceElement::PageLinks PTCitiesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}
}	}
