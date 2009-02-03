////////////////////////////////////////////////////////////////////////////////
/// BroadcastPointsAdmin class implementation.
///	@file BroadcastPointsAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "BroadcastPointsAdmin.h"
#include "DisplaySearchAdmin.h"
#include "ArrivalDepartureTableRight.h"
#include "DeparturesTableModule.h"
#include "SearchFormHTMLTable.h"
#include "ResultHTMLTable.h"
#include "ConnectionPlaceTableSync.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "EnvModule.h"
#include "FunctionRequest.h"
#include "Session.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"
#include "User.h"
#include "Profile.h"

#include <map>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace departurestable;
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

	namespace departurestable
	{
		const string BroadcastPointsAdmin::PARAMETER_CPU_NUMBER("cn");
		const string BroadcastPointsAdmin::PARAMETER_CITY_NAME = "city";
		const string BroadcastPointsAdmin::PARAMETER_PLACE_NAME = "place";
		const string BroadcastPointsAdmin::PARAMETER_LINE_ID = "line";
		const string BroadcastPointsAdmin::PARAMETER_DISPLAY_NUMBER = "dpln";
		
		BroadcastPointsAdmin::BroadcastPointsAdmin()
			: AdminInterfaceElementTemplate<BroadcastPointsAdmin>(),
			_displayNumber(WITH_OR_WITHOUT_ANY_BROADCASTPOINT),
			_cpuNumber(WITH_OR_WITHOUT_ANY_BROADCASTPOINT),
			_lineUId(UNKNOWN_VALUE)
		{}

		void BroadcastPointsAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_cityName = map.getString(PARAMETER_CITY_NAME, false, FACTORY_KEY);
			_placeName = map.getString(PARAMETER_PLACE_NAME, false, FACTORY_KEY);
			
			int i(map.getInt(PARAMETER_DISPLAY_NUMBER, false, FACTORY_KEY));
			if (i != UNKNOWN_VALUE)	_displayNumber = static_cast<BroadcastPointsPresence>(i);

			i = map.getInt(PARAMETER_CPU_NUMBER, false, FACTORY_KEY);
			if (i != UNKNOWN_VALUE) _cpuNumber = static_cast<BroadcastPointsPresence>(i);

			_lineUId = map.getUid(PARAMETER_LINE_ID, false, FACTORY_KEY);

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_CITY_NAME, 30);
			
			if(!doDisplayPreparationActions) return;
			
			_searchResult = searchConnectionPlacesWithBroadcastPoints(
				_env,
				_request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
				, _request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
				, READ
				, _cityName
				, _placeName
				, _displayNumber
				, _cpuNumber
				, _lineUId
				, _requestParameters.maxSize
				, _requestParameters.first
				, _requestParameters.orderField == PARAMETER_CITY_NAME
				, _requestParameters.orderField == PARAMETER_PLACE_NAME
				, _requestParameters.orderField == PARAMETER_DISPLAY_NUMBER
				, _requestParameters.orderField == PARAMETER_CPU_NUMBER
				, _requestParameters.raisingOrder
			);
			_resultParameters.setFromResult(_requestParameters, _searchResult);
		}
		
		
		
		server::ParametersMap BroadcastPointsAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_CITY_NAME, _cityName);
			m.insert(PARAMETER_PLACE_NAME, _placeName);
			m.insert(PARAMETER_DISPLAY_NUMBER, static_cast<int>(_displayNumber));
			m.insert(PARAMETER_CPU_NUMBER, static_cast<int>(_cpuNumber));
			m.insert(PARAMETER_LINE_ID, _lineUId);
			return m;
		}



		void BroadcastPointsAdmin::display(ostream& stream, interfaces::VariablesMap& variables) const
		{
			FunctionRequest<AdminRequest> goRequest(_request);
			goRequest.getFunction()->setPage<DisplaySearchAdmin>();

			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setSamePage(this);

			vector<pair<int, string> > m;
			m.push_back(make_pair((int) WITH_OR_WITHOUT_ANY_BROADCASTPOINT, "(filtre désactivé)"));
			m.push_back(make_pair((int) AT_LEAST_ONE_BROADCASTPOINT, "Au moins un"));
			m.push_back(make_pair((int) NO_BROADCASTPOINT, "Aucun"));

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune", st.getForm().getTextInput(PARAMETER_CITY_NAME, _cityName));
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_PLACE_NAME, _placeName));
			stream << st.cell(
				"Terminaux d'affichage",
				st.getForm().getSelectInput(PARAMETER_DISPLAY_NUMBER, m, static_cast<int>(_displayNumber))
			);
			stream << st.cell(
				"Unités centrales",
				st.getForm().getSelectInput(PARAMETER_CPU_NUMBER, m, static_cast<int>(_cpuNumber))
			);
			stream << st.cell("Ligne", st.getForm().getSelectInput(
				PARAMETER_LINE_ID,
				EnvModule::getCommercialLineLabels(
					_request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
					, _request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
					, READ
					, true
				),
				_lineUId)
			);
			stream << st.close();

			stream << "<h1>Résultats de la recherche</h1>";

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_CITY_NAME, "Commune"));
			h.push_back(make_pair(PARAMETER_PLACE_NAME, "Nom zone d'arrêt"));
			h.push_back(make_pair(PARAMETER_DISPLAY_NUMBER, "Afficheurs"));
			h.push_back(make_pair(PARAMETER_CPU_NUMBER, "Unités centrales"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,st.getForm(), _requestParameters, _resultParameters);

			stream << t.open();
			BOOST_FOREACH(shared_ptr<ConnectionPlaceWithBroadcastPoint> pl, _searchResult)
			{
				stream << t.row();
				try
				{
					stream << t.col() << pl->cityName;
					stream << t.col() << pl->place->getName();
					stream << t.col() << pl->broadCastPointsNumber;
					stream << t.col() << pl->cpuNumber;
					HTMLForm gf(goRequest.getHTMLForm());
					gf.addHiddenField(DisplaySearchAdmin::PARAMETER_SEARCH_LOCALIZATION_ID, Conversion::ToString(pl->place->getKey()));
					stream << t.col() << gf.getLinkButton("Editer", string(), "building_edit.png");
				}
				catch (...)
				{
					stream << t.col(3) << HTMLModule::getHTMLImage("exclamation.png", "Erreur de données") << " Erreur de données : arrêt inexistant";
				}
			}
			stream << t.close();
		}

		bool BroadcastPointsAdmin::isAuthorized() const
		{
			return _request->isAuthorized<ArrivalDepartureTableRight>(READ);
		}

		AdminInterfaceElement::PageLinks BroadcastPointsAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == DeparturesTableModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}

		bool BroadcastPointsAdmin::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return true;
		}
	}
}
