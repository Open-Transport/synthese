
//////////////////////////////////////////////////////////////////////////
/// PTRoadsAdmin class implementation.
///	@file PTRoadsAdmin.cpp
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

#include "PTRoadsAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "RoadPlace.h"
#include "ResultHTMLTable.h"
#include "Road.h"
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
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace road;
	using namespace html;

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
		PTRoadsAdmin::PTRoadsAdmin()
			: AdminInterfaceElementTemplate<PTRoadsAdmin>()
		{ }


		
		void PTRoadsAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
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
			const FunctionRequest<AdminRequest>& request
		) const	{
			return request.isAuthorized<TransportNetworkRight>(READ);
		}



		void PTRoadsAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const FunctionRequest<AdminRequest>& request
		) const	{
		
			AdminFunctionRequest<PTRoadAdmin> openRoadRequest(request);

			HTMLTable::ColsVector c;
			c.push_back("Début");
			c.push_back("Fin");
			c.push_back("Longueur");
			c.push_back("Actions");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			stream << t.open();
			BOOST_FOREACH(const Road* road, _roadPlace->getRoads())
			{
				openRoadRequest.getPage()->setRoad(
					Env::GetOfficialEnv().getSPtr(road)
				);

				stream << t.row();
				stream << t.col();

				stream << t.col();
				stream << t.col();
				stream << t.col();
				stream << HTMLModule::getLinkButton(openRoadRequest.getURL(), "Ouvrir", string(), PTRoadAdmin::ICON);
			}
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks PTRoadsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				BOOST_FOREACH(const Road* road, _roadPlace->getRoads())
				{
					shared_ptr<PTRoadAdmin> p(getNewOtherPage<PTRoadAdmin>());
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

			links.push_back(getNewOtherPage<PTCitiesAdmin>());
			shared_ptr<PTPlacesAdmin> p(getNewOtherPage<PTPlacesAdmin>());
			p->setCity(Env::GetOfficialEnv().getSPtr(_roadPlace->getCity()));
			links.push_back(p);
			links.push_back(getNewPage());

			return links;
		}



		std::string PTRoadsAdmin::getTitle() const
		{
			return _roadPlace.get() ? _roadPlace->getName() : DEFAULT_TITLE;
		}
	}
}
