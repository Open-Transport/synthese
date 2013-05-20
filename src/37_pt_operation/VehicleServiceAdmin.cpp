
//////////////////////////////////////////////////////////////////////////
/// VehicleServiceAdmin class implementation.
///	@file VehicleServiceAdmin.cpp
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

#include "VehicleServiceAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "GlobalRight.h"
#include "VehicleService.hpp"
#include "DeadRun.hpp"
#include "ResultHTMLTable.h"
#include "DRTArea.hpp"
#include "Depot.hpp"
#include "ScheduledService.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "NamedPlace.h"
#include "DRTAreaAdmin.hpp"
#include "VehicleServicesAdmin.hpp"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "ServiceAdmin.h"
#include "DriverService.hpp"
#include "DriverServiceAdmin.hpp"
#include "ImportableAdmin.hpp"
#include "StaticActionRequest.h"
#include "VehicleServiceUpdateAction.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace pt;
	using namespace graph;
	using namespace html;
	using namespace geography;
	using namespace impex;



	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehicleServiceAdmin>::FACTORY_KEY("VehicleServiceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehicleServiceAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehicleServiceAdmin>::DEFAULT_TITLE("Service voiture");
	}

	namespace pt_operation
	{
		const string VehicleServiceAdmin::TAB_ROUTE("route");
		const string VehicleServiceAdmin::TAB_DRIVER_SERVICES("driver_services");

		const string VehicleServiceAdmin::PARAMETER_DATE("date");



		VehicleServiceAdmin::VehicleServiceAdmin()
			: AdminInterfaceElementTemplate<VehicleServiceAdmin>()
		{}



		void VehicleServiceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Service
			try
			{
				_vehicleService = Env::GetOfficialEnv().get<VehicleService>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch (ObjectNotFoundException<VehicleService>&)
			{
				throw AdminParametersException("No such vehicle service");
			}

			// Date
			string dateStr(map.getDefault<string>(PARAMETER_DATE));
			if(!dateStr.empty())
			{
				_date = from_string(dateStr);
			}
		}



		ParametersMap VehicleServiceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_vehicleService.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _vehicleService->getKey());
			}
			return m;
		}



		bool VehicleServiceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void VehicleServiceAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// ROUTE TAB
			if (openTabContent(stream, TAB_ROUTE))
			{
				stream << "<h1>Date</h1>";

				AdminFunctionRequest<VehicleServiceAdmin> openRequest(request, *this);
				SearchFormHTMLTable f(openRequest.getHTMLForm("date"));
				stream << f.open();
				stream << f.cell("Date (vide = pas de filtre)", f.getForm().getCalendarInput(PARAMETER_DATE, _date));
				stream << f.close();

				AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);

				stream << "<h1>Itinéraire</h1>";
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Course");
				c.push_back("Course");
				c.push_back("Origine");
				c.push_back("Origine");
				c.push_back("Destination");
				c.push_back("Destination");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				size_t rank(0);
				const Hub* lastHub(NULL);
				BOOST_FOREACH(SchedulesBasedService* service, _vehicleService->getServices())
				{
					if(!_date.is_not_a_date() && !service->isActive(_date))
					{
						continue;
					}

					stream << t.row();

					// Rank
					stream << t.col() << rank++;

					// Line // Service
					if(dynamic_cast<ScheduledService*>(service))
					{
						stream << t.col();
						const CommercialLine& line(*static_cast<ScheduledService*>(service)->getRoute()->getCommercialLine());
						stream << "<span class=\"line " << line.getStyle() << "\">" << line.getShortName() << "</span>";

						openServiceRequest.getPage()->setService(Env::GetOfficialEnv().getSPtr(static_cast<ScheduledService*>(service)));
						stream << t.col() << HTMLModule::getHTMLLink(openServiceRequest.getURL(), service->getServiceNumber());
					}
					else if(dynamic_cast<DeadRun*>(service))
					{
						stream << t.col(2);
						stream << "HLP";
					}

					// Origin time
					stream << t.col();
					stream << service->getDepartureSchedule(false, 0);

					// Origin place
					stream << t.col();
					{
						const Hub* hub(service->getPath()->getEdge(0)->getFromVertex()->getHub());
						if(lastHub && hub != lastHub)
						{
							stream << HTMLModule::getHTMLImage("/admin/img/error.png", "Le lieu de départ est différent de la précédente arrivée");
						}
						if(dynamic_cast<const Depot*>(hub))
						{
							stream << "Dépôt " << static_cast<const Depot*>(hub)->getName();
						}
						else if(dynamic_cast<const NamedPlace*>(hub))
						{
							stream << dynamic_cast<const NamedPlace*>(hub)->getFullName();
						}
						else if(dynamic_cast<const DRTArea*>(hub))
						{
							stream << HTMLModule::getHTMLImage("/admin/img/"+DRTAreaAdmin::ICON, "Zone TAD");

							stream << dynamic_cast<const DRTArea*>(hub)->getName();
						}
					}

					// Destination place
					stream << t.col();
					{
						const Hub* hub(service->getPath()->getLastEdge()->getFromVertex()->getHub());
						lastHub = hub;
						if(dynamic_cast<const Depot*>(hub))
						{
							stream << "Dépôt " << static_cast<const Depot*>(hub)->getName();
						}
						else if(dynamic_cast<const NamedPlace*>(hub))
						{
							stream << dynamic_cast<const NamedPlace*>(hub)->getFullName();
						}
						else if(dynamic_cast<const DRTArea*>(hub))
						{
							stream << HTMLModule::getHTMLImage("/admin/img/"+DRTAreaAdmin::ICON, "Zone TAD");

							stream << dynamic_cast<const DRTArea*>(hub)->getName();
						}
					}

					// Destination time
					stream << t.col();
					stream << service->getArrivalSchedule(false, service->getPath()->getEdges().size()-1);

					// Remove
					stream << t.col();
					// TODO
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// DRIVER SERVICES TAB
			if (openTabContent(stream, TAB_DRIVER_SERVICES))
			{
				stream << "<h1>Date</h1>";

				AdminFunctionRequest<VehicleServiceAdmin> openRequest(request, *this);
				SearchFormHTMLTable f(openRequest.getHTMLForm("date"));
				stream << f.open();
				stream << f.cell("Date (vide = pas de filtre)", f.getForm().getCalendarInput(PARAMETER_DATE, _date));
				stream << f.close();

				AdminFunctionRequest<DriverServiceAdmin> openServiceRequest(request);

				stream << "<h1>Services agents</h1>";
				HTMLTable::ColsVector c;
				c.push_back("Heure début");
				c.push_back("Heure fin");
				c.push_back("Nb");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const DriverService::Chunk* chunk, _vehicleService->getDriverServiceChunks())
				{
					// Date filter
					if(!_date.is_not_a_date() && !chunk->driverService->isActive(_date))
					{
						continue;
					}

					openServiceRequest.getPage()->setDriverService(
						Env::GetOfficialEnv().getSPtr(chunk->driverService)
					);

					const DriverService::Chunk::Element& firstService(*chunk->elements.begin());
					const DriverService::Chunk::Element& lastService(*chunk->elements.rbegin());

					stream << t.row();
					stream << t.col();
					stream << firstService.service->getDepartureSchedule(false, firstService.startRank);

					stream << t.col();
					stream << lastService.service->getArrivalSchedule(false, lastService.endRank);

					stream << t.col();
					stream << chunk->elements.size();

					stream << t.col();
					stream << HTMLModule::getLinkButton(openServiceRequest.getURL(), "Ouvrir", string(), "/admin/img/" + DriverServiceAdmin::ICON);
				}

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// OPERATOR CODES TAB
			if (openTabContent(stream, ImportableAdmin::TAB_DATA_SOURCES))
			{
				StaticActionRequest<VehicleServiceUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setVehicleService(const_pointer_cast<VehicleService>(_vehicleService));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_vehicleService, updateOnlyRequest);
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}





		std::string VehicleServiceAdmin::getTitle() const
		{
			return
				_vehicleService.get() ?
					_vehicleService->getName().empty() ?
					lexical_cast<string>(_vehicleService->getKey()) :
				_vehicleService->getName() :
				DEFAULT_TITLE;
		}



		bool VehicleServiceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _vehicleService == static_cast<const VehicleServiceAdmin&>(other)._vehicleService;
		}



		void VehicleServiceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Itinéraire", TAB_ROUTE, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Services agents", TAB_DRIVER_SERVICES, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Sources de données", ImportableAdmin::TAB_DATA_SOURCES, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks VehicleServiceAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			boost::shared_ptr<VehicleServicesAdmin> p(
				getNewPage<VehicleServicesAdmin>()
			);
			links.push_back(p);
			return links;
		}
}	}
