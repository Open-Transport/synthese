
//////////////////////////////////////////////////////////////////////////
/// PTPlacesAdmin class implementation.
///	@file PTPlacesAdmin.cpp
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

#include "PTPlacesAdmin.h"

#include "AdminParametersException.h"
#include "ObjectCreateAction.hpp"
#include "ParametersMap.h"
#include "PTModule.h"
#include "PublicPlaceAdmin.hpp"
#include "PublicPlaceTableSync.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "City.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "ResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "FrenchSentence.h"
#include "PTPlaceAdmin.h"
#include "PTRoadsAdmin.h"
#include "StopArea.hpp"
#include "PublicPlace.h"
#include "RoadPlace.h"
#include "PTCitiesAdmin.h"
#include "Profile.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "CityUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "StopAreaAddAction.h"
#include "StopAreaTableSync.hpp"
#include "RemoveObjectAction.hpp"
#include "PlaceAlias.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace geography;
	using namespace html;
	using namespace lexical_matcher;
	using namespace road;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTPlacesAdmin>::FACTORY_KEY("PTPlaces");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTPlacesAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTPlacesAdmin>::DEFAULT_TITLE("Recherche de lieu");
	}

	namespace pt
	{
		const string PTPlacesAdmin::PARAM_SEARCH_NAME("sn");
		const string PTPlacesAdmin::PARAM_SEARCH_CITY("sc");
		const string PTPlacesAdmin::TAB_ALIASES("al");
		const string PTPlacesAdmin::TAB_CONNECTION_PLACES("cp");
		const string PTPlacesAdmin::TAB_PHONETIC("ph");
		const string PTPlacesAdmin::TAB_PUBLIC_PLACES("pp");
		const string PTPlacesAdmin::TAB_ROAD_PLACES("rp");
		const string PTPlacesAdmin::TAB_PROPERTIES("pr");

		PTPlacesAdmin::PTPlacesAdmin()
			: AdminInterfaceElementTemplate<PTPlacesAdmin>()
		{ }



		void PTPlacesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchCity = map.getDefault<string>(PARAM_SEARCH_CITY);
			_searchName = map.getDefault<string>(PARAM_SEARCH_NAME);

			if(map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
			{
				try
				{
					_city = Env::GetOfficialEnv().get<City>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				}
				catch (ObjectNotFoundException<City> e)
				{
					throw AdminParametersException("No such city");
				}
			}

			// Search table initialization
			_requestParameters.setFromParametersMap(map, PARAM_SEARCH_CITY, 30);
		}



		ParametersMap PTPlacesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_city.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _city->getKey());
			}

			return m;
		}



		bool PTPlacesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTPlacesAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PHONETIC
			if (openTabContent(stream, TAB_PHONETIC))
			{
				AdminFunctionRequest<PTPlacesAdmin> searchRequest(request, *this);
				searchRequest.getPage()->setCity(_city);

				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
				AdminFunctionRequest<PTRoadsAdmin> openRoadRequest(request);
				AdminFunctionRequest<PublicPlaceAdmin> openPPRequest(request);

				stream << "<h1>Recherche phonétique</h1>";

				SearchFormHTMLTable t(searchRequest.getHTMLForm());
				stream << t.open();
				if(!_city.get())
				{
					stream << t.cell("Localité", t.getForm().getTextInput(PARAM_SEARCH_CITY, _searchCity));
				}
				stream << t.cell("Nom", t.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
				stream << t.close();

				if(!_searchName.empty())
				{
					stream << "<h1>Résultats</h1>";

					stream << "<p>Phonétique du texte recherché : " << FrenchSentence(_searchName).getPhoneticString() << "</p>";

					HTMLTable::ColsVector c;
					c.push_back("Type");
					c.push_back("Type");
					c.push_back("Lieu");
					c.push_back("Clé");
					c.push_back("Phonétique");
					c.push_back("Score");
					c.push_back("Levenshtein");
					c.push_back("Actions");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					const City::PlacesMatcher& matcher(_city->getAllPlacesMatcher());
					City::PlacesMatcher::MatchResult result(matcher.bestMatches(_searchName, 20));

					BOOST_FOREACH(const City::PlacesMatcher::MatchHit& it, result)
					{
						stream << t.row();

						if(dynamic_pointer_cast<RoadPlace>(it.value).get())
						{
							boost::shared_ptr<RoadPlace> roadPlace(static_pointer_cast<RoadPlace>(it.value));
							openRoadRequest.getPage()->setRoadPlace(roadPlace);

							stream << t.col();
							stream << t.col() << "Route";
							stream << t.col() << roadPlace->getName();
							stream << t.col() << it.key.getSource();
							stream << t.col() << it.key.getPhoneticString();
							stream << t.col() << it.score.phoneticScore;
							stream << t.col() << it.score.levenshtein;
							stream << t.col() << HTMLModule::getLinkButton(openRoadRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else if(dynamic_pointer_cast<StopArea>(it.value).get())
						{
							boost::shared_ptr<StopArea> connectionPlace(static_pointer_cast<StopArea>(it.value));
							openPlaceRequest.getPage()->setConnectionPlace(connectionPlace);

							stream << t.col();
							stream << t.col() << "Zone d'arrêt";
							stream << t.col() << connectionPlace->getName();
							stream << t.col() << it.key.getSource();
							stream << t.col() << it.key.getPhoneticString();
							stream << t.col() << it.score.phoneticScore;
							stream << t.col() << it.score.levenshtein;
							stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else if(dynamic_cast<const PlaceAlias*>(it.value.get()))
						{
							boost::shared_ptr<PlaceAlias> alias(static_pointer_cast<PlaceAlias>(it.value));
							const StopArea* connectionPlace(dynamic_cast<const StopArea*>(alias->getAliasedPlace()));
							if(connectionPlace)
							{
								openPlaceRequest.getPage()->setConnectionPlace(Env::GetOfficialEnv().getSPtr<StopArea>(connectionPlace));

								stream << t.col();
								stream << t.col() << "Alias de zone d'arrêt";
								stream << t.col() << connectionPlace->getName();
								stream << t.col() << it.key.getSource();
								stream << t.col() << it.key.getPhoneticString();
								stream << t.col() << it.score.phoneticScore;
								stream << t.col() << it.score.levenshtein;
								stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(), "Ouvrir", string(), "building.png");
							}
						}
						else if(dynamic_cast<const PublicPlace*>(it.value.get()))
						{
							const PublicPlace* publicPlace(static_cast<const PublicPlace*>(it.value.get()));
							openPPRequest.getPage()->setPlace(Env::GetOfficialEnv().getSPtr(publicPlace));

							stream << t.col();
							stream << t.col() << "Lieu public";
							stream << t.col() << publicPlace->getName();
							stream << t.col() << it.key.getSource();
							stream << t.col() << it.key.getPhoneticString();
							stream << t.col() << it.score.phoneticScore;
							stream << t.col() << it.score.levenshtein;
							stream << t.col() << HTMLModule::getLinkButton(openPPRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else
						{
							stream << t.col();
							stream << t.col() << "???";
							stream << t.col() << string();
							stream << t.col() << it.key.getSource();
							stream << t.col() << it.key.getPhoneticString();
							stream << t.col() << it.score.phoneticScore;
							stream << t.col() << it.score.levenshtein;
							stream << t.col();
						}
					}

					stream << t.close();
				}

				if(_city.get())
				{
					stream << "<h1>Lieux principaux</h1>";

					HTMLTable::ColsVector c;
					c.push_back("Type");
					c.push_back("Type");
					c.push_back("Lieu");
					c.push_back("Actions");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					BOOST_FOREACH(const City::IncludedPlaces::value_type it, _city->getIncludedPlaces())
					{
						stream << t.row();

						if(dynamic_cast<const RoadPlace*>(it))
						{
							const RoadPlace* roadPlace(dynamic_cast<const RoadPlace*>(it));
							openRoadRequest.getPage()->setRoadPlace(Env::GetOfficialEnv().getSPtr(roadPlace));

							stream << t.col();
							stream << t.col() << "Route";
							stream << t.col() << roadPlace->getName();
							stream << t.col() << HTMLModule::getLinkButton(openRoadRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else if(dynamic_cast<const StopArea*>(it))
						{
							const StopArea* connectionPlace(dynamic_cast<const StopArea*>(it));
							openPlaceRequest.getPage()->setConnectionPlace(Env::GetOfficialEnv().getSPtr(connectionPlace));

							stream << t.col();
							stream << t.col() << "Zone d'arrêt";
							stream << t.col() << connectionPlace->getName();
							stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else if(dynamic_cast<const PublicPlace*>(it))
						{
							const PublicPlace* publicPlace(dynamic_cast<const PublicPlace*>(it));
							openPPRequest.getPage()->setPlace(Env::GetOfficialEnv().getSPtr(publicPlace));

							stream << t.col();
							stream << t.col() << "Lieu public";
							stream << t.col() << publicPlace->getName();
							stream << t.col() << HTMLModule::getLinkButton(openPPRequest.getURL(), "Ouvrir", string(), "building.png");
						}
						else
						{
							stream << t.col();
							stream << t.col() << "???";
							stream << t.col() << string();
							stream << t.col();
						}
					}
					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONNECTION PLACES
			if (openTabContent(stream, TAB_CONNECTION_PLACES))
			{
				stream << "<h1>Recherche</h1>";
				AdminFunctionRequest<PTPlacesAdmin> searchRequest(request, *this);
				searchRequest.getPage()->setCity(_city);
				getHTMLStopAreaSearchForm(
					stream,
					searchRequest.getHTMLForm("connsearch"),
					_city.get() ? optional<const string&>() : optional<const string&>(_searchCity),
					_searchName
				);

				stream << "<h1>Résultats</h1>";

				StopAreaTableSync::SearchResult places(
					StopAreaTableSync::Search(
						Env::GetOfficialEnv(),
						_city.get() ? _city->getKey() : optional<RegistryKeyType>(),
						logic::indeterminate,
						optional<string>(),
						_searchName.empty() ? optional<string>() : ("%"+ _searchName +"%"),
						_city.get() ? optional<string>() : (_searchCity.empty() ? string() : ("%"+ _searchCity +"%")),
						true,
						true,
						0
				)	);

				AdminActionFunctionRequest<StopAreaAddAction,PTPlaceAdmin> stopAddRequest(request);
				stopAddRequest.getAction()->setCity(const_pointer_cast<City>(_city));
				stopAddRequest.getAction()->setCreateCityIfNecessary(false);
				stopAddRequest.setActionWillCreateObject();
				stopAddRequest.setActionFailedPage<PTPlacesAdmin>();
				static_pointer_cast<PTPlacesAdmin>(stopAddRequest.getActionFailedPage())->setCity(_city);

				AdminActionFunctionRequest<RemoveObjectAction, PTPlacesAdmin> removeRequest(request, *this);

				AdminFunctionRequest<PTPlaceAdmin> openRequest(request);

				HTMLForm f(stopAddRequest.getHTMLForm());
				stream << f.open();

				HTMLTable::ColsVector c;
				c.push_back("id");
				if(!_city.get())
				{
					c.push_back("Localité");
				}
				c.push_back("Nom");
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Actions");
				c.push_back("Actions");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const StopAreaTableSync::SearchResult::value_type& place, places)
				{
					stream << t.row();
					stream << t.col() << place->getKey();
					if(!_city.get())
					{
						stream << t.col() << place->getCity()->getName();
					}
					stream << t.col() << place->getName();
					if(place->getPoint().get())
					{
						stream << t.col() << fixed << place->getPoint()->getX();
						stream << t.col() << fixed << place->getPoint()->getY();
					}
					else
					{
						stream << t.col(2) << "Non localisé";
					}

					openRequest.getPage()->setConnectionPlace(const_pointer_cast<const StopArea>(place));
					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PTPlaceAdmin::ICON);

					// Remove button only if no stops inside
					stream << t.col();
					if(place->getPhysicalStops().empty())
					{
						removeRequest.getAction()->setObjectId(place->getKey());
						stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la zone d'arrêt ?");
					}
				}

				stream << t.row();
				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << t.col() << f.getSubmitButton("Ajouter");
				stream << t.col();

				stream << t.close();
				stream << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB ROAD PLACES
			if (openTabContent(stream, TAB_ROAD_PLACES))
			{
			}

			////////////////////////////////////////////////////////////////////
			// TAB PUBLIC PLACES
			if (openTabContent(stream, TAB_PUBLIC_PLACES))
			{
				stream << "<h1>Recherche</h1>";
				AdminFunctionRequest<PTPlacesAdmin> searchRequest(request, *this);
				searchRequest.getPage()->setCity(_city);
				getHTMLStopAreaSearchForm(
					stream,
					searchRequest.getHTMLForm("ppsearch"),
					_city.get() ? optional<const string&>() : optional<const string&>(_searchCity),
					_searchName
				);

				stream << "<h1>Résultats</h1>";

				PublicPlaceTableSync::SearchResult places(
					PublicPlaceTableSync::Search(
						Env::GetOfficialEnv(),
						_city.get() ? _city->getKey() : optional<RegistryKeyType>(),
						_searchName.empty() ? optional<string>() : ("%"+ _searchName +"%"),
						0
				)	);

				AdminActionFunctionRequest<ObjectCreateAction, PublicPlaceAdmin> publicPlaceAddRequest(request);
				publicPlaceAddRequest.getAction()->setTable<PublicPlace>();
				PublicPlace ppTemplate;
				ppTemplate.setCity(const_pointer_cast<City>(_city).get());
				publicPlaceAddRequest.getAction()->set<NamedPlaceField>(NULL, ppTemplate);
				publicPlaceAddRequest.setActionWillCreateObject();
				publicPlaceAddRequest.setActionFailedPage<PTPlacesAdmin>();
				static_pointer_cast<PTPlacesAdmin>(publicPlaceAddRequest.getActionFailedPage())->setCity(_city);

				AdminActionFunctionRequest<RemoveObjectAction, PTPlacesAdmin> removeRequest(request, *this);

				AdminFunctionRequest<PublicPlaceAdmin> openRequest(request);

				HTMLForm f(publicPlaceAddRequest.getHTMLForm());
				stream << f.open();

				HTMLTable::ColsVector c;
				c.push_back("id");
				if(!_city.get())
				{
					c.push_back("Localité");
				}
				c.push_back("ID");
				c.push_back("Nom");
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Actions");
				c.push_back("Actions");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const PublicPlaceTableSync::SearchResult::value_type& place, places)
				{
					stream << t.row();
					openRequest.getPage()->setPlace(const_pointer_cast<const PublicPlace>(place));
					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PublicPlaceAdmin::ICON);

					stream << t.col() << place->getKey();
					if(!_city.get() && place->getCity())
					{
						stream << t.col() << place->getCity()->getName();
					}
					stream << t.col() << place->getName();
					if(place->getPoint().get())
					{
						stream << t.col() << fixed << place->getPoint()->getX();
						stream << t.col() << fixed << place->getPoint()->getY();
					}
					else
					{
						stream << t.col(2) << "Non localisé";
					}

					// Remove button only if no stops inside
					stream << t.col();
					if(place->getEntrances().empty())
					{
						removeRequest.getAction()->setObjectId(place->getKey());
						stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le lieu public ?");
					}
				}

				stream << t.row();
				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << t.col() << f.getSubmitButton("Ajouter");
				stream << t.col();

				stream << t.close();
				stream << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB ALIASED PLACES
			if (openTabContent(stream, TAB_ALIASES))
			{
			}


			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				AdminActionFunctionRequest<CityUpdateAction,PTPlacesAdmin> updateRequest(request);
				updateRequest.getAction()->setCity(const_pointer_cast<City>(_city));
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_city->getKey()));
				stream << t.cell("Nom", t.getForm().getTextInput(CityUpdateAction::PARAMETER_NAME, _city->getName()));
				stream << t.cell("Code", t.getForm().getTextInput(CityUpdateAction::PARAMETER_CODE, _city->getCode()));
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTPlacesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks PTPlacesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage.getCurrentTreeBranch().find(*this))
			{
				links.push_back(currentPage.getCurrentTreeBranch().getNextSubPage(*this));
			}

			return links;
		}


		std::string PTPlacesAdmin::getTitle() const
		{
			return _city.get() ? _city->getName() : DEFAULT_TITLE;
		}



		void PTPlacesAdmin::setCity( boost::shared_ptr<const geography::City> value )
		{
			_city = value;
		}



		void PTPlacesAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			if(_city.get())
			{
				_tabs.push_back(Tab("Lieux", TAB_PHONETIC, true, "text_allcaps.png"));
			}
			_tabs.push_back(Tab("Zones d'arrêt", TAB_CONNECTION_PLACES, true, "building.png"));
			_tabs.push_back(Tab("Routes", TAB_ROAD_PLACES, true, "building.png"));
			_tabs.push_back(Tab("Lieux publics", TAB_PUBLIC_PLACES, true, "building.png"));
			_tabs.push_back(Tab("Alias", TAB_ALIASES, true, "building.png"));
			if(_city.get())
			{
				_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true, "building.png"));
			}
			_tabBuilded = true;
		}



		bool PTPlacesAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			if(!_city.get() != !static_cast<const PTPlacesAdmin&>(other)._city.get()) return false;
			return !_city.get() ||
				_city->getKey() == static_cast<const PTPlacesAdmin&>(other)._city->getKey();
		}



		AdminInterfaceElement::PageLinks PTPlacesAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			if(_city.get())
			{
				links.push_back(getNewPage<PTCitiesAdmin>());
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		void PTPlacesAdmin::getHTMLStopAreaSearchForm(
			std::ostream& stream,
			const html::HTMLForm& form,
			boost::optional<const std::string&> cityName,
			boost::optional<const std::string&> stopName
		){
			SearchFormHTMLTable st(form);
			stream << st.open();
			if(cityName)
			{
				stream << st.cell("Localité", st.getForm().getTextInput(PARAM_SEARCH_CITY, *cityName));
			}
			if(stopName)
			{
				stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, *stopName));
			}
			stream << st.close();
		}
	}
}
