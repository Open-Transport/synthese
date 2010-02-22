
//////////////////////////////////////////////////////////////////////////
/// ServiceAdmin class implementation.
///	@file ServiceAdmin.cpp
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

#include "ServiceAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "ScheduledServiceTableSync.h"
#include "ScheduledService.h"
#include "ContinuousServiceTableSync.h"
#include "ContinuousService.h"
#include "ResultHTMLTable.h"
#include "Line.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "HTMLForm.h"
#include "AdminActionFunctionRequest.hpp"
#include "ScheduleRealTimeUpdateAction.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "CalendarHTMLViewer.h"
#include "LineAdmin.h"
#include "Profile.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace env;
	using namespace graph;
	using namespace html;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ServiceAdmin>::FACTORY_KEY("ServiceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ServiceAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<ServiceAdmin>::DEFAULT_TITLE("Service");
	}

	namespace pt
	{
		const string ServiceAdmin::TAB_CALENDAR("ca");
		const string ServiceAdmin::TAB_CONTINUOUS("co");
		const string ServiceAdmin::TAB_SCHEDULES("sc");


		ServiceAdmin::ServiceAdmin()
			: AdminInterfaceElementTemplate<ServiceAdmin>()
		{ }


		
		void ServiceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			try
			{
				if(decodeTableId(id) == ScheduledServiceTableSync::TABLE.ID)
				{
					setService(Env::GetOfficialEnv().getRegistry<ScheduledService>().get(id));
				}
				else if(decodeTableId(id) == ContinuousServiceTableSync::TABLE.ID)
				{
					setService(Env::GetOfficialEnv().getRegistry<ContinuousService>().get(id));
				}
				else
				{
					throw AdminParametersException("Invalid ID");
				}

			}
			catch (ObjectNotFoundException<ScheduledService>)
			{
				throw AdminParametersException("No such scheduled service");
			}
			catch (ObjectNotFoundException<ContinuousService>)
			{
				throw AdminParametersException("No such continuous service");
			}
		}



		ParametersMap ServiceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_service.get())
				m.insert(Request::PARAMETER_OBJECT_ID, _service->getKey());
			return m;
		}


		
		bool ServiceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void ServiceAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{
		
			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULES
			if (openTabContent(stream, TAB_SCHEDULES))
			{
				AdminActionFunctionRequest<ScheduleRealTimeUpdateAction, ServiceAdmin> scheduleUpdateRequest(request);
				if(_scheduledService.get())
				{
					scheduleUpdateRequest.getPage()->setService(_scheduledService);
				}
				else
				{
					scheduleUpdateRequest.getPage()->setService(_continuousService);
				}
				scheduleUpdateRequest.getAction()->setService(_scheduledService);

				AdminActionFunctionRequest<ServiceVertexRealTimeUpdateAction, ServiceAdmin> vertexUpdateRequest(request);
				if(_scheduledService.get())
				{
					vertexUpdateRequest.getPage()->setService(_scheduledService);
				}
				else
				{
					vertexUpdateRequest.getPage()->setService(_continuousService);
				}
				vertexUpdateRequest.getAction()->setService(_scheduledService);

				stream << "<h1>Horaires</h1>";
				
				HTMLTable::ColsVector vs;
				vs.push_back("Arrêt");
				vs.push_back("Quai");
				vs.push_back("Arrivée");
				vs.push_back("Arrivée");
				vs.push_back("Départ");
				vs.push_back("Départ");
				if(_scheduledService.get())
				{
					vs.push_back("Retard");
					vs.push_back("Changement de quai");
				}
				HTMLTable ts(vs, ResultHTMLTable::CSS_CLASS);

				stream << ts.open();

				const Path* line(_service->getPath());
				BOOST_FOREACH(const Path::Edges::value_type& edge, line->getEdges())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));
					
					stream << ts.row();
					stream << ts.col() << lineStop.getPhysicalStop()->getConnectionPlace()->getFullName();
					stream << ts.col() << lineStop.getPhysicalStop()->getName();
					stream << ts.col();
					if(lineStop.isArrival())
					{
						stream << _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath());
					}
					stream << ts.col();
					if(lineStop.isArrival() && !(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) == _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath())))
					{
						time_duration delta(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) - _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}
					stream << ts.col();
					if(lineStop.isDeparture())
					{
						stream << _service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath());
					}
					stream << ts.col();
					if(lineStop.isDeparture() && !(_service->getDepartureBeginScheduleToIndex(true, lineStop.getRankInPath()) == _service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath())))
					{
						time_duration delta(_service->getDepartureBeginScheduleToIndex(true, lineStop.getRankInPath()) - _service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}
					if(_scheduledService.get())
					{
						scheduleUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());
						vertexUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());

						stream << ts.col();
						HTMLForm f(scheduleUpdateRequest.getHTMLForm("delay"+lexical_cast<string>(lineStop.getRankInPath())));
						stream << f.open();
						stream << "Durée : " << f.getSelectNumberInput(ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES, 0, 500);
						stream << "Propager : " << f.getOuiNonRadioInput(ScheduleRealTimeUpdateAction::PARAMETER_PROPAGATE_CONSTANTLY, true);
						stream << f.getSubmitButton("OK");
						stream << f.close();

						stream << ts.col();
						HTMLForm f2(vertexUpdateRequest.getHTMLForm("quay"+lexical_cast<string>(lineStop.getRankInPath())));
						stream << f2.open();
						stream << "Quai : " << f.getSelectInput(
							ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID,
							lineStop.getPhysicalStop()->getConnectionPlace()->getPhysicalStopLabels(),
							_scheduledService->getRealTimeVertex(lineStop.getRankInPath())->getKey()
						);
						stream << f.getSubmitButton("OK");
						stream << f2.close();
					}
				}

				stream << ts.close();

				if(_scheduledService.get())
				{
					stream << "<h1>Informations temps réel</h1>";

					stream << "<p>Information temps réel valables jusqu'à : " << posix_time::to_simple_string(_scheduledService->getNextRTUpdate()) << "</p>";
				}

			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTINUOUS
			if (openTabContent(stream, TAB_CONTINUOUS))
			{
				
			}

			////////////////////////////////////////////////////////////////////
			// TAB CALENDAR
			if (openTabContent(stream, TAB_CALENDAR))
			{
				CalendarHTMLViewer cv(*_service);
				cv.display(stream);
			}
		
			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		std::string ServiceAdmin::getTitle() const
		{
			return _service.get() ? _service->getServiceNumber() + (_scheduledService.get() ? " (service à horaires)" : " (service continu)") : DEFAULT_TITLE;
		}



		bool ServiceAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			return _service->getKey() == dynamic_cast<const ServiceAdmin&>(other)._service->getKey();
		}



		void ServiceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Horaires", TAB_SCHEDULES, true));

			if(_continuousService.get())
			{
				_tabs.push_back(Tab("Service continu", TAB_CONTINUOUS, true));
			}
			_tabs.push_back(Tab("Calendrier", TAB_CALENDAR, true));
			
			_tabBuilded = true;
		}



		void ServiceAdmin::setService( boost::shared_ptr<const env::ScheduledService> value )
		{
			_scheduledService = value;
			_service = static_pointer_cast<const NonPermanentService, const ScheduledService>(value);
		}



		void ServiceAdmin::setService( boost::shared_ptr<const env::ContinuousService> value )
		{
			_continuousService = value;
			_service = static_pointer_cast<const NonPermanentService, const ContinuousService>(value);
		}



		boost::shared_ptr<const NonPermanentService> ServiceAdmin::getService() const
		{
			return _service;
		}



		AdminInterfaceElement::PageLinks ServiceAdmin::_getCurrentTreeBranch() const
		{
			shared_ptr<LineAdmin> p(
				getNewOtherPage<LineAdmin>()
			);
			p->setLine(Env::GetOfficialEnv().getSPtr(static_cast<const Line*>(_service->getPath())));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}
	}
}
