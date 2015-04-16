////////////////////////////////////////////////////////////////////////////////
/// BroadcastPointsAdmin class implementation.
///	@file BroadcastPointsAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "BroadcastPointsAdmin.h"

#include "DisplaySearchAdmin.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "SearchFormHTMLTable.h"
#include "ResultHTMLTable.h"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "StopArea.hpp"
#include "PTModule.h"
#include "StaticFunctionRequest.h"
#include "Session.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "User.h"
#include "Profile.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "DisplayAdmin.h"
#include "DisplayScreenCPUAdmin.h"

#include <map>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace html;
	using namespace departure_boards;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BroadcastPointsAdmin>::FACTORY_KEY("1broadcastpoints");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BroadcastPointsAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<BroadcastPointsAdmin>::DEFAULT_TITLE("Lieux de diffusion");
	}

	namespace departure_boards
	{
		const string BroadcastPointsAdmin::PARAMETER_DEVICES_NUMBER("cn");
		const string BroadcastPointsAdmin::PARAMETER_CITY_NAME = "city";
		const string BroadcastPointsAdmin::PARAMETER_PLACE_NAME = "place";
		const string BroadcastPointsAdmin::PARAMETER_LINE_ID = "line";

		BroadcastPointsAdmin::BroadcastPointsAdmin():
			AdminInterfaceElementTemplate<BroadcastPointsAdmin>(),
			_searchDevicesNumber(WITH_OR_WITHOUT_ANY_BROADCASTPOINT)
		{}

		void BroadcastPointsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_cityName = map.getDefault<string>(PARAMETER_CITY_NAME);
			_placeName = map.getDefault<string>(PARAMETER_PLACE_NAME);

			optional<int> i(map.getOptional<int>(PARAMETER_DEVICES_NUMBER));
			if (i)	_searchDevicesNumber = static_cast<BroadcastPointsPresence>(*i);

			_lineUId = map.getOptional<RegistryKeyType>(PARAMETER_LINE_ID);

			_requestParameters.setFromParametersMap(map, PARAMETER_CITY_NAME, 30);
		}



		util::ParametersMap BroadcastPointsAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_CITY_NAME, _cityName);
			m.insert(PARAMETER_PLACE_NAME, _placeName);
			m.insert(PARAMETER_DEVICES_NUMBER, static_cast<int>(_searchDevicesNumber));
			if(_lineUId)
			{
				m.insert(PARAMETER_LINE_ID, *_lineUId);
			}
			return m;
		}



		void BroadcastPointsAdmin::display(
			ostream& stream,
			const server::Request& _request) const
		{
			vector<pair<optional<int>, string> > m;
			m.push_back(make_pair((int) WITH_OR_WITHOUT_ANY_BROADCASTPOINT, "(filtre désactivé)"));
			m.push_back(make_pair((int) AT_LEAST_ONE_BROADCASTPOINT, "Au moins un"));
			m.push_back(make_pair((int) NO_BROADCASTPOINT, "Aucun"));

			stream << "<h1>Recherche</h1>";

			AdminFunctionRequest<BroadcastPointsAdmin> searchRequest(_request, *this);
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune", st.getForm().getTextInput(PARAMETER_CITY_NAME, _cityName));
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_PLACE_NAME, _placeName));
			stream << st.cell(
				"Equipements",
				st.getForm().getSelectInput(
					PARAMETER_DEVICES_NUMBER,
					m,
					optional<int>(static_cast<int>(_searchDevicesNumber))
			)	);
			stream << st.cell("Ligne", st.getForm().getSelectInput(
					PARAMETER_LINE_ID,
					PTModule::getCommercialLineLabels(
						_request.getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>(),
						_request.getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
						, READ
						, true
					),
					_lineUId
			)	);
			stream << st.close();

			stream << "<h1>Résultats de la recherche</h1>";

			std::vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchResult(
				searchConnectionPlacesWithBroadcastPoints(
					_getEnv(),
					_request.getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
					, _request.getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
					, READ
					, _cityName
					, _placeName
					, _searchDevicesNumber,
					_lineUId
					, _requestParameters.maxSize
					, _requestParameters.first
					, _requestParameters.orderField == PARAMETER_CITY_NAME
					, _requestParameters.orderField == PARAMETER_PLACE_NAME
					, _requestParameters.orderField == PARAMETER_DEVICES_NUMBER
					, _requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_CITY_NAME, "Commune"));
			h.push_back(make_pair(PARAMETER_PLACE_NAME, "Nom zone d'arrêt"));
			h.push_back(make_pair(PARAMETER_DEVICES_NUMBER, "Equipements"));
			h.push_back(make_pair(PARAMETER_DEVICES_NUMBER, "Equipements"));
			h.push_back(make_pair(string(), "Actions"));

			ResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				_requestParameters,
				searchResult
			);

			stream << t.open();
			AdminFunctionRequest<DisplaySearchAdmin> goRequest(_request);
			BOOST_FOREACH(const boost::shared_ptr<ConnectionPlaceWithBroadcastPoint>& pl, searchResult)
			{
				stream << t.row();
				try
				{
					stream << t.col() << pl->cityName;
					stream << t.col() << pl->place->getName();

					if(pl->cpuNumber == 0 && pl->broadCastPointsNumber == 0)
					{
						stream << t.col(2) << "aucun";
					}
					else
					{
						stream << t.col();
						if(pl->cpuNumber > 0)
						{
							stream <<
								HTMLModule::getHTMLImage("/admin/img/" + DisplayScreenCPUAdmin::ICON, "unité centrale") <<
								"x" << pl->cpuNumber
							;
						}
						stream << t.col();
						if(pl->broadCastPointsNumber > 0)
						{
							stream <<
								HTMLModule::getHTMLImage("/admin/img/" + DisplayAdmin::ICON, "écran") <<
								"x" << pl->broadCastPointsNumber
							;
						}
					}

					HTMLForm gf(goRequest.getHTMLForm());
					gf.addHiddenField(DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID, lexical_cast<string>(pl->place->getKey()));
					stream << t.col() << gf.getLinkButton("Ouvrir", string(), "/admin/img/building_edit.png");
				}
				catch (...)
				{
					stream << t.col(3) << HTMLModule::getHTMLImage("/admin/img/exclamation.png", "Erreur de données") << " Erreur de données : arrêt inexistant";
				}
			}
			stream << t.close();
		}



		bool BroadcastPointsAdmin::isAuthorized(
			const security::User& user
		) const {
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}



		AdminInterfaceElement::PageLinks BroadcastPointsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const DeparturesTableModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}


		AdminInterfaceElement::PageLinks BroadcastPointsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			const DisplaySearchAdmin* sa(
				dynamic_cast<const DisplaySearchAdmin*>(&currentPage)
			);

			if(	sa ||
				dynamic_cast<const DisplayScreenCPUAdmin*>(&currentPage) ||
				dynamic_cast<const BroadcastPointsAdmin*>(&currentPage) ||
				dynamic_cast<const DisplayAdmin*>(&currentPage)
			){
				vector<boost::shared_ptr<ConnectionPlaceWithBroadcastPoint> > searchResult(
					searchConnectionPlacesWithBroadcastPoints(
						_getEnv(),
						request.getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>(),
						request.getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ,
						READ,
						string(),
						string(),
						AT_LEAST_ONE_BROADCASTPOINT
				)	);

				bool currentToAdd(sa && sa->getPlace() && sa->getPlace()->get());
				BOOST_FOREACH(const boost::shared_ptr<ConnectionPlaceWithBroadcastPoint>& result, searchResult)
				{
					boost::shared_ptr<DisplaySearchAdmin> p(
						getNewPage<DisplaySearchAdmin>()
					);
					p->setPlace(result->place->getKey());
					links.push_back(p);
					if(*p == currentPage) currentToAdd = false;
				}

				if(currentToAdd)
				{
					boost::shared_ptr<DisplaySearchAdmin> p(
						getNewPage<DisplaySearchAdmin>()
					);
					p->setPlace((*sa->getPlace())->getKey());
					links.push_back(p);
			}	}

			return links;
		}



		bool BroadcastPointsAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			return true;
		}
	}
}
