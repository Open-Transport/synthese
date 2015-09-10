
//////////////////////////////////////////////////////////////////////////
/// ServiceAdmin class implementation.
///	@file ServiceAdmin.cpp
///	@author Hugues
///	@date 2009
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

#include "ServiceAdmin.h"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminModule.h"
#include "AdminParametersException.h"
#include "BaseCalendarAdmin.hpp"
#include "CalendarTemplate.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "ContinuousServiceUpdateAction.h"
#include "DRTArea.hpp"
#include "DRTAreaAdmin.hpp"
#include "HTMLForm.h"
#include "ImportableAdmin.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternAdmin.hpp"
#include "LineStop.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTPlaceAdmin.h"
#include "PTRuleUserAdmin.hpp"
#include "ResultHTMLTable.h"
#include "ScheduledServiceTableSync.h"
#include "ScheduledService.h"
#include "ScheduleRealTimeUpdateAction.h"
#include "ServiceTimetableUpdateAction.h"
#include "ServiceUpdateAction.h"
#include "ServiceVertexUpdateAction.hpp"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "Session.h"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace graph;
	using namespace html;
	using namespace calendar;
	using namespace db;
	using namespace impex;

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
		const string ServiceAdmin::TAB_PROPERTIES("pr");
		const string ServiceAdmin::TAB_SCHEDULES("sc");
		const string ServiceAdmin::TAB_REAL_TIME("rt");

		const string ServiceAdmin::SESSION_VARIABLE_SERVICE_ADMIN_ID("service_admin_id");
		const string ServiceAdmin::SESSION_VARIABLE_SERVICE_ADMIN_SCHEDULE_RANK("service_admin_schedule_rank");



		ServiceAdmin::ServiceAdmin()
			: AdminInterfaceElementTemplate<ServiceAdmin>()
		{}



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
			if(_service.get() == NULL) return false;

			if(_service->getPath() && _service->getPath()->getPathGroup())
			{
				return user.getProfile()->isAuthorized<TransportNetworkRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(static_cast<const CommercialLine*>(_service->getPath()->getPathGroup())->getKey()));
			}
			else
			{
				return false;
			}
		}



		void ServiceAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			const JourneyPattern* line(
				static_cast<const JourneyPattern*>(_service->getPath())
			);

			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULES
			if (openTabContent(stream, TAB_SCHEDULES))
			{
				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
				AdminFunctionRequest<DRTAreaAdmin> openAreaRequest(request);

				AdminActionFunctionRequest<ServiceTimetableUpdateAction,ServiceAdmin> timetableUpdateRequest(request, *this);
				timetableUpdateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));

				AdminActionFunctionRequest<ServiceVertexUpdateAction,ServiceAdmin> serviceVertexUpdateAction(request, *this);
				if(_scheduledService.get())
				{
					serviceVertexUpdateAction.getPage()->setService(_scheduledService);
				}
				else
				{
					serviceVertexUpdateAction.getPage()->setService(_continuousService);
				}
				serviceVertexUpdateAction.getAction()->setService(const_pointer_cast<ScheduledService>(_scheduledService));

				stream << "<h1>Horaires</h1>";

				HTMLTable::ColsVector vs;
				vs.push_back("Arrêt");
				vs.push_back("Quai");
				vs.push_back("Arrivée");
				vs.push_back("Départ");

				HTMLTable ts(vs, ResultHTMLTable::CSS_CLASS);

				stream << ts.open();

				size_t rank(0);
				bool focusDone(false);
				BOOST_FOREACH(const JourneyPattern::LineStops::value_type& edge, line->getLineStops())
				{
					const LineStop& lineStop(static_cast<const LineStop&>(*edge));
					if(!lineStop.get<ScheduleInput>())
					{
						continue;
					}

					timetableUpdateRequest.getAction()->setRank(edge->get<RankInPath>());

					stream << ts.row();

					// Place / area
					StopPoint* stopPoint(
						dynamic_cast<StopPoint*>(&*lineStop.get<LineNode>())
					);
					DRTArea* area(
						dynamic_cast<DRTArea*>(&*lineStop.get<LineNode>())
					);
					if(stopPoint)
					{
						stream << ts.col();
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(stopPoint->getConnectionPlace())
						);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								stopPoint->getConnectionPlace()->getFullName()
							);
						stream << ts.col();

						BOOST_FOREACH(const StopArea::PhysicalStops::value_type& ps, stopPoint->getConnectionPlace()->getPhysicalStops())
						{
							if(ps.second == _service->getVertex(edge->get<RankInPath>()))
							{
								stream << "[";
							}

							if(ps.second == stopPoint)
							{
								stream << "<b>";
							}

							serviceVertexUpdateAction.getAction()->setPhysicalStop(Env::GetOfficialEnv().getEditableSPtr(const_cast<StopPoint*>(ps.second)));
							serviceVertexUpdateAction.getAction()->setLineStopRank(lineStop.get<RankInPath>());
							stream << HTMLModule::getHTMLLink(
								serviceVertexUpdateAction.getHTMLForm().getURL(),
								ps.second->getName().empty() ? lexical_cast<string>(ps.second->getKey()) : ps.second->getName()
							);
							serviceVertexUpdateAction.getAction()->setPhysicalStop(boost::shared_ptr<StopPoint>());

							if(ps.second == stopPoint)
							{
								stream << "</b>";
							}

							if(ps.second == _service->getVertex(edge->get<RankInPath>()))
							{
								stream << "]";
							}
							stream << " ";
						}
					}
					if(area)
					{
						stream << ts.col(2);
						openAreaRequest.getPage()->setArea(
							Env::GetOfficialEnv().getSPtr(area)
						);
						stream <<
							HTMLModule::getHTMLLink(
								openAreaRequest.getURL(),
								area->getName()
							);
					}

					// Arrival time
					stream << ts.col();
					if(lineStop.get<IsArrival>())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(true);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm("arrival"+ lexical_cast<string>(lineStop.get<RankInPath>())));
						stream << tuForm.open();
						stream << tuForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_TIME,
							to_simple_string(
								_service->getArrivalBeginScheduleToIndex(false, lineStop.get<RankInPath>())
							), string(),
							AdminModule::CSS_TIME_INPUT
						);
						stream << tuForm.getSubmitButton("Change");
						stream << tuForm.close();

						HTMLForm suForm(timetableUpdateRequest.getHTMLForm("shiftarrival"+ lexical_cast<string>(lineStop.get<RankInPath>())));
						stream << suForm.open();
						stream << suForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_SHIFTING_DELAY,
							string(),
							string(),
							AdminModule::CSS_2DIGIT_INPUT
						);
						string sessionVariableId(request.getSession()->getSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_ID));
						if(!focusDone)
						{
							if(	!sessionVariableId.empty() &&
								lexical_cast<RegistryKeyType>(sessionVariableId) == _service->getKey()
							){
								string sessionVariableRank(request.getSession()->getSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_SCHEDULE_RANK));
								if(	!sessionVariableRank.empty() &&
									lineStop.get<RankInPath>() >= (lexical_cast<size_t>(sessionVariableRank) + 1)
								){
									stream << suForm.setFocus(ServiceTimetableUpdateAction::PARAMETER_SHIFTING_DELAY);
									focusDone = true;
								}
							}
							else
							{
								suForm.setFocus(ServiceTimetableUpdateAction::PARAMETER_SHIFTING_DELAY);
								focusDone = true;
						}	}

						stream << suForm.getSubmitButton("Shift");
						stream << suForm.close();
					}

					// Departure time
					stream << ts.col();
					if(lineStop.get<IsDeparture>())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(false);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm("departure"+ lexical_cast<string>(lineStop.get<RankInPath>())));
						stream << tuForm.open();
						stream << tuForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_TIME,
							to_simple_string(
								_service->getDepartureBeginScheduleToIndex(false, lineStop.get<RankInPath>())
							), string(),
							AdminModule::CSS_TIME_INPUT
						);
						stream << tuForm.getSubmitButton("Change");
						stream << tuForm.close();

						HTMLForm suForm(timetableUpdateRequest.getHTMLForm("shiftdeparture"+ lexical_cast<string>(lineStop.get<RankInPath>())));
						stream << suForm.open();
						stream << suForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_SHIFTING_DELAY,
							string(),
							string(),
							AdminModule::CSS_2DIGIT_INPUT
						);
						stream << suForm.getSubmitButton("Shift");
						stream << suForm.close();
					}

					++rank;
				}

				stream << ts.close();

				if(_continuousService.get())
				{
					stream << "<h1>Service continu</h1>";

					AdminActionFunctionRequest<ContinuousServiceUpdateAction,ServiceAdmin> updateRequest(request, *this);
					updateRequest.getAction()->setService(const_pointer_cast<ContinuousService>(_continuousService));

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Attente maximale (minutes)", t.getForm().getTextInput(ContinuousServiceUpdateAction::PARAMETER_WAITING_DURATION, lexical_cast<string>(_continuousService->getMaxWaitingTime().total_seconds() / 60)));
					stream << t.cell("Fin de période", t.getForm().getTextInput(ContinuousServiceUpdateAction::PARAMETER_END_TIME, lexical_cast<string>(_continuousService->getDepartureEndScheduleToIndex(false, 0))));
					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB REAL TIME
			if (openTabContent(stream, TAB_REAL_TIME))
			{
				AdminActionFunctionRequest<ScheduleRealTimeUpdateAction, ServiceAdmin> scheduleUpdateRequest(request, *this);
				if(_scheduledService.get())
				{
					scheduleUpdateRequest.getPage()->setService(_scheduledService);
				}
				else
				{
					scheduleUpdateRequest.getPage()->setService(_continuousService);
				}
				scheduleUpdateRequest.getAction()->setService(_scheduledService);

				AdminActionFunctionRequest<ServiceVertexRealTimeUpdateAction, ServiceAdmin> vertexUpdateRequest(request, *this);
				if(_scheduledService.get())
				{
					vertexUpdateRequest.getPage()->setService(_scheduledService);
				}
				else
				{
					vertexUpdateRequest.getPage()->setService(_continuousService);
				}
				vertexUpdateRequest.getAction()->setService(const_pointer_cast<ScheduledService>(_scheduledService));

				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
				AdminFunctionRequest<DRTAreaAdmin> openAreaRequest(request);

				stream << "<h1>Horaires</h1>";

				HTMLTable::ColsVector vs;
				vs.push_back("Arrêt");
				vs.push_back("Quai");
				vs.push_back("Arrivée");
				vs.push_back("Arrivée");
				vs.push_back("Départ");
				vs.push_back("Départ");
				vs.push_back("Retard");
				vs.push_back("Changement de quai");

				HTMLTable ts(vs, ResultHTMLTable::CSS_CLASS);

				stream << ts.open();

				BOOST_FOREACH(const JourneyPattern::LineStops::value_type& edge, line->getLineStops())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));

					// Place / area
					StopPoint* stopPoint(
						dynamic_cast<StopPoint*>(&*lineStop.get<LineNode>())
					);
					DRTArea* area(
						dynamic_cast<DRTArea*>(&*lineStop.get<LineNode>())
					);

					stream << ts.row();

					// Place / area
					if(stopPoint)
					{
						stream << ts.col();
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(stopPoint->getConnectionPlace())
						);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								stopPoint->getConnectionPlace()->getFullName()
							);
						stream << ts.col();
						stream << stopPoint->getName();
					}
					if(area)
					{
						stream << ts.col(2);
						openAreaRequest.getPage()->setArea(
								Env::GetOfficialEnv().getSPtr(area)
							);
						stream <<
							HTMLModule::getHTMLLink(
								openAreaRequest.getURL(),
								area->getName()
							);
					}

					// Arrival time
					stream << ts.col();
					if(lineStop.get<IsArrival>())
					{
						stream << to_simple_string(
							_service->getArrivalBeginScheduleToIndex(false, lineStop.get<RankInPath>())
						);
					}

					// Arrival real time
					stream << ts.col();
					if(lineStop.get<IsArrival>() && !(_service->getArrivalBeginScheduleToIndex(true, lineStop.get<RankInPath>()) == _service->getArrivalBeginScheduleToIndex(false, lineStop.get<RankInPath>())))
					{
						time_duration delta(_service->getArrivalBeginScheduleToIndex(true, lineStop.get<RankInPath>()) - _service->getArrivalBeginScheduleToIndex(false, lineStop.get<RankInPath>()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}

					// Departure time
					stream << ts.col();
					if(lineStop.get<IsDeparture>())
					{
						stream << to_simple_string(
							_service->getDepartureBeginScheduleToIndex(false, lineStop.get<RankInPath>())
						);
					}

					// Departure real time
					stream << ts.col();
					if(lineStop.get<IsDeparture>() && !(_service->getDepartureBeginScheduleToIndex(true, lineStop.get<RankInPath>()) == _service->getDepartureBeginScheduleToIndex(false, lineStop.get<RankInPath>())))
					{
						time_duration delta(_service->getDepartureBeginScheduleToIndex(true, lineStop.get<RankInPath>()) - _service->getDepartureBeginScheduleToIndex(false, lineStop.get<RankInPath>()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}
					scheduleUpdateRequest.getAction()->setLineStopRank(lineStop.get<RankInPath>());
					scheduleUpdateRequest.getAction()->setAtDeparture(true);
					scheduleUpdateRequest.getAction()->setAtArrival(true);
					vertexUpdateRequest.getAction()->setLineStopRank(lineStop.get<RankInPath>());

					stream << ts.col();
					HTMLForm f(scheduleUpdateRequest.getHTMLForm("delay"+lexical_cast<string>(lineStop.get<RankInPath>())));
					stream << f.open();
					stream << "Durée : " << f.getSelectNumberInput(ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES, 0, 500);
					stream << "Propager : " << f.getOuiNonRadioInput(ScheduleRealTimeUpdateAction::PARAMETER_PROPAGATE_CONSTANTLY, true);
					stream << f.getSubmitButton("OK");
					stream << f.close();

					// Served stop
					stream << ts.col();
					if(stopPoint)
					{
						HTMLForm f2(vertexUpdateRequest.getHTMLForm("quay"+lexical_cast<string>(lineStop.get<RankInPath>())));
						stream << f2.open();
						stream << "Quai : " << f.getSelectInput(
							ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID,
							stopPoint->getConnectionPlace()->getPhysicalStopLabels(false, "Non desservi"),
							optional<RegistryKeyType>(
								_service->getRealTimeVertex(lineStop.get<RankInPath>()) ?
								_service->getRealTimeVertex(lineStop.get<RankInPath>())->getKey() :
								0
							)
						);
						stream << f.getSubmitButton("OK");
						stream << f2.close();
					}
				}

				stream << ts.close();

				stream << "<h1>Informations temps réel</h1>";
				stream << "<p>Information temps réel valables jusqu'à : " << posix_time::to_simple_string(_service->getNextRTUpdate()) << "</p>";
			}

			////////////////////////////////////////////////////////////////////
			// TAB CALENDAR
			if (openTabContent(stream, TAB_CALENDAR))
			{
				const TransportNetwork* network(
					static_cast<const CommercialLine*>(_service->getPath()->getPathGroup())->getNetwork()
				);

				AdminActionFunctionRequest<ServiceUpdateAction, ServiceAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setService(
					const_pointer_cast<SchedulesBasedService>(_service)
				);

				BaseCalendarAdmin::Display(
					stream,
					*_service,
					updateRequest,
					network->get<DaysCalendarParent>() ? optional<RegistryKeyType>(network->get<DaysCalendarParent>()->getKey()) : optional<RegistryKeyType>(),
					network->get<PeriodsCalendarParent>() ? optional<RegistryKeyType>(network->get<PeriodsCalendarParent>()->getKey()) : optional<RegistryKeyType>()
				);
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<ServiceUpdateAction,ServiceAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setService(
					const_pointer_cast<SchedulesBasedService>(
						_service
				)	);
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Numéro", t.getForm().getTextInput(ServiceUpdateAction::PARAMETER_SERVICE_NUMBER, _service->getServiceNumber()));
				if(_scheduledService.get())
				{
					stream << t.cell("Equipe", t.getForm().getTextInput(ServiceUpdateAction::PARAMETER_TEAM_NUMBER, _scheduledService->getTeam()));
				}
				stream << t.close();

				PTRuleUserAdmin<SchedulesBasedService,ServiceAdmin>::Display(stream, _service, request);

				if(_scheduledService.get())
				{
					StaticActionRequest<ServiceUpdateAction> updateOnlyRequest(request);
					updateOnlyRequest.getAction()->setService(
						const_pointer_cast<SchedulesBasedService>(
							_service
					)	);
					ImportableAdmin::DisplayDataSourcesTab(stream, *_scheduledService, updateOnlyRequest);
				}
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		std::string ServiceAdmin::getTitle() const
		{
			return _service.get() ? _service->getServiceNumber() : DEFAULT_TITLE;
		}



		bool ServiceAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			return _service->getKey() == dynamic_cast<const ServiceAdmin&>(other)._service->getKey();
		}



		void ServiceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Horaires", TAB_SCHEDULES, true, "time.png"));

			_tabs.push_back(Tab("Temps réel", TAB_REAL_TIME, true, "time.png"));

			_tabs.push_back(Tab("Calendrier", TAB_CALENDAR, true, "calendar.png"));

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true, "application_form.png"));

			_tabBuilded = true;
		}



		void ServiceAdmin::setService( boost::shared_ptr<const ScheduledService> value )
		{
			_scheduledService = value;
			_service = static_pointer_cast<const SchedulesBasedService, const ScheduledService>(value);
		}



		void ServiceAdmin::setService( boost::shared_ptr<const ContinuousService> value )
		{
			_continuousService = value;
			_service = static_pointer_cast<const SchedulesBasedService, const ContinuousService>(value);
		}



		boost::shared_ptr<const SchedulesBasedService> ServiceAdmin::getService() const
		{
			return _service;
		}



		AdminInterfaceElement::PageLinks ServiceAdmin::_getCurrentTreeBranch() const
		{
			boost::shared_ptr<JourneyPatternAdmin> p(
				getNewPage<JourneyPatternAdmin>()
			);
			p->setLine(Env::GetOfficialEnv().get<JourneyPattern>(_service->getPath()->getKey()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}



		std::string ServiceAdmin::getIcon() const
		{
			return _continuousService.get() ? "arrow_ew.png" : ICON;
		}
	}
}
