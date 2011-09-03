
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
#include "LineArea.hpp"
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
#include "DRTAreaAdmin.hpp"
#include "DRTArea.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "ServiceCalendarLinkUpdateAction.hpp"
#include "ServiceCalendarLink.hpp"
#include "CalendarTemplateAdmin.h"
#include "RemoveObjectAction.hpp"
#include "ImportableAdmin.hpp"

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
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void ServiceAdmin::display(
			ostream& stream,
			const AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULES
			if (openTabContent(stream, TAB_SCHEDULES))
			{
				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
				AdminFunctionRequest<DRTAreaAdmin> openAreaRequest(request);

				AdminActionFunctionRequest<ServiceTimetableUpdateAction,ServiceAdmin> timetableUpdateRequest(request);
				timetableUpdateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));

				stream << "<h1>Horaires</h1>";

				HTMLTable::ColsVector vs;
				vs.push_back("Arrêt");
				vs.push_back("Quai");
				vs.push_back("Arrivée");
				vs.push_back("Départ");

				HTMLTable ts(vs, ResultHTMLTable::CSS_CLASS);

				stream << ts.open();

				const Path* line(_service->getPath());
				size_t rank(0);
				bool focusDone(false);
				BOOST_FOREACH(const Path::Edges::value_type& edge, line->getEdges())
				{
					const LineStop& lineStop(static_cast<const LineStop&>(*edge));
					if(!lineStop.getScheduleInput())
					{
						continue;
					}

					const DesignatedLinePhysicalStop* linePhysicalStop(dynamic_cast<const DesignatedLinePhysicalStop*>(edge));
					const LineArea* lineArea(dynamic_cast<const LineArea*>(edge));

					timetableUpdateRequest.getAction()->setRank(edge->getRankInPath());

					stream << ts.row();

					// Place / area
					if(linePhysicalStop)
					{
						stream << ts.col();
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(linePhysicalStop->getPhysicalStop()->getConnectionPlace())
						);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getFullName()
							);
						stream << ts.col();
						stream << linePhysicalStop->getPhysicalStop()->getName();
					}
					if(lineArea)
					{
						stream << ts.col(2);
						openAreaRequest.getPage()->setArea(
							Env::GetOfficialEnv().getSPtr(lineArea->getArea())
						);
						stream <<
							HTMLModule::getHTMLLink(
								openAreaRequest.getURL(),
								lineArea->getArea()->getName()
							);
					}

					// Arrival time
					stream << ts.col();
					if(lineStop.isArrivalAllowed())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(true);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm("arrival"+ lexical_cast<string>(lineStop.getRankInPath())));
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

						HTMLForm suForm(timetableUpdateRequest.getHTMLForm("shiftarrival"+ lexical_cast<string>(lineStop.getRankInPath())));
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
									lineStop.getRankInPath() >= (lexical_cast<size_t>(sessionVariableRank) + 1)
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
					if(lineStop.isDepartureAllowed())
					{
						timetableUpdateRequest.getAction()->setUpdateArrival(false);
						HTMLForm tuForm(timetableUpdateRequest.getHTMLForm("departure"+ lexical_cast<string>(lineStop.getRankInPath())));
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

						HTMLForm suForm(timetableUpdateRequest.getHTMLForm("shiftdeparture"+ lexical_cast<string>(lineStop.getRankInPath())));
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
			// TAB REAL TIME
			if (openTabContent(stream, TAB_REAL_TIME))
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

				const Path* line(_service->getPath());
				BOOST_FOREACH(const Path::Edges::value_type& edge, line->getEdges())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));

					const DesignatedLinePhysicalStop* linePhysicalStop(dynamic_cast<const DesignatedLinePhysicalStop*>(edge));
					const LineArea* lineArea(dynamic_cast<const LineArea*>(edge));

					stream << ts.row();

					// Place / area
					if(linePhysicalStop)
					{
						stream << ts.col();
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(linePhysicalStop->getPhysicalStop()->getConnectionPlace())
						);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getFullName()
							);
						stream << ts.col();
						stream << linePhysicalStop->getPhysicalStop()->getName();
					}
					if(lineArea)
					{
						stream << ts.col(2);
						openAreaRequest.getPage()->setArea(
								Env::GetOfficialEnv().getSPtr(lineArea->getArea())
							);
						stream <<
							HTMLModule::getHTMLLink(
								openAreaRequest.getURL(),
								lineArea->getArea()->getName()
							);
					}

					// Arrival time
					stream << ts.col();
					if(lineStop.isArrival())
					{
						stream << to_simple_string(
							_service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath())
						);
					}

					// Arrival real time
					stream << ts.col();
					if(lineStop.isArrival() && !(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) == _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath())))
					{
						time_duration delta(_service->getArrivalBeginScheduleToIndex(true, lineStop.getRankInPath()) - _service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()));
						stream << (delta.total_seconds() > 0 ? "+" : string()) << delta << " min";
					}

					// Departure time
					stream << ts.col();
					if(lineStop.isDeparture())
					{
						stream << to_simple_string(
							_service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath())
						);
					}

					// Departure real time
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

					// Served stop
					stream << ts.col();
					if(linePhysicalStop)
					{
						HTMLForm f2(vertexUpdateRequest.getHTMLForm("quay"+lexical_cast<string>(lineStop.getRankInPath())));
						stream << f2.open();
						stream << "Quai : " << f.getSelectInput(
							ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID,
							linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getPhysicalStopLabels(false, "Non desservi"),
							optional<RegistryKeyType>(
								_service->getRealTimeVertex(lineStop.getRankInPath()) ?
								_service->getRealTimeVertex(lineStop.getRankInPath())->getKey() :
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
				// Session variables
				string sessionStartDateStr(request.getSession()->getSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE));
				date sessionStartDate;
				if(!sessionStartDateStr.empty())
				{
					sessionStartDate = from_string(sessionStartDateStr);
				}
				string sessionEndDateStr(request.getSession()->getSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE));
				date sessionEndDate;
				if(!sessionEndDateStr.empty())
				{
					sessionEndDate = from_string(sessionEndDateStr);
				}
				string sessionCalendarTemplateIdStr(request.getSession()->getSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID));
				RegistryKeyType sessionCalendarTemplateId(0);
				if(!sessionCalendarTemplateIdStr.empty())
				{
					sessionCalendarTemplateId = lexical_cast<RegistryKeyType>(sessionCalendarTemplateIdStr);
				}
				string sessionCalendarTemplateIdStr2(request.getSession()->getSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2));
				RegistryKeyType sessionCalendarTemplateId2(0);
				if(!sessionCalendarTemplateIdStr2.empty())
				{
					sessionCalendarTemplateId2 = lexical_cast<RegistryKeyType>(sessionCalendarTemplateIdStr2);
				}

				stream << "<h1>Définition par lien</h1>";

				if(_service->getCalendarLinks().empty())
				{
					stream << "<p class=\"info\">Lorsque un calendrier de service est défini par lien, la définition par date est impossible et certaines fonctions temps réel peuvent ne pas fonctionner.</p>";
				}

				AdminActionFunctionRequest<ServiceCalendarLinkUpdateAction,ServiceAdmin> calendarLinkAddRequest(request);
				calendarLinkAddRequest.getAction()->setService(static_pointer_cast<NonPermanentService, SchedulesBasedService>(const_pointer_cast<SchedulesBasedService>(_service)));

				AdminFunctionRequest<CalendarTemplateAdmin> openCalendarRequest(request);

				AdminActionFunctionRequest<RemoveObjectAction,ServiceAdmin> calendarLinkRemoveRequest(request);
				
				HTMLForm calendarLinkAddForm(calendarLinkAddRequest.getHTMLForm("calendar_link_add"));
				stream << calendarLinkAddForm.open();

				HTMLTable::ColsVector vs;
				vs.push_back("Date début");
				vs.push_back("Date fin");
				vs.push_back("Calendrier jours");
				vs.push_back("Calendrier période");
				vs.push_back("Action");
				HTMLTable tc(vs, ResultHTMLTable::CSS_CLASS);
				stream << tc.open();
				BOOST_FOREACH(NonPermanentService::CalendarLinks::value_type link, _service->getCalendarLinks())
				{
					stream << tc.row();
					stream << tc.col() << link->getStartDate();
					stream << tc.col() << link->getEndDate();
					
					// Calendar days
					stream << tc.col();
					if(link->getCalendarTemplate())
					{
						openCalendarRequest.getPage()->setCalendar(Env::GetOfficialEnv().getSPtr(link->getCalendarTemplate()));
						stream << HTMLModule::getHTMLLink(openCalendarRequest.getURL(), link->getCalendarTemplate()->getName());
					}

					// Calendar period
					stream << tc.col();
					if(link->getCalendarTemplate2())
					{
						openCalendarRequest.getPage()->setCalendar(Env::GetOfficialEnv().getSPtr(link->getCalendarTemplate2()));
						stream << HTMLModule::getHTMLLink(openCalendarRequest.getURL(), link->getCalendarTemplate2()->getName());
					}

					// Remove button
					calendarLinkRemoveRequest.getAction()->setObjectId(link->getKey());
					stream << tc.col() << HTMLModule::getLinkButton(calendarLinkRemoveRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le lien ?");
				}
				const TransportNetwork* network(
					static_cast<const CommercialLine*>(_service->getPath()->getPathGroup())->getNetwork()
				);
				stream << tc.row();
				stream << tc.col() << calendarLinkAddForm.getCalendarInput(ServiceCalendarLinkUpdateAction::PARAMETER_START_DATE, sessionStartDate);
				stream << tc.col() << calendarLinkAddForm.getCalendarInput(ServiceCalendarLinkUpdateAction::PARAMETER_END_DATE, sessionEndDate);
				stream << tc.col() << calendarLinkAddForm.getSelectInput(
						ServiceCalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID,
						CalendarTemplateTableSync::GetCalendarTemplatesList(
							"(aucun)",
							optional<RegistryKeyType>(),
							network->getDaysCalendarsParent() ? optional<RegistryKeyType>(network->getDaysCalendarsParent()->getKey()) : optional<RegistryKeyType>()
						),
						optional<RegistryKeyType>(sessionCalendarTemplateId)
				);
				stream << tc.col() << calendarLinkAddForm.getSelectInput(
						ServiceCalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID2,
						CalendarTemplateTableSync::GetCalendarTemplatesList(
							"(aucun)",
							optional<RegistryKeyType>(),
							network->getPeriodsCalendarsParent() ? optional<RegistryKeyType>(network->getPeriodsCalendarsParent()->getKey()) : optional<RegistryKeyType>()
						),
						optional<RegistryKeyType>(sessionCalendarTemplateId2)
				);
				stream << tc.col() << calendarLinkAddForm.getSubmitButton("Ajouter");
				stream << tc.close();
				stream << calendarLinkAddForm.close();
				
				if(_service->getCalendarLinks().empty())
				{
					stream << "<h1>Définition par date</h1>";

					AdminActionFunctionRequest<ServiceApplyCalendarAction,ServiceAdmin> updateRequest(request);
					updateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));
					PropertiesHTMLTable p(updateRequest.getHTMLForm("applycalendar"));
					stream << p.open();
					stream << p.cell("Date début", p.getForm().getCalendarInput(ServiceApplyCalendarAction::PARAMETER_START_DATE, sessionStartDate));
					stream << p.cell("Date fin", p.getForm().getCalendarInput(ServiceApplyCalendarAction::PARAMETER_END_DATE, sessionEndDate));
					stream << p.cell("Période", p.getForm().getTextInput(ServiceApplyCalendarAction::PARAMETER_PERIOD, "1", string(), AdminModule::CSS_2DIGIT_INPUT));
					stream << p.cell("Modèle", p.getForm().getSelectInput(
							ServiceApplyCalendarAction::PARAMETER_CALENDAR_TEMPLATE_ID,
							CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
							optional<RegistryKeyType>(sessionCalendarTemplateId)
					)	);
					stream << p.cell("Ajout", p.getForm().getOuiNonRadioInput(ServiceApplyCalendarAction::PARAMETER_ADD, true));
					stream << p.close();
				}

				stream << "<h1>Résultat</h1>";
				AdminActionFunctionRequest<ServiceDateChangeAction,ServiceAdmin> updateDateRequest(request);
				updateDateRequest.getAction()->setService(const_pointer_cast<SchedulesBasedService>(_service));
				CalendarHTMLViewer<AdminActionFunctionRequest<ServiceDateChangeAction,ServiceAdmin> > cv(*_service, &updateDateRequest);
				cv.display(stream);
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

				if(_scheduledService.get())
				{
					StaticActionRequest<ServiceUpdateAction> updateOnlyRequest(request);
					updateOnlyRequest.getAction()->setService(const_pointer_cast<Service>(static_pointer_cast<const Service>(_scheduledService)));
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
			shared_ptr<JourneyPatternAdmin> p(
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
