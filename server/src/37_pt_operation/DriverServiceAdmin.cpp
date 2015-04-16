
//////////////////////////////////////////////////////////////////////////
/// DriverServiceAdmin class implementation.
///	@file DriverServiceAdmin.cpp
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

#include "DriverServiceAdmin.hpp"

#include "AdminParametersException.h"
#include "BaseCalendarAdmin.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "DriverService.hpp"
#include "HTMLTable.h"
#include "SchedulesBasedService.h"
#include "DRTAreaAdmin.hpp"
#include "DRTArea.hpp"
#include "Depot.hpp"
#include "ScheduledService.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "AdminFunctionRequest.hpp"
#include "ServiceAdmin.h"
#include "DeadRun.hpp"
#include "NamedPlace.h"
#include "DriverServiceUpdateAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "ImportableAdmin.hpp"
#include "VehicleServiceAdmin.hpp"
#include "VehicleService.hpp"
#include "DriverServicesAdmin.hpp"

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
	using namespace pt;
	using namespace graph;
	using namespace geography;
	using namespace calendar;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DriverServiceAdmin>::FACTORY_KEY("DriverServiceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DriverServiceAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<DriverServiceAdmin>::DEFAULT_TITLE("Service agent");
	}

	namespace pt_operation
	{
		const string DriverServiceAdmin::TAB_CALENDAR("calendar");
		const string DriverServiceAdmin::TAB_SERVICES("services");
		const string DriverServiceAdmin::TAB_DATASOURCE_LINKS("datasource_links");



		DriverServiceAdmin::DriverServiceAdmin()
			: AdminInterfaceElementTemplate<DriverServiceAdmin>()
		{ }



		void DriverServiceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_driverService = Env::GetOfficialEnv().get<DriverService>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<DriverService>&)
			{
			}
		}



		ParametersMap DriverServiceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_driverService.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _driverService->getKey());
			}
			return m;
		}



		bool DriverServiceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		void DriverServiceAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if(openTabContent(stream, TAB_SERVICES))
			{
				stream << "<h1>Courses</h1>";

				AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);
				AdminFunctionRequest<VehicleServiceAdmin> openVehicleServiceRequest(request);

				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Pause");
				c.push_back("Course");
				c.push_back("Course");
				c.push_back("Début");
				c.push_back("Début");
				c.push_back("Fin");
				c.push_back("Fin");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				size_t rank(0);
				const Hub* lastHub(NULL);
				size_t chunkRank(0);
				time_duration lastTime(not_a_date_time);

				BOOST_FOREACH(const DriverService::Chunk& chunk, _driverService->getChunks())
				{
					stream << t.row();
					stream << t.col(8, string(), true);
					stream << "Tronçon " << chunkRank;
					if(chunk.vehicleService)
					{
						openVehicleServiceRequest.getPage()->setVehicleService(
							Env::GetOfficialEnv().getSPtr(chunk.vehicleService)
						);
						stream << " - " <<
							HTMLModule::getHTMLLink(
								openVehicleServiceRequest.getURL(),
								"Service voiture " + chunk.vehicleService->getCodeBySources()
							)
						;
					}
					++chunkRank;

					BOOST_FOREACH(const DriverService::Chunk::Element& es, chunk.elements)
					{
						SchedulesBasedService* service(es.service);

						stream << t.row();

						// Rank
						stream << t.col() << rank++;

						// Pause
						stream << t.col();
						if(!lastTime.is_not_a_date_time())
						{
							stream << service->getDepartureSchedule(false, es.startRank) - lastTime;
						}

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
						stream << service->getDepartureSchedule(false, es.startRank);

						// Origin place
						stream << t.col();
						{
							const Hub* hub(service->getPath()->getEdge(es.startRank)->getFromVertex()->getHub());
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

							if(es.startRank != 0)
							{
								stream << "<span title=\"Relève en ligne\">*</span>";
							}
						}

						// Destination place
						stream << t.col();
						{
							const Hub* hub(service->getPath()->getEdge(es.endRank)->getFromVertex()->getHub());
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

							if(es.endRank+1 != service->getPath()->getEdges().size())
							{
								stream << "<span title=\"Relève en ligne\">*</span>";
							}
						}

						// Destination time
						stream << t.col();
						stream << service->getArrivalSchedule(false, es.endRank);
						lastTime = service->getArrivalSchedule(false, es.endRank);

						// Remove
						//stream << t.col();
				}	}

				stream << t.close();

				stream << "<p class=\"info\">*= relève en ligne</p>";
			}

			////////////////////////////////////////////////////////////////////
			// CALENDAR TAB
			if(openTabContent(stream, TAB_CALENDAR))
			{
				// Update request
				AdminActionFunctionRequest<DriverServiceUpdateAction, DriverServiceAdmin> updateDateRequest(request, *this);
				updateDateRequest.getAction()->setDriverService(const_pointer_cast<DriverService>(_driverService));

				// Display
				BaseCalendarAdmin::Display(stream, *_driverService, updateDateRequest);
			}

			////////////////////////////////////////////////////////////////////
			// DATASOURCE LINKS TAB
			if(openTabContent(stream, TAB_DATASOURCE_LINKS))
			{
				// Update request
				StaticActionRequest<DriverServiceUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setDriverService(const_pointer_cast<DriverService>(_driverService));

				// Table
				ImportableAdmin::DisplayDataSourcesTab(stream, *_driverService, updateOnlyRequest);
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string DriverServiceAdmin::getTitle() const
		{
			return _driverService.get() ? _driverService->getCodeBySources() : DEFAULT_TITLE;
		}



		bool DriverServiceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _driverService == static_cast<const DriverServiceAdmin&>(other)._driverService;
		}



		void DriverServiceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Services", TAB_SERVICES, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Calendrier", TAB_CALENDAR, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Sources de données", TAB_DATASOURCE_LINKS, profile.isAuthorized<GlobalRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks DriverServiceAdmin::_getCurrentTreeBranch() const
		{
			boost::shared_ptr<DriverServicesAdmin> p(
				getNewPage<DriverServicesAdmin>()
			);
			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);

			return links;
		}
}	}
