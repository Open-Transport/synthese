
/** StopPointInaccessibilityCalendarAdmin class implementation.
	@file StopPointInaccessibilityCalendarAdmin.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "StopPointInaccessibilityCalendarAdmin.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "CalendarTemplateTableSync.h"
#include "GraphConstants.h"
#include "HTMLForm.h"
#include "ResultHTMLTable.h"
#include "ImportableAdmin.hpp"
#include "ObjectUpdateAction.hpp"
#include "Profile.h"
#include "StopPointAdmin.hpp"
#include "StopPointInaccessibilityCalendar.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace html;
	using namespace impex;
	using namespace pt;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, StopPointInaccessibilityCalendarAdmin>::FACTORY_KEY("StopPointInaccessibilityCalendarAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<StopPointInaccessibilityCalendarAdmin>::ICON("handicapped_access.png");
		template<> const string AdminInterfaceElementTemplate<StopPointInaccessibilityCalendarAdmin>::DEFAULT_TITLE("Période d'inaccessibilité");
	}

	namespace pt
	{
		const string StopPointInaccessibilityCalendarAdmin::TAB_PROPERTIES("pr");

		StopPointInaccessibilityCalendarAdmin::StopPointInaccessibilityCalendarAdmin()
			: AdminInterfaceElementTemplate<StopPointInaccessibilityCalendarAdmin>()
		{}



		void StopPointInaccessibilityCalendarAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_stopPointInaccessibilityCalendar = Env::GetOfficialEnv().get<StopPointInaccessibilityCalendar>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<StopPointInaccessibilityCalendar>&)
			{
				throw AdminParametersException("No such inaccessibility calendar");
			}
		}



		ParametersMap StopPointInaccessibilityCalendarAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_stopPointInaccessibilityCalendar.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _stopPointInaccessibilityCalendar->getKey());
			}
			return m;
		}



		bool StopPointInaccessibilityCalendarAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void StopPointInaccessibilityCalendarAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				AdminActionFunctionRequest<ObjectUpdateAction, StopPointInaccessibilityCalendarAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setObject(*_stopPointInaccessibilityCalendar);

				AdminFunctionRequest<StopPointAdmin> openRequest(request);
				openRequest.getPage()->setStop(
					Env::GetOfficialEnv().getSPtr(&(*_stopPointInaccessibilityCalendar->get<StopPointer>()))
				);
				openRequest.getPage()->setActiveTab(StopPointAdmin::TAB_INACCESSIBILITY);

				HTMLForm updateForm(updateRequest.getHTMLForm("modify"));

				map<boost::optional<graph::UserClassCode>, string> userClassMap;
				userClassMap.insert(make_pair(0, "Toutes"));
				userClassMap.insert(make_pair(graph::USER_PEDESTRIAN, "Piétons"));
				userClassMap.insert(make_pair(graph::USER_HANDICAPPED, "PMR"));
				userClassMap.insert(make_pair(graph::USER_BIKE, "Vélos"));
				userClassMap.insert(make_pair(graph::USER_CAR, "Voitures"));

				map<boost::optional<string>, string> timeMap;
				timeMap.insert(make_pair(string(), string("-")));
				for(int i = 0 ; i < 1440 ; i+=30)
				{
					stringstream timeString;
					timeString << std::setw(2) << std::setfill('0') << int(i / 60) << ":";
					timeString << std::setw(2) << std::setfill('0') << int(i % 60);
					timeMap.insert(make_pair(timeString.str(), timeString.str()));
				}

				HTMLTable::ColsVector columns;
				columns.push_back("Arrêt");
				columns.push_back("Calendrier");
				columns.push_back("Classes d'utilisateur");
				columns.push_back("Heure de début");
				columns.push_back("Heure de fin");
				columns.push_back("Filtre horaire quotidien");
				columns.push_back(string());
				HTMLTable tableUpdate(columns, ResultHTMLTable::CSS_CLASS);

				stream << "<h1>Détails</h1>";
				stream << updateForm.open();
				stream << tableUpdate.open();

				stream << tableUpdate.row();

				stream << tableUpdate.col();
				stream << HTMLModule::getLinkButton(
					openRequest.getURL(),
					_stopPointInaccessibilityCalendar->get<StopPointer>()->getName(),
					string(),
					"/admin/img/" + StopPointAdmin::ICON
				);

				stream << tableUpdate.col() << updateForm.getSelectInput(
					ObjectUpdateAction::GetInputName<CalendarTemplatePointer>(),
					calendar::CalendarTemplateTableSync::GetCalendarTemplatesList(),
					boost::optional<RegistryKeyType>(_stopPointInaccessibilityCalendar->get<CalendarTemplatePointer>()->getKey())
				);

				stream << tableUpdate.col() << updateForm.getSelectInput(
					ObjectUpdateAction::GetInputName<UserClass>(),
					userClassMap,
					boost::optional<graph::UserClassCode>(_stopPointInaccessibilityCalendar->get<UserClass>())
				);

				stream << tableUpdate.col() << updateForm.getSelectInput(
					ObjectUpdateAction::GetInputName<StartHour>(),
					timeMap,
					boost::optional<string>(boost::posix_time::to_simple_string(_stopPointInaccessibilityCalendar->get<StartHour>()).substr(0, 5))
				);

				stream << tableUpdate.col() << updateForm.getSelectInput(
					ObjectUpdateAction::GetInputName<EndHour>(),
					timeMap,
					boost::optional<string>(boost::posix_time::to_simple_string(_stopPointInaccessibilityCalendar->get<EndHour>()).substr(0, 5))
				);

				stream << tableUpdate.col() << updateForm.getOuiNonRadioInput(
					ObjectUpdateAction::GetInputName<DailyTimeFilter>(),
					_stopPointInaccessibilityCalendar->get<DailyTimeFilter>()
				);
				stream << tableUpdate.col() << updateForm.getSubmitButton("Enregistrer");

				stream << tableUpdate.close();
				stream << updateForm.close();

				StaticActionRequest<ObjectUpdateAction> dataSourceUpdateRequest(request);
				dataSourceUpdateRequest.getAction()->setObject(*_stopPointInaccessibilityCalendar);
				ImportableAdmin::DisplayDataSourcesTab(stream, *_stopPointInaccessibilityCalendar, dataSourceUpdateRequest);
			}

			closeTabContent(stream);
		}



		std::string StopPointInaccessibilityCalendarAdmin::getTitle() const
		{
			return
				_stopPointInaccessibilityCalendar.get() ?
				"Période d'inaccessibilité " +
				(_stopPointInaccessibilityCalendar->get<StopPointer>()->getName().empty() ?
					boost::lexical_cast<string>(_stopPointInaccessibilityCalendar->get<StopPointer>()->getKey()) :
					_stopPointInaccessibilityCalendar->get<StopPointer>()->getName()) :
				DEFAULT_TITLE
			;
		}



		bool StopPointInaccessibilityCalendarAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return (_stopPointInaccessibilityCalendar->getKey() ==
					static_cast<const StopPointInaccessibilityCalendarAdmin&>(other)._stopPointInaccessibilityCalendar->getKey()
			);
		}



		void StopPointInaccessibilityCalendarAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabBuilded = true;
		}
	}
}
