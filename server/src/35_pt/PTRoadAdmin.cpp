
//////////////////////////////////////////////////////////////////////////
/// PTRoadAdmin class implementation.
///	@file PTRoadAdmin.cpp
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

#include "PTRoadAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "RoadPath.hpp"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Road.h"
#include "RoadPlace.h"
#include "ResultHTMLTable.h"
#include "Crossing.h"
#include "PublicPlace.h"
#include "StopArea.hpp"
#include "Edge.h"
#include "Vertex.h"
#include "City.h"
#include "HTMLModule.h"
#include "AdminFunctionRequest.hpp"
#include "PTRoadsAdmin.h"
#include "PTPlaceAdmin.h"
#include "Profile.h"

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
			const Request& request
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
				AdminFunctionRequest<PTRoadAdmin> openRoadRequest(request, *this);
				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

				HTMLTable::ColsVector c;
				c.push_back("Rang");
				c.push_back("PM");
				c.push_back("Type");
				c.push_back("Nom");
				c.push_back("Liens");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const Edge* edge, _road->getForwardPath().getEdges())
				{
					const Crossing& crossing(static_cast<const Crossing&>(*edge->getFromVertex()->getHub()));

					stream << t.row();
					stream << t.col();
					stream << edge->getRankInPath();
					stream << t.col();
					stream << edge->getMetricOffset();
					stream << t.col(2);
					stream << "Intersection de routes";

					stream << t.col();
					set<const Road*> roads;
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, crossing.getDepartureEdges())
					{
						roads.insert(dynamic_cast<const RoadPath*>(edge.second->getParentPath())->getRoad());
					}
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, crossing.getArrivalEdges())
					{
						roads.insert(dynamic_cast<const RoadPath*>(edge.second->getParentPath())->getRoad());
					}
					BOOST_FOREACH(const Road* road, roads)
					{
						if(road->getKey() <= 0 || road == _road.get()) continue;
						openRoadRequest.getPage()->setRoad(Env::GetOfficialEnv().getSPtr(road));
						stream << HTMLModule::getHTMLLink(openRoadRequest.getURL(), road->getAnyRoadPlace()->getName()) << " ";
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
			return _road.get() ? _road->getAnyRoadPlace()->getFullName() : DEFAULT_TITLE;
		}



		void PTRoadAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			_tabs.push_back(Tab("Carte", TAB_MAP, true));
			_tabs.push_back(Tab("Détail", TAB_DETAIL, true));
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
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

			boost::shared_ptr<PTRoadsAdmin> p(getNewPage<PTRoadsAdmin>());
			p->setRoadPlace(Env::GetOfficialEnv().getSPtr(
				&*_road->getAnyRoadPlace()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewCopiedPage());

			return links;
		}
	}
}
