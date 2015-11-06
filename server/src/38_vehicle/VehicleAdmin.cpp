
//////////////////////////////////////////////////////////////////////////
/// VehicleAdmin class implementation.
///	@file VehicleAdmin.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "VehicleAdmin.hpp"

#include "AdminParametersException.h"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "VehicleModule.hpp"
#include "User.h"
#include "Vehicle.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "VehicleTableSync.hpp"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "VehiclePositionTableSync.hpp"
#include "ActionResultHTMLTable.h"
#include "VehiclePositionUpdateAction.hpp"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "RemoveObjectAction.hpp"
#include "DepotTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace pt_operation;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace html;
	using namespace db;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehicleAdmin>::FACTORY_KEY("VehicleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehicleAdmin>::DEFAULT_TITLE("Vehicule");
	}

	namespace vehicle
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
			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_START_DATE, 500, false);
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
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<ObjectUpdateAction, VehicleAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setObject(*_vehicle);

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update_form"));
				stream << t.open();
				stream << t.cell("Nom", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Name>(), _vehicle->get<Name>()));
				stream << t.cell("Numéro", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Number>(), _vehicle->get<Number>()));
				stream << t.cell("Image", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Picture>(), _vehicle->get<Picture>()));
				stream << t.cell("Places", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Seats>(), Seats::ToString(_vehicle->get<Seats>())));
				stream << t.cell("Lignes autorisées", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<AllowedLines>(), AllowedLines::ToString(_vehicle->get<AllowedLines>())));
				stream << t.cell("En service", t.getForm().getOuiNonRadioInput(ObjectUpdateAction::GetInputName<Available>(), _vehicle->get<Available>()));
				stream << t.cell("URL", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<URL>(), _vehicle->get<URL>()));
				stream << t.cell("Immatriculation", t.getForm().getTextInput(ObjectUpdateAction::GetInputName<RegistrationNumber>(), _vehicle->get<RegistrationNumber>()));
				if(!_vehicle->get<URL>().empty())
				{
					stream << t.cell("Lien", HTMLModule::getHTMLLink(_vehicle->get<URL>(), _vehicle->get<URL>(), string(), false, string(), string(), "target=\"_new\""));
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				stream << "<h1>Recherche</h1>";

				ptime now(second_clock::local_time());
				AdminFunctionRequest<VehicleAdmin> searchRequest(request, *this);
				SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
				stream << s.open();
				stream << s.cell("Date début", s.getForm().getCalendarInput(PARAMETER_SEARCH_START_DATE, _searchStartDate ? *_searchStartDate : now));
				stream << s.cell("Date fin", s.getForm().getCalendarInput(PARAMETER_SEARCH_END_DATE, _searchEndDate ? *_searchEndDate : now));
				stream << s.close();

				stream << "<h1>Résultats</h1>";

				AdminActionFunctionRequest<VehiclePositionUpdateAction,VehicleAdmin> addRequest(request, *this);
				addRequest.getAction()->setVehicle(const_pointer_cast<Vehicle>(_vehicle).get());

				AdminActionFunctionRequest<RemoveObjectAction, VehicleAdmin> removeRequest(request, *this);

				// Search
				VehiclePositionTableSync::SearchResult positions(
					VehiclePositionTableSync::Search(
					_getEnv(),
					_vehicle->getKey(),
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
				v.push_back(make_pair(string(), string("Dst")));
				v.push_back(make_pair(string(), string("Statut")));
				v.push_back(make_pair(string(), string("Lieu")));
				v.push_back(make_pair(string(), string("Passagers")));
				v.push_back(make_pair(string(), string("Commentaire")));
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
				VehiclePosition::Meters meters(0);
				VehiclePosition::Meters lastMeters(0);
				Depot* lastDepot(NULL);
				StopPoint* lastStopPoint(NULL);
				DepotTableSync::DepotsList placesList(
					DepotTableSync::GetDepotsList(*_env, optional<string>("(pas de lieu)"))
				);
				set<StopPoint*> knownPlaces;
				BOOST_FOREACH(const boost::shared_ptr<VehiclePosition>& position, positions)
				{
					removeRequest.getAction()->setObjectId(position->getKey());

					stream << t.row(lexical_cast<string>(position->getKey()));
					stream << t.col() << rank++;
					stream << t.col() << position->getTime();

					// Meters
					stream << t.col() << position->getMeterOffset();
					if(lastMeters > 0)
					{
						if(	(position->getMeterOffset() < lastMeters && _requestParameters.raisingOrder) ||
							(position->getMeterOffset() > lastMeters && !_requestParameters.raisingOrder)
						){
							std::string title("Inversion d'ordre des mètres");
							stream << HTMLModule::getHTMLImage("/admin/img/exclamation.png", "!", title);
						}
						if(	((position->getMeterOffset() != lastMeters &&
							  (	position->getDepot() && position->getDepot() == lastDepot)) ||
								(position->getStopPoint() && position->getStopPoint() == lastStopPoint)
						)	){
							std::string title("Mouvement probablement manquant");
							stream << HTMLModule::getHTMLImage("/admin/img/error.png", "!", title);
						}
					}

					lastMeters = position->getMeterOffset();

					// Status
					stream << t.col() << VehiclePosition::GetStatusName(position->getStatus());

					// Place
					stream << t.col();
					if(position->getStopPoint() && position->getStopPoint()->getConnectionPlace())
					{
						stream << position->getStopPoint()->getConnectionPlace()->getFullName();
						lastStopPoint = position->getStopPoint();
						lastDepot = NULL;
						if(knownPlaces.find(lastStopPoint) == knownPlaces.end())
						{
							knownPlaces.insert(lastStopPoint);
							placesList.push_back(make_pair(lastStopPoint->getKey(), lastStopPoint->getConnectionPlace()->getFullName()));
						}
					}
					else if(position->getDepot())
					{
						stream << "Dépôt " << position->getDepot()->getName();
						lastDepot = position->getDepot();
						lastStopPoint = NULL;
					}
					else
					{
						lastStopPoint = NULL;
						lastDepot = NULL;
					}

					stream << t.col() << position->getPassengers();
					stream << t.col() << position->getComment();
					stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer cette entrée du journal ?");

					if(position->getMeterOffset() > meters)
					{
						meters = position->getMeterOffset();
					}
				}

				stream << t.row(string());
				stream << t.col() << rank;
				stream << t.col() << t.getActionForm().getCalendarInput(VehiclePositionUpdateAction::PARAMETER_TIME, now);
				stream << t.col() << t.getActionForm().getTextInput(VehiclePositionUpdateAction::PARAMETER_METER_OFFSET, lexical_cast<string>(meters));
				stream << t.col() << t.getActionForm().getSelectInput(VehiclePositionUpdateAction::PARAMETER_STATUS, VehiclePosition::GetStatusList(), optional<VehiclePositionStatusEnum>());
				stream << t.col() << t.getActionForm().getSelectInput(VehiclePositionUpdateAction::PARAMETER_STOP_POINT_ID, placesList, optional<RegistryKeyType>(0));
				stream << t.col() << t.getActionForm().getSelectNumberInput(VehiclePositionUpdateAction::PARAMETER_PASSENGERS, 0, 99);
				stream << t.col() << t.getActionForm().getTextInput(VehiclePositionUpdateAction::PARAMETER_COMMENT, string());
				stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
				stream << t.close();


			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string VehicleAdmin::getTitle() const
		{
			return _vehicle.get() ? (_vehicle->get<Name>() + " ("+ _vehicle->get<Number>() + ")") : DEFAULT_TITLE;
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
}	}


