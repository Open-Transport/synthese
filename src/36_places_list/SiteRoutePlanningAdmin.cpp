
/** SiteSiteRoutePlanningAdmin class implementation.
	@file SiteRoutePlanningAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "SiteRoutePlanningAdmin.h"
#include "Profile.h"
#include "Line.h"
#include "PlacesListModule.h"
#include "Site.h"
#include "SiteTableSync.h"
#include "TransportSiteAdmin.h"
#include "TransportWebsiteRight.h"
#include "PTTimeSlotRoutePlanner.h"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "HTMLForm.h"
#include "PTRoutePlannerResult.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace ptrouteplanner;
	using namespace algorithm;
	using namespace time;
	using namespace html;
	using namespace env;
	using namespace security;
	using namespace graph;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, transportwebsite::SiteRoutePlanningAdmin>::FACTORY_KEY("SiteRoutePlanningAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::ICON("arrow_switch.png");
		template<> const string AdminInterfaceElementTemplate<transportwebsite::SiteRoutePlanningAdmin>::DEFAULT_TITLE("Calcul d'itinéraires");
	}

	namespace transportwebsite
	{
		const string SiteRoutePlanningAdmin::PARAMETER_DATE_TIME("dt");
		const string SiteRoutePlanningAdmin::PARAMETER_START_CITY("sc");
		const string SiteRoutePlanningAdmin::PARAMETER_START_PLACE("sp");
		const string SiteRoutePlanningAdmin::PARAMETER_END_CITY("ec");
		const string SiteRoutePlanningAdmin::PARAMETER_END_PLACE("ep");
		const string SiteRoutePlanningAdmin::PARAMETER_RESULTS_NUMBER("rn");
		const string SiteRoutePlanningAdmin::PARAMETER_ACCESSIBILITY("ac");
		const string SiteRoutePlanningAdmin::PARAMETER_LOG("lo");

		SiteRoutePlanningAdmin::SiteRoutePlanningAdmin()
			: AdminInterfaceElementTemplate<SiteRoutePlanningAdmin>()
			, _resultsNumber(UNKNOWN_VALUE)
			, _dateTime(TIME_UNKNOWN)
			, _log(false),
			_accessibility(USER_PEDESTRIAN)
		{ }


		
		void SiteRoutePlanningAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_startCity = map.getString(PARAMETER_START_CITY, false, FACTORY_KEY);
			_startPlace = map.getString(PARAMETER_START_PLACE, false, FACTORY_KEY);
			_endCity = map.getString(PARAMETER_END_CITY, false, FACTORY_KEY);
			_endPlace = map.getString(PARAMETER_END_PLACE, false, FACTORY_KEY);
			_dateTime = map.getDateTime(PARAMETER_DATE_TIME, false, FACTORY_KEY);
			_log = map.getBool(PARAMETER_LOG, false, false, FACTORY_KEY);
			if (_dateTime.isUnknown())
				_dateTime = DateTime(TIME_CURRENT);
			_resultsNumber = map.getInt(PARAMETER_RESULTS_NUMBER, false, FACTORY_KEY);
			_accessibility = static_cast<UserClassCode>(
				map.getInt(PARAMETER_ACCESSIBILITY, false, string())
			);
		
			try
			{
				_site = SiteTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv()
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
		}
		
		
		
		server::ParametersMap SiteRoutePlanningAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_START_CITY, _startCity);
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_CITY, _endCity);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			m.insert(PARAMETER_DATE_TIME, _dateTime);
			m.insert(PARAMETER_LOG, _log);
			m.insert(PARAMETER_RESULTS_NUMBER, _resultsNumber);
			m.insert(PARAMETER_ACCESSIBILITY, static_cast<int>(_accessibility));
			if(_site.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
			return m;
		}



		void SiteRoutePlanningAdmin::display(ostream& stream, VariablesMap& variables,
					const admin::AdminRequest& _request) const
		{
			AdminFunctionRequest<SiteRoutePlanningAdmin> searchRequest(_request);

			// Search form
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune départ", st.getForm().getTextInput(PARAMETER_START_CITY, _startCity));
			stream << st.cell("Arrêt départ", st.getForm().getTextInput(PARAMETER_START_PLACE, _startPlace));
			stream << st.cell("Commune arrivée", st.getForm().getTextInput(PARAMETER_END_CITY, _endCity));
			stream << st.cell("Arrêt arrivée", st.getForm().getTextInput(PARAMETER_END_PLACE, _endPlace));
			stream << st.cell("Date/Heure", st.getForm().getCalendarInput(PARAMETER_DATE_TIME, _dateTime.toPosixTime()));
			stream << st.cell("Nombre réponses", st.getForm().getSelectNumberInput(PARAMETER_RESULTS_NUMBER, 1, 99, _resultsNumber, 1, "(illimité)"));
			stream << st.cell("Accessibilité", st.getForm().getSelectInput(PARAMETER_ACCESSIBILITY, PlacesListModule::GetAccessibilityNames(), _accessibility));
			stream << st.cell("Trace", st.getForm().getOuiNonRadioInput(PARAMETER_LOG, _log));
			stream << st.close();

			// No calculation without cities
			if (_startCity.empty() || _endCity.empty())
				return;

			if (_log)
				stream << "<h1>Trace</h1>";

			DateTime endDate(_dateTime);
			endDate++;

			// Route planning
			const Place* startPlace(_site->fetchPlace(_startCity, _startPlace));
			const Place* endPlace(_site->fetchPlace(_endCity, _endPlace));
			PTTimeSlotRoutePlanner r(
				startPlace,
				endPlace,
				_dateTime,
				endDate,
				_dateTime,
				endDate,
				_resultsNumber,
				_site->getAccessParameters(_accessibility),
				DEPARTURE_FIRST
//				, &stream
//				, _log ? Log::LEVEL_TRACE : Log::LEVEL_NONE
			);
			const PTRoutePlannerResult jv(r.run());

			stream << "<h1>Résultats</h1>";

			jv.displayHTMLTable(stream, optional<HTMLForm&>(), string());
		}



		bool SiteRoutePlanningAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}
		
		
		boost::shared_ptr<const Site> SiteRoutePlanningAdmin::getSite() const
		{
			return _site;
		}
		
		void SiteRoutePlanningAdmin::setSite(boost::shared_ptr<const Site> value)
		{
			_site = value;
		}
		
		
		bool SiteRoutePlanningAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _site->getKey() == static_cast<const SiteRoutePlanningAdmin&>(other)._site->getKey();
		}
	}
}
