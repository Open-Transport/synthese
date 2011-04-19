
//////////////////////////////////////////////////////////////////////////
/// VehicleAdmin class implementation.
///	@file VehicleAdmin.cpp
///	@author RCSobility
///	@date 2011
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

#include "VehicleAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTOperationModule.hpp"
#include "PropertiesHTMLTable.h"
#include "Vehicle.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehicleTableSync.hpp"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "VehiclePositionTableSync.hpp"
#include "ActionResultHTMLTable.h"
#include "VehiclePositionUpdateAction.hpp"
#include "StopPoint.hpp"
#include "StopArea.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehicleAdmin>::FACTORY_KEY("VehicleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::DEFAULT_TITLE("Vehicule");
	}

	namespace pt_operation
	{
		const string VehicleAdmin::TAB_LOG("lg");
		const string VehicleAdmin::TAB_PROPERTIES("pr");

		const string VehicleAdmin::PARAMETER_SEARCH_START_DATE("sd");
		const string VehicleAdmin::PARAMETER_SEARCH_END_DATE("ed");

		VehicleAdmin::VehicleAdmin()
			: AdminInterfaceElementTemplate<VehicleAdmin>()
		{ }


		
		void VehicleAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_vehicle = Env::GetOfficialEnv().get<Vehicle>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw AdminParametersException("No such vehicle");
			}

			// Start date
			string searchStartDateStr(map.getDefault<string>(PARAMETER_SEARCH_START_DATE));
			if(!searchStartDateStr.empty())
			{
				_searchStartDate = time_from_string(searchStartDateStr);
			}

			// End date
			string searchEndDateStr(map.getDefault<string>(PARAMETER_SEARCH_END_DATE));
			if(!searchEndDateStr.empty())
			{
				_searchEndDate = time_from_string(searchEndDateStr);
			}

			// Parameters
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_START_DATE, 100, false);
		}



		ParametersMap VehicleAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_vehicle.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _vehicle->getKey());
			}

			return m;
		}


		
		bool VehicleAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void VehicleAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<VehicleUpdateAction, VehicleAdmin> updateRequest(request);
				updateRequest.getAction()->setVehicle(const_pointer_cast<Vehicle>(_vehicle));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update_form"));
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_NAME, _vehicle->getName()));
				stream << t.cell("Numéro", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_NUMBER, _vehicle->getNumber()));
				stream << t.cell("Image", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_PICTURE, _vehicle->getPicture()));
				stream << t.cell("Places", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_SEATS, VehicleTableSync::SerializeSeats(_vehicle->getSeats())));
				stream << t.cell("Lignes autorisées", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_ALLOWED_LINES, VehicleTableSync::SerializeAllowedLines(_vehicle->getAllowedLines())));
				stream << t.cell("En service", t.getForm().getOuiNonRadioInput(VehicleUpdateAction::PARAMETER_AVAILABLE, _vehicle->getAvailable()));
				stream << t.cell("URL", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_URL, _vehicle->getURL()));
				stream << t.cell("Immatriculation", t.getForm().getTextInput(VehicleUpdateAction::PARAMETER_REGISTRATION_NUMBERS, _vehicle->getRegistrationNumbers()));
				if(!_vehicle->getURL().empty())
				{
					stream << t.cell("Lien", HTMLModule::getHTMLLink(_vehicle->getURL(), _vehicle->getURL(), string(), false, string(), string(), "target=\"_new\""));
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				stream << "<h1>Recherche</h1>";

				ptime now(second_clock::local_time());
				AdminFunctionRequest<VehicleAdmin> searchRequest(request);
				SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
				stream << s.open();
				stream << s.cell("Date début", s.getForm().getCalendarInput(PARAMETER_SEARCH_START_DATE, _searchStartDate ? *_searchStartDate : now));
				stream << s.cell("Date fin", s.getForm().getCalendarInput(PARAMETER_SEARCH_END_DATE, _searchEndDate ? *_searchEndDate : now));
				stream << s.close();
				
				stream << "<h1>Résultats</h1>";

				AdminActionFunctionRequest<VehiclePositionUpdateAction,VehicleAdmin> addRequest(request);
				addRequest.getAction()->setVehicle(const_pointer_cast<Vehicle>(_vehicle));

				// Search
				VehiclePositionTableSync::SearchResult positions(
					VehiclePositionTableSync::Search(
					_getEnv(),
					_searchStartDate,
					_searchEndDate,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_START_DATE,
					_requestParameters.raisingOrder
				)	);

				ActionResultHTMLTable::HeaderVector v;
				v.push_back(make_pair(string(), string("#")));
				v.push_back(make_pair(PARAMETER_SEARCH_START_DATE, string("Heure")));
				v.push_back(make_pair(string(), string("Statut")));
				v.push_back(make_pair(string(), string("Lieu")));
				v.push_back(make_pair(string(), string("Actions")));
				ActionResultHTMLTable t(
					v,
					searchRequest.getHTMLForm(),
					_requestParameters,
					positions,
					addRequest.getHTMLForm("add"),
					VehiclePositionUpdateAction::PARAMETER_BEFORE_ID
				);

				stream << t.open();
				size_t rank(0);
				BOOST_FOREACH(shared_ptr<VehiclePosition> position, positions)
				{
					stream << t.row(lexical_cast<string>(position->getKey()));
					stream << t.col() << rank++;
					stream << t.col() << position->getTime();
					stream << t.col();
					if(position->getStopPoint() && position->getStopPoint()->getConnectionPlace())
					{
						stream << position->getStopPoint()->getConnectionPlace()->getFullName();
					}
				}

				stream << t.row(string());
				stream << t.col() << rank;
				stream << t.col() << t.getActionForm().getCalendarInput(VehiclePositionUpdateAction::PARAMETER_TIME, now);
				stream << t.col();
				stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
				stream << t.close();


			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string VehicleAdmin::getTitle() const
		{
			return _vehicle.get() ? (_vehicle->getName() + " ("+ _vehicle->getNumber() + ")") : DEFAULT_TITLE;
		}



		bool VehicleAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _vehicle->getKey()  == static_cast<const VehicleAdmin&>(other)._vehicle->getKey();
		}



		void VehicleAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();
//			bool writeRight(profile.isAuthorized<ResaRight>(WRITE, UNKNOWN_RIGHT_LEVEL));

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true, "car.png"));
			_tabs.push_back(Tab("Journal", TAB_LOG, true, "book.png"));

			_tabBuilded = true;
		}
	}
}


