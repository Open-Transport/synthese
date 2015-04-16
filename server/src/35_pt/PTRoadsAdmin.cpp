
//////////////////////////////////////////////////////////////////////////
/// PTRoadsAdmin class implementation.
///	@file PTRoadsAdmin.cpp
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

#include "PTRoadsAdmin.h"

#include "AdminParametersException.h"
#include "Road.h"
#include "RoadChunkEdge.hpp"
#include "RoadPath.hpp"
#include "ParametersMap.h"
#include "AdminActionFunctionRequest.hpp"
#include "RoadPlaceUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "ImportableAdmin.hpp"
#include "CityListFunction.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "ResultHTMLTable.h"
#include "PTRoadAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PTCitiesAdmin.h"
#include "PTPlacesAdmin.h"
#include "City.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace road;
	using namespace html;
	using namespace graph;
	using namespace db;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTRoadsAdmin>::FACTORY_KEY("PTRoadsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTRoadsAdmin>::ICON("pause_blue.png");
		template<> const string AdminInterfaceElementTemplate<PTRoadsAdmin>::DEFAULT_TITLE("Routes");
	}

	namespace pt
	{
		const string PTRoadsAdmin::TAB_PROPERTIES("tab_properties");
		const string PTRoadsAdmin::TAB_DETAILS("tab_details");

		PTRoadsAdmin::PTRoadsAdmin()
			: AdminInterfaceElementTemplate<PTRoadsAdmin>()
		{ }



		void PTRoadsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_roadPlace = Env::GetOfficialEnv().get<RoadPlace>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<RoadPlace> e)
			{
				throw AdminParametersException("No such road place");
			}
			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTRoadsAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_roadPlace)
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _roadPlace->getKey());
			}
			return m;
		}



		bool PTRoadsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTRoadsAdmin::display(
			ostream& stream,
			const Request& request
		) const	{
			////////////////////////////////////////////////////////////////////
			// DETAILS TAB
			if (openTabContent(stream, TAB_DETAILS))
			{
				AdminFunctionRequest<PTRoadAdmin> openRoadRequest(request);

				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Gauche");
				c.push_back("Gauche");
				c.push_back("Droite");
				c.push_back("Droite");
				c.push_back("Longueur");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(Road* road, _roadPlace->getRoads())
				{
					// Avoid auto generated objects
					RoadPath& mainRoad(road->getForwardPath());

					// New row
					stream << t.row();

					// Open button
					stream << t.col();
					openRoadRequest.getPage()->setRoad(
								Env::GetOfficialEnv().getSPtr(road)
								);
					stream << HTMLModule::getLinkButton(openRoadRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PTRoadAdmin::ICON);

					// Left first number
					stream << t.col();
					if( !mainRoad.getEdges().empty() &&
						static_cast<const RoadChunkEdge*>(mainRoad.getEdge(0))->getRoadChunk()->getLeftHouseNumberBounds()
						){
						stream <<
								  static_cast<const RoadChunkEdge*>(mainRoad.getEdge(0))->getRoadChunk()->getLeftHouseNumberBounds()->first
								  ;
					}

					// Left last number
					stream << t.col();
					if( mainRoad.getEdges().size() > 1 &&
						static_cast<const RoadChunkEdge*>(mainRoad.getLastEdge()->getPrevious())->getRoadChunk()->getLeftHouseNumberBounds()
						){
						stream <<
								  static_cast<const RoadChunkEdge*>(mainRoad.getLastEdge()->getPrevious())->getRoadChunk()->getLeftHouseNumberBounds()->second
								  ;
					}

					// Right first number
					stream << t.col();
					if( !mainRoad.getEdges().empty() &&
						static_cast<const RoadChunkEdge*>(mainRoad.getEdge(0))->getRoadChunk()->getRightHouseNumberBounds()
						){
						stream <<
								  static_cast<const RoadChunkEdge*>(mainRoad.getEdge(0))->getRoadChunk()->getRightHouseNumberBounds()->first
								  ;
					}

					// Left last number
					stream << t.col();
					if( mainRoad.getEdges().size() > 1 &&
						static_cast<const RoadChunkEdge*>(mainRoad.getLastEdge()->getPrevious())->getRoadChunk()->getRightHouseNumberBounds()
						){
						stream <<
								  static_cast<const RoadChunkEdge*>(mainRoad.getLastEdge()->getPrevious())->getRoadChunk()->getRightHouseNumberBounds()->second
								  ;
					}

					// Length
					stream << t.col();
					if( !mainRoad.getEdges().empty())
					{
						stream << mainRoad.getLastEdge()->getMetricOffset() - mainRoad.getEdge(0)->getMetricOffset();
					}

					// Delete
					stream << t.col();
					/// TODO
				}
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<RoadPlaceUpdateAction, PTRoadsAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setPlace(const_pointer_cast<RoadPlace>(_roadPlace));

				// General properties
				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_roadPlace->getKey()));
				if(_roadPlace->getCity())
				{
					stream << t.cell("Localité", _roadPlace->getCity()->getName());
				}
				stream << t.cell(
					"Localité",
					t.getForm().getTextInputAutoCompleteFromService(
						RoadPlaceUpdateAction::PARAMETER_CITY_ID,
						lexical_cast<string>(
							  _roadPlace->getCity() ?
							  _roadPlace->getCity()->getKey() :
							  RegistryKeyType(0)
						),
						_roadPlace->getCity() ? _roadPlace->getCity()->getName() : string(),
						pt_website::CityListFunction::FACTORY_KEY,
						pt_website::CityListFunction::DATA_CITIES,
						pt_website::CityListFunction::DATA_CITY,
						string(), string(),
						false, true, true, true
				)	);
				stream << t.cell(
					"Nom",
					t.getForm().GetTextInput(
						RoadPlaceUpdateAction::PARAMETER_NAME,
						_roadPlace->getName()
				)	);
				stream << t.cell(
					"Rue principale",
					t.getForm().getOuiNonRadioInput(
					RoadPlaceUpdateAction::PARAMETER_IS_MAIN,
					_roadPlace->getCity() ?
						  _roadPlace->getCity()->includes(*_roadPlace) :
						  false
				)	);
				stream << t.close();

				// Importable admin
				StaticActionRequest<RoadPlaceUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setPlace(const_pointer_cast<RoadPlace>(_roadPlace));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_roadPlace, updateOnlyRequest);
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTRoadsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				BOOST_FOREACH(Road* road, _roadPlace->getRoads())
				{
					boost::shared_ptr<PTRoadAdmin> p(getNewPage<PTRoadAdmin>());
					p->setRoad(Env::GetOfficialEnv().getSPtr(road));
					links.push_back(p);
				}
			}
			return links;
		}



		void PTRoadsAdmin::setRoadPlace( boost::shared_ptr<const RoadPlace> value )
		{
			_roadPlace = value;
		}



		AdminInterfaceElement::PageLinks PTRoadsAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			links.push_back(getNewPage<PTCitiesAdmin>());
			boost::shared_ptr<PTPlacesAdmin> p(getNewPage<PTPlacesAdmin>());
			p->setCity(Env::GetOfficialEnv().getSPtr(_roadPlace->getCity()));
			links.push_back(p);
			links.push_back(getNewCopiedPage());

			return links;
		}



		std::string PTRoadsAdmin::getTitle() const
		{
			return _roadPlace.get() ? _roadPlace->getName() : DEFAULT_TITLE;
		}


		void PTRoadsAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Détails", TAB_DETAILS, true));
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));

			_tabBuilded = true;
		}
	}
}
