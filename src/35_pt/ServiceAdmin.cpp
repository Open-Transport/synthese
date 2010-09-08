
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
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "HTMLForm.h"
#include "AdminActionFunctionRequest.hpp"
#include "ScheduleRealTimeUpdateAction.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "CalendarHTMLViewer.h"
#include "JourneyPatternAdmin.hpp"
#include "Profile.h"
#include "PTPlaceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "ContinuousServiceUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "PTRuleUserAdmin.hpp"
#include "ServiceTimetableUpdateAction.h"
#include "ServiceUpdateAction.h"
#include "AdminModule.h"
#include "ServiceApplyCalendarAction.h"
#include "CalendarTemplateTableSync.h"
#include "ServiceDateChangeAction.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
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
		const string ServiceAdmin::TAB_PROPERTIES("pr");
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

				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

				AdminActionFunctionRequest<ServiceTimetableUpdateAction,ServiceAdmin> timetableUpdateRequest(request);
				timetableUpdateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));
				
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

				const Path* line(_service->getPath());
				BOOST_FOREACH(const Path::Edges::value_type& edge, line->getEdges())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));
					timetableUpdateRequest.getAction()->setRank(lineStop.getRankInPath());

					openPlaceRequest.getPage()->setConnectionPlace(
						Env::GetOfficialEnv().getSPtr(lineStop.getPhysicalStop()->getConnectionPlace())
					);
					
					stream << ts.row();
					stream << ts.col() << 
						HTMLModule::getHTMLLink(
							openPlaceRequest.getURL(),
							lineStop.getPhysicalStop()->getConnectionPlace()->getFullName()
						);
					stream << ts.col() << lineStop.getPhysicalStop()->getName();
					stream << ts.col();
					if(lineStop.isArrival())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(true);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm());
						stream << tuForm.open();
						stream << tuForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_TIME,
							to_simple_string(
								_service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath())
							), string(),
							AdminModule::CSS_TIME_INPUT
						);
						stream << tuForm.getSubmitButton("Change");
						stream << tuForm.close();
						
						HTMLForm suForm(timetableUpdateRequest.getHTMLForm());
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
					stream << ts.col();
					if(lineStop.isArrival() && !(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) == _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath())))
					{
						time_duration delta(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) - _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}
					stream << ts.col();
					if(lineStop.isDeparture())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(false);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm());
						stream << tuForm.open();
						stream << tuForm.getTextInput(
							ServiceTimetableUpdateAction::PARAMETER_TIME,
							to_simple_string(
								_service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath())
							), string(),
							AdminModule::CSS_TIME_INPUT
						);
						stream << tuForm.getSubmitButton("Change");
						stream << tuForm.close();

						HTMLForm suForm(timetableUpdateRequest.getHTMLForm());
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
					stream << ts.col();
					if(lineStop.isDeparture() && !(_service->getDepartureBeginScheduleToIndex(true, lineStop.getRankInPath()) == _service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath())))
					{
						time_duration delta(_service->getDepartureBeginScheduleToIndex(true, lineStop.getRankInPath()) - _service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}
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
						optional<RegistryKeyType>(_service->getRealTimeVertex(lineStop.getRankInPath())->getKey())
					);
					stream << f.getSubmitButton("OK");
					stream << f2.close();
				}

				stream << ts.close();

				stream << "<h1>Informations temps réel</h1>";
				stream << "<p>Information temps réel valables jusqu'à : " << posix_time::to_simple_string(_service->getNextRTUpdate()) << "</p>";

				if(_continuousService.get())
				{
					stream << "<h1>Service continu</h1>";

					AdminActionFunctionRequest<ContinuousServiceUpdateAction,ServiceAdmin> updateRequest(request);
					updateRequest.getAction()->setService(const_pointer_cast<ContinuousService>(_continuousService));
					
					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Attente maximale (minutes)", t.getForm().getTextInput(ContinuousServiceUpdateAction::PARAMETER_WAITING_DURATION, lexical_cast<string>(_continuousService->getMaxWaitingTime().total_seconds() / 60)));
					stream << t.cell("Fin de période", t.getForm().getTextInput(ContinuousServiceUpdateAction::PARAMETER_END_TIME, lexical_cast<string>(_continuousService->getDepartureEndScheduleToIndex(false, 0))));
					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB CALENDAR
			if (openTabContent(stream, TAB_CALENDAR))
			{
				stream << "<h1>Modifier</h1>";

				date now(day_clock::local_day());
				AdminActionFunctionRequest<ServiceApplyCalendarAction,ServiceAdmin> updateRequest(request);
				updateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));
				PropertiesHTMLTable p(updateRequest.getHTMLForm("applycalendar"));
				stream << p.open();
				stream << p.cell("Date début", p.getForm().getCalendarInput(ServiceApplyCalendarAction::PARAMETER_START_DATE, now));
				stream << p.cell("Date fin", p.getForm().getCalendarInput(ServiceApplyCalendarAction::PARAMETER_END_DATE, now));
				stream << p.cell("Période", p.getForm().getTextInput(ServiceApplyCalendarAction::PARAMETER_PERIOD, "1", string(), AdminModule::CSS_2DIGIT_INPUT));
				stream << p.cell("Modèle", p.getForm().getSelectInput(
						ServiceApplyCalendarAction::PARAMETER_CALENDAR_TEMPLATE_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
						optional<RegistryKeyType>(0)
				)	);
				stream << p.cell("Ajout", p.getForm().getOuiNonRadioInput(ServiceApplyCalendarAction::PARAMETER_ADD, true));
				stream << p.close();

				stream << "<h1>Résultat</h1>";
				AdminActionFunctionRequest<ServiceDateChangeAction,ServiceAdmin> updateDateRequest(request);
				updateDateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));
				CalendarHTMLViewer<AdminActionFunctionRequest<ServiceDateChangeAction,ServiceAdmin> > cv(*_service, &updateDateRequest);
				cv.display(stream);

				// ServiceDateChangeAction
			}
		
			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<ServiceUpdateAction,ServiceAdmin> updateRequest(request);
				updateRequest.getAction()->setService(const_pointer_cast<Service>(static_pointer_cast<const Service>(_service)));
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Numéro", t.getForm().getTextInput(ServiceUpdateAction::PARAMETER_SERVICE_NUMBER, _service->getServiceNumber()));
				if(_scheduledService.get())
				{
					stream << t.cell("Equipe", t.getForm().getTextInput(ServiceUpdateAction::PARAMETER_TEAM_NUMBER, _scheduledService->getTeam()));
				}
				stream << t.close();

				PTRuleUserAdmin<SchedulesBasedService,ServiceAdmin>::Display(stream, _service, request);
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

			_tabs.push_back(Tab("Horaires", TAB_SCHEDULES, true, "time.png"));

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
			shared_ptr<JourneyPatternAdmin> p(
				getNewOtherPage<JourneyPatternAdmin>()
			);
			p->setLine(Env::GetOfficialEnv().get<JourneyPattern>(_service->getPathId()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}
	}
}
