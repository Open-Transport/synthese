
//////////////////////////////////////////////////////////////////////////
/// PTRoadAdmin class implementation.
///	@file PTRoadAdmin.cpp
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTRoadAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "Road.h"
#include "RoadPlace.h"
#include "ResultHTMLTable.h"
#include "Crossing.h"
#include "PublicPlace.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Edge.h"
#include "Vertex.h"
#include "City.h"
#include "HTMLModule.h"
#include "Address.h"
#include "AdminFunctionRequest.hpp"
#include "AddressablePlace.h"
#include "PTRoadsAdmin.h"
#include "PTPlaceAdmin.h"
#include "Profile.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace road;
	using namespace html;
	using namespace graph;
	using namespace geography;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTRoadAdmin>::FACTORY_KEY("PTRoadAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTRoadAdmin>::ICON("pause_blue.png");
		template<> const string AdminInterfaceElementTemplate<PTRoadAdmin>::DEFAULT_TITLE("Route");
	}

	namespace pt
	{
		const string PTRoadAdmin::TAB_DETAIL("de");
		const string PTRoadAdmin::TAB_MAP("ma");
		const string PTRoadAdmin::TAB_PROPERTIES("pr");

		PTRoadAdmin::PTRoadAdmin()
			: AdminInterfaceElementTemplate<PTRoadAdmin>()
		{ }


		
		void PTRoadAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_road = Env::GetOfficialEnv().get<Road>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<Road> e)
			{
				throw AdminParametersException("No such road");
			}

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTRoadAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			if(_road.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _road->getKey());

			return m;
		}


		
		bool PTRoadAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTRoadAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{
		
			////////////////////////////////////////////////////////////////////
			// TAB MAP
			if (openTabContent(stream, TAB_MAP))
			{
			}

			////////////////////////////////////////////////////////////////////
			// TAB DETAIL
			if (openTabContent(stream, TAB_DETAIL))
			{
				AdminFunctionRequest<PTRoadAdmin> openRoadRequest(request);
				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

				HTMLTable::ColsVector c;
				c.push_back("Rang");
				c.push_back("PM");
				c.push_back("Type");
				c.push_back("Nom");
				c.push_back("Liens");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const Edge* edge, _road->getEdges())
				{
					const Hub* place(edge->getFromVertex()->getHub());

					stream << t.row();
					stream << t.col();
					stream << edge->getRankInPath();
					stream << t.col();
					stream << edge->getMetricOffset();
					if(dynamic_cast<const Crossing*>(place))
					{
						stream << t.col(2);
						stream << "Intersection de routes";
					}
					else if(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(place))
					{
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(static_cast<const PublicTransportStopZoneConnectionPlace*>(place))
						);

						stream << t.col();
						stream << "Zone d'arr�t";
						stream << t.col();
						stream << HTMLModule::getHTMLLink(openPlaceRequest.getURL(), static_cast<const PublicTransportStopZoneConnectionPlace*>(place)->getName());
					}
					else if(dynamic_cast<const PublicPlace*>(place))
					{
						openPlaceRequest.getPage()->setPublicPlace(
							Env::GetOfficialEnv().getSPtr(static_cast<const PublicPlace*>(place))
						);

						stream << t.col();
						stream << "Lieu public";
						stream << t.col();
						stream << HTMLModule::getHTMLLink(openPlaceRequest.getURL(), static_cast<const PublicPlace*>(place)->getName());
					}

					stream << t.col();
					set<const Road*> roads;
					if(dynamic_cast<const Crossing*>(place))
					{
						const Address* address(static_cast<const Crossing*>(place)->getAddress());
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, address->getDepartureEdges())
						{
							roads.insert(dynamic_cast<const Road*>(edge.second->getParentPath()));
						}
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, address->getArrivalEdges())
						{
							roads.insert(dynamic_cast<const Road*>(edge.second->getParentPath()));
						}
					}
					if(dynamic_cast<const AddressablePlace*>(place))
					{
						BOOST_FOREACH(const Address* address, static_cast<const AddressablePlace*>(place)->getAddresses())
						{
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, address->getDepartureEdges())
							{
								roads.insert(dynamic_cast<const Road*>(edge.second->getParentPath()));
							}
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, address->getArrivalEdges())
							{
								roads.insert(dynamic_cast<const Road*>(edge.second->getParentPath()));
							}
						}
					}
					BOOST_FOREACH(const Road* road, roads)
					{
						if(road->getKey() <= 0 || road == _road.get()) continue;
						openRoadRequest.getPage()->setRoad(Env::GetOfficialEnv().getSPtr(road));
						stream << HTMLModule::getHTMLLink(openRoadRequest.getURL(), road->getRoadPlace()->getName()) << " ";
					}
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		std::string PTRoadAdmin::getTitle() const
		{
			return _road.get() ? _road->getRoadPlace()->getFullName() : DEFAULT_TITLE;
		}



		void PTRoadAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			_tabs.push_back(Tab("Carte", TAB_MAP, true));
			_tabs.push_back(Tab("D�tail", TAB_DETAIL, true));
			_tabs.push_back(Tab("Propri�t�s", TAB_PROPERTIES, true));
			_tabBuilded = true;
		}



		bool PTRoadAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			return _road->getKey() == static_cast<const PTRoadAdmin&>(other)._road->getKey();
		}



		void PTRoadAdmin::setRoad( boost::shared_ptr<const road::Road> value )
		{
			_road = value;
		}



		AdminInterfaceElement::PageLinks PTRoadAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			shared_ptr<PTRoadsAdmin> p(getNewOtherPage<PTRoadsAdmin>(false));
			p->setRoadPlace(Env::GetOfficialEnv().getSPtr(
				_road->getRoadPlace()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewPage());

			return links;
		}
	}
}
