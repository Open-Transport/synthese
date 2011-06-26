////////////////////////////////////////////////////////////////////////////////
/// JourneyPatternAdmin class implementation.
///	@file JourneyPatternAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "JourneyPatternAdmin.hpp"
#include "CommercialLineAdmin.h"
#include "PTModule.h"
#include "Profile.h"
#include "CommercialLine.h"
#include "HTMLModule.h"
#include "StopPoint.hpp"
#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineArea.hpp"
#include "LineStopTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "StopArea.hpp"
#include "TransportNetworkRight.h"
#include "ServiceAdmin.h"
#include "Request.h"
#include "AdminFunctionRequest.hpp"
#include "ActionResultHTMLTable.h"
#include "AdminParametersException.h"
#include "ServiceAddAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "PTPlaceAdmin.h"
#include "JourneyPatternUpdateAction.hpp"
#include "PTRuleUserAdmin.hpp"
#include "PropertiesHTMLTable.h"
#include "RollingStockTableSync.h"
#include "LineStopAddAction.h"
#include "PTPlacesAdmin.h"
#include "City.h"
#include "LineStopUpdateAction.hpp"
#include "RemoveObjectAction.hpp"
#include "ImportableAdmin.hpp"
#include "DRTAreaAdmin.hpp"
#include "DRTArea.hpp"
#include "HTMLMap.hpp"
#include "StopPointUpdateAction.hpp"
#include "ServiceApplyCalendarAction.h"
#include "AdminModule.h"
#include "CalendarTemplateTableSync.h"
#include "Destination.hpp"

#include <geos/geom/Envelope.h>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace pt;
	using namespace html;
	using namespace security;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace geography;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, JourneyPatternAdmin>::FACTORY_KEY("JourneyPatternAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<JourneyPatternAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<JourneyPatternAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace pt
	{
		const string JourneyPatternAdmin::TAB_STOPS("st");
		const string JourneyPatternAdmin::TAB_SCHEDULED_SERVICES("ss");
		const string JourneyPatternAdmin::TAB_CONTINUOUS_SERVICES("cs");
		const string JourneyPatternAdmin::TAB_PROPERTIES("pr");
		const string JourneyPatternAdmin::TAB_INDICES("in");

		JourneyPatternAdmin::JourneyPatternAdmin()
			: AdminInterfaceElementTemplate<JourneyPatternAdmin>()
		{ }

		void JourneyPatternAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_line = Env::GetOfficialEnv().get<JourneyPattern>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch (ObjectNotFoundException<JourneyPattern>&)
			{
				throw AdminParametersException("No such line");
			}

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), ScheduledServiceTableSync::COL_SCHEDULES);
		}



		server::ParametersMap JourneyPatternAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_line.get()) m.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			return m;
		}



		void JourneyPatternAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{
			map<const Service*, string> services;

			ScheduledServiceTableSync::SearchResult sservices(
				ScheduledServiceTableSync::Search(
					Env::GetOfficialEnv(),
					_line->getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					false,
					0,
					optional<size_t>(),
					_requestParameters.orderField == ScheduledServiceTableSync::COL_SCHEDULES,
					_requestParameters.raisingOrder,
					UP_DOWN_LINKS_LOAD_LEVEL
			)	);

			ContinuousServiceTableSync::SearchResult cservices(
				ContinuousServiceTableSync::Search(
					Env::GetOfficialEnv(),
					_line->getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					_requestParameters.orderField == ScheduledServiceTableSync::COL_SCHEDULES,
					_requestParameters.raisingOrder,
					UP_DOWN_LINKS_LOAD_LEVEL
			)	);

			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				stream << "<h1>Desserte</h1>";

				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(_request);
				AdminFunctionRequest<PTPlacesAdmin> openCityRequest(_request);
				AdminFunctionRequest<DRTAreaAdmin> openDRTAreaRequest(_request);

				// Reservation
// 				bool reservation(_line->getReservationRule() && _line->getReservationRule()->getType() == RESERVATION_COMPULSORY);

				AdminActionFunctionRequest<RemoveObjectAction,JourneyPatternAdmin> lineStopRemoveAction(_request);

				AdminActionFunctionRequest<LineStopAddAction,JourneyPatternAdmin> lineStopAddAction(_request);
				lineStopAddAction.getAction()->setRoute(const_pointer_cast<JourneyPattern>(_line));
				HTMLForm f(lineStopAddAction.getHTMLForm("add_journey_pattern_stop"));

				AdminActionFunctionRequest<LineStopUpdateAction,JourneyPatternAdmin> lineStopUpdateAction(_request);

				HTMLTable::ColsVector v;
				v.push_back("Rang");
				v.push_back("Rang");
				v.push_back("Localité");
				v.push_back("Arrêt");
				v.push_back("Quai");
				v.push_back("pm");
				v.push_back("A");
				v.push_back("D");
				v.push_back("Hor");
// 				if (reservation)
// 					v.push_back("Resa");
				v.push_back("Action");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

				if(sservices.empty() && cservices.empty())
				{
					stream << f.open();
				}
				stream << t.open();

				BOOST_FOREACH(const Edge* edge, _line->getEdges())
				{
					shared_ptr<const DesignatedLinePhysicalStop> linePhysicalStop(
						dynamic_cast<const DesignatedLinePhysicalStop*>(edge) ?
						Env::GetOfficialEnv().getSPtr(static_cast<const DesignatedLinePhysicalStop*>(edge)) :
						shared_ptr<const DesignatedLinePhysicalStop>()
					);
					shared_ptr<const LineArea> lineArea(
						dynamic_cast<const LineArea*>(edge) ?
						Env::GetOfficialEnv().getSPtr(static_cast<const LineArea*>(edge)) :
						shared_ptr<const LineArea>()
					);
					shared_ptr<const LineStop> lineStop(
						linePhysicalStop.get() ?
						static_pointer_cast<const LineStop, const LinePhysicalStop>(linePhysicalStop) :
						static_pointer_cast<const LineStop, const LineArea>(lineArea)
					);

					lineStopRemoveAction.getAction()->setObjectId(lineStop->getKey());
					lineStopUpdateAction.getAction()->setLineStop(const_pointer_cast<LineStop>(lineStop));

					if(linePhysicalStop.get())
					{
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(linePhysicalStop->getPhysicalStop()->getConnectionPlace())
						);
						openCityRequest.getPage()->setCity(
							Env::GetOfficialEnv().getSPtr(linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getCity())
						);
					}

					if(lineArea.get())
					{
						openDRTAreaRequest.getPage()->setArea(
							Env::GetOfficialEnv().getSPtr(lineArea->getArea())
						);
					}

					stream << t.row();
					stream << t.col() << f.getRadioInput(LineStopAddAction::PARAMETER_RANK, optional<size_t>(lineStop->getRankInPath()), optional<size_t>());
					stream << t.col() << lineStop->getRankInPath();

					if(linePhysicalStop.get())
					{
						stream << t.col() << HTMLModule::getHTMLLink(
							openCityRequest.getURL(),
							linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getCity()->getName()
						);

						stream << t.col() << HTMLModule::getHTMLLink(
							openPlaceRequest.getURL(),
							linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getName()
						);

						// Physical stop
						stream << t.col();
						/*					HTMLForm f2(lineStopUpdateAction.getHTMLForm("quay"+lexical_cast<string>(lineStop->getRankInPath())));
						stream << f2.open();
						stream << f2.getSelectInput(
						LineStopUpdateAction::PARAMETER_PHYSICAL_STOP_ID,
						lineStop->getPhysicalStop()->getConnectionPlace()->getPhysicalStopLabels(),
						optional<RegistryKeyType>(lineStop->getPhysicalStop()->getKey())
						);
						stream << f2.getSubmitButton("OK");
						stream << f2.close();
						*/
						BOOST_FOREACH(const StopArea::PhysicalStops::value_type& ps, linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getPhysicalStops())
						{
							if(ps.second == linePhysicalStop->getPhysicalStop())
							{
								stream << "[";
							}
							lineStopUpdateAction.getAction()->setPhysicalStop(Env::GetOfficialEnv().getEditableSPtr(const_cast<StopPoint*>(ps.second)));
							stream << HTMLModule::getHTMLLink(
								lineStopUpdateAction.getHTMLForm().getURL(),
								ps.second->getName().empty() ? lexical_cast<string>(ps.second->getKey()) : ps.second->getName()
							);
							lineStopUpdateAction.getAction()->setPhysicalStop(shared_ptr<StopPoint>());

							if(ps.second == linePhysicalStop->getPhysicalStop())
							{
								stream << "]";
							}
							stream << " ";
						}
					}

					stream << t.col();
					stream << lineStop->getMetricOffset();

					if(lineArea.get())
					{
						stream << t.col();

						stream << HTMLModule::getHTMLImage(DRTAreaAdmin::ICON, "Zone TAD");

						stream << HTMLModule::getHTMLLink(
							openDRTAreaRequest.getURL(),
							lineArea->getArea()->getName()
						);

						stream << t.col(2);
						stream << "Desserte interne : ";
						AdminActionFunctionRequest<LineStopUpdateAction,JourneyPatternAdmin> internalUpdateRequest(_request);
						internalUpdateRequest.getAction()->setLineStop(const_pointer_cast<LineStop>(lineStop));
						if(lineArea->getInternalService())
						{
							internalUpdateRequest.getAction()->setAllowedInternal(false);
							stream << "[OUI] " << HTMLModule::getHTMLLink(internalUpdateRequest.getURL(), "NON");
						}
						else
						{
							internalUpdateRequest.getAction()->setAllowedInternal(true);
							stream << HTMLModule::getHTMLLink(internalUpdateRequest.getURL(), "OUI") << " [NON]";
						}
					}


					// Allowed arrival
					lineStopUpdateAction.getAction()->setAllowedArrival(optional<bool>(!lineStop->isArrival()));
					stream <<
						t.col() <<
						HTMLModule::getHTMLLink(
							lineStopUpdateAction.getHTMLForm().getURL(),
							(lineStop->isArrivalAllowed() ? HTMLModule::getHTMLImage("bullet_green.png","Arrivée possible") : HTMLModule::getHTMLImage("bullet_white.png", "Arrivée impossible"))
						);
					lineStopUpdateAction.getAction()->setAllowedArrival(optional<bool>());

					// Allowed departure
					lineStopUpdateAction.getAction()->setAllowedDeparture(optional<bool>(!lineStop->isDeparture()));
					stream <<
						t.col() <<
						HTMLModule::getHTMLLink(
							lineStopUpdateAction.getHTMLForm().getURL(),
							(lineStop->isDepartureAllowed() ? HTMLModule::getHTMLImage("bullet_green.png", "Départ possible") : HTMLModule::getHTMLImage("bullet_white.png", "Départ impossible"))
						);
					lineStopUpdateAction.getAction()->setAllowedDeparture(optional<bool>());

					// Scheduled stop
					stream << t.col();
					if(	lineArea.get() ||
						linePhysicalStop.get() && linePhysicalStop->getScheduleInput()
					){
						stream << HTMLModule::getHTMLImage("time.png", "Horaire fourni à cet arrêt");
					}
					else
					{
						stream << HTMLModule::getHTMLImage("ftv2vertline.png", "Horaire non fourni à cet arrêt");
					}

// 					if (reservation)
// 						stream << t.col() << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation obligatoire au départ de cet arrêt");
					stream << t.col() << HTMLModule::getLinkButton(lineStopRemoveAction.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
				}

				if(sservices.empty() && cservices.empty())
				{
					stream << t.row();
					stream << t.col(1,string(),false,string(),2) << f.getRadioInput(LineStopAddAction::PARAMETER_RANK, optional<size_t>(_line->getEdges().size()), optional<size_t>());
					stream << t.col(1,string(),false,string(),2) << _line->getEdges().size();
					stream << t.col() << f.getTextInput(LineStopAddAction::PARAMETER_CITY_NAME, string(), "(localité)");
					stream << t.col() << f.getTextInput(LineStopAddAction::PARAMETER_STOP_NAME, string(), "(arrêt)");
					stream << t.col(1,string(),false,string(),2);
					stream << t.col(1,string(),false,string(),2);
					stream << t.col(1,string(),false,string(),2);
					stream << t.col(1,string(),false,string(),2);
					stream << t.col(1,string(),false,string(),2);
					stream << t.col(1,string(),false,string(),2) << f.getSubmitButton("Ajouter");
					stream << t.row();
					stream << t.col(2) << "ou zone TAD n°" << f.getTextInput(LineStopAddAction::PARAMETER_AREA, string());
				}

				stream << t.close();
				if(sservices.empty() && cservices.empty())
				{
					stream << f.close();
				}


				Envelope e;
				shared_ptr<geos::geom::Point> center;
				BOOST_FOREACH(const Path::Edges::value_type& edge, _line->getEdges())
				{
					if(edge->getFromVertex()->hasGeometry())
					{
						e.expandToInclude(*edge->getFromVertex()->getGeometry()->getCoordinate());
					}
				}
				if(!e.isNull())
				{
					Coordinate c;
					e.centre(c);
					center.reset(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(c));
				}


				if(center.get())
				{
					stream << "<h1>Carte</h1>";
					HTMLMap map(*center, 12, true, false, false);
					StaticActionRequest<LineStopUpdateAction> lineStopUpdateRequest(_request);
					StaticActionRequest<StopPointUpdateAction> stopPointUpdateRequest(_request);
					for(Path::Edges::const_iterator itEdge(_line->getEdges().begin()); itEdge!=_line->getEdges().end(); ++itEdge)
					{
						if(dynamic_cast<DesignatedLinePhysicalStop*>(*itEdge))
						{
							lineStopUpdateRequest.getAction()->setLineStop(
								static_pointer_cast<LineStop, DesignatedLinePhysicalStop>(
									Env::GetOfficialEnv().getEditableSPtr(static_cast<DesignatedLinePhysicalStop*>(*itEdge))
							)	);
							stopPointUpdateRequest.getAction()->setStop(
								Env::GetOfficialEnv().getEditableSPtr(static_cast<StopPoint*>((*itEdge)->getFromVertex()))
							);
						}
						if(itEdge+1 != _line->getEdges().end())
						{
							shared_ptr<LineString> geom((*itEdge)->getRealGeometry());
							if(geom.get())
							{
								map.addLineString(
									HTMLMap::MapLineString(
										*geom,
										dynamic_cast<DesignatedLinePhysicalStop*>(*itEdge) ? lineStopUpdateRequest.getURL() : string()
								)	);
							}
						}
						if((*itEdge)->getFromVertex()->getGeometry().get())
						{
							map.addPoint(
								HTMLMap::MapPoint(
									*(*itEdge)->getFromVertex()->getGeometry(),
									"arret-rouge-blanc-8px.png",
									"arret-rouge-blanc-8px.png",
									"arret-rouge-blanc-8px.png",
									dynamic_cast<DesignatedLinePhysicalStop*>(*itEdge) ? stopPointUpdateRequest.getURL() : string(),
									dynamic_cast<DesignatedLinePhysicalStop*>(*itEdge) ?
										static_cast<StopPoint*>((*itEdge)->getFromVertex())->getConnectionPlace()->getFullName() :
										string(),
									10, 10
							)	);
						}
					}

					map.draw(stream);
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULED SERVICES
			if (openTabContent(stream, TAB_SCHEDULED_SERVICES))
			{
				stream << "<h1>Services à horaire</h1>";

				AdminFunctionRequest<JourneyPatternAdmin> searchRequest(_request);
				HTMLForm sortedForm(searchRequest.getHTMLForm());

				AdminActionFunctionRequest<ServiceAddAction, JourneyPatternAdmin> newRequest(_request);
				newRequest.getAction()->setLine(const_pointer_cast<JourneyPattern>(_line));
				newRequest.getAction()->setIsContinuous(false);

				AdminActionFunctionRequest<RemoveObjectAction, JourneyPatternAdmin> removeRequest(_request);

				ActionResultHTMLTable::HeaderVector vs;
				vs.push_back(make_pair(string(), "Num"));
				vs.push_back(make_pair(string(), "Numéro"));
				vs.push_back(make_pair(ScheduledServiceTableSync::COL_SCHEDULES, "Départ"));
				vs.push_back(make_pair(string(), "Arrivée"));
				vs.push_back(make_pair(string(), "Durée"));
				vs.push_back(make_pair(string(), "Dernier jour"));
				vs.push_back(make_pair(string(), "Actions"));
				vs.push_back(make_pair(string(), "Actions"));
				ActionResultHTMLTable ts(vs, sortedForm, _requestParameters, sservices, newRequest.getHTMLForm(), ServiceAddAction::PARAMETER_TEMPLATE_ID);

				AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

				stream << ts.open();

				size_t i(0);
				BOOST_FOREACH(shared_ptr<ScheduledService> service, sservices)
				{
					serviceRequest.getPage()->setService(service);
					removeRequest.getAction()->setObjectId(service->getKey());

					string number("S"+ lexical_cast<string>(i++));
					services[service.get()] = number;

					time_duration ds(service->getDepartureSchedule(false, 0));
					time_duration as(service->getLastArrivalSchedule(false));

					stream << ts.row(lexical_cast<string>(service->getKey()));

					stream << ts.col() << number;

					stream << ts.col() << service->getServiceNumber();

					stream << ts.col() << ds;
					stream << ts.col() << as;

					stream << ts.col() << (as - ds);

					serviceRequest.getPage()->setActiveTab(ServiceAdmin::TAB_CALENDAR);
					stream << ts.col() << HTMLModule::getHTMLLink(serviceRequest.getURL(), to_iso_extended_string(service->getLastActiveDate()));

					serviceRequest.getPage()->setActiveTab(string());
					stream << ts.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), ServiceAdmin::ICON);

					stream << ts.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le service "+ service->getServiceNumber() +" ?");
				}

				stream << ts.row();
				stream << ts.col() << "S" << i;
				stream << ts.col() << ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_NUMBER, string());
				stream << ts.col() << ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_START_DEPARTURE_TIME, string());
				stream << ts.col(3) << "cadence : " << ts.getActionForm().getSelectNumberInput(ServiceAddAction::PARAMETER_PERIOD, 0, 120, 0, 1, string(), "non") << " " <<
					ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_END_DEPARTURE_TIME, string(), "(fin cadence)");
				stream << ts.col(2) << ts.getActionForm().getSubmitButton("Créer");

				stream << ts.close();

				stream << "<h1>Mise à jour collective des calendriers</h1>";

				stream << "<p class=\"info\">Cette fonction met également à jour les services continus</p>";

				date now(day_clock::local_day());
				AdminActionFunctionRequest<ServiceApplyCalendarAction, JourneyPatternAdmin> updateRequest(_request);
				updateRequest.getAction()->setJourneyPattern(_line);
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
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTINUOUS SERVICES
			if (openTabContent(stream, TAB_CONTINUOUS_SERVICES))
			{
				AdminFunctionRequest<JourneyPatternAdmin> searchRequest(_request);
				HTMLForm sortedForm(searchRequest.getHTMLForm());

				AdminActionFunctionRequest<ServiceAddAction, JourneyPatternAdmin> newRequest(_request);
				newRequest.getAction()->setLine(const_pointer_cast<JourneyPattern>(_line));
				newRequest.getAction()->setIsContinuous(true);

				AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

				AdminActionFunctionRequest<RemoveObjectAction, JourneyPatternAdmin> removeRequest(_request);

				ActionResultHTMLTable::HeaderVector vc;
				vc.push_back(make_pair(string(), "Num"));
				vc.push_back(make_pair(ScheduledServiceTableSync::COL_SCHEDULES, "Départ premier"));
				vc.push_back(make_pair(string(), "Départ dernier"));
				vc.push_back(make_pair(string(), "Arrivée premier"));
				vc.push_back(make_pair(string(), "Arrivée dernier"));
				vc.push_back(make_pair(string(), "Durée"));
				vc.push_back(make_pair(string(), "Amplitude"));
				vc.push_back(make_pair(string(), "Fréquence"));
				vc.push_back(make_pair(string(), "Dernier jour"));
				vc.push_back(make_pair(string(), "Actions"));
				vc.push_back(make_pair(string(), "Actions"));

				ActionResultHTMLTable tc(vc, sortedForm, _requestParameters, cservices, newRequest.getHTMLForm(), ServiceAddAction::PARAMETER_TEMPLATE_ID);

				stream << tc.open();

				size_t i(0);
				BOOST_FOREACH(shared_ptr<ContinuousService> service, cservices)
				{
					serviceRequest.getPage()->setService(service);
					removeRequest.getAction()->setObjectId(service->getKey());

					string number("C"+ lexical_cast<string>(i++));
					services[service.get()] = number;

					stream << tc.row(lexical_cast<string>(service->getKey()));

					stream << tc.col() << number;

					time_duration ds(service->getDepartureSchedule(false, 0));
					time_duration as(service->getLastArrivalSchedule(false));

					stream << tc.col() << ds;
					ds += service->getRange();
					stream << tc.col() << ds;

					stream << tc.col() << as;
					as += service->getRange();
					stream << tc.col() << as;

					stream << tc.col() << (as - ds);

					stream << tc.col() << service->getRange();
					stream << tc.col() << service->getMaxWaitingTime();

					stream << tc.col() << to_iso_extended_string(service->getLastActiveDate());

					stream << tc.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), ServiceAdmin::ICON);

					stream << tc.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le service ?");
				}

				stream << tc.row();
				stream << tc.col() << "C" << i;
				stream << tc.col() << tc.getActionForm().getTextInput(ServiceAddAction::PARAMETER_START_DEPARTURE_TIME, string());
				stream << tc.col() << tc.getActionForm().getTextInput(ServiceAddAction::PARAMETER_END_DEPARTURE_TIME, string());
				stream << tc.col(4);
				stream << tc.col() << tc.getActionForm().getSelectNumberInput(ServiceAddAction::PARAMETER_PERIOD, 1, 120, 5);
				stream << tc.col(2) << tc.getActionForm().getSubmitButton("Créer");

				stream << tc.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				map<optional<bool>, string> waybackMap;
				waybackMap.insert(make_pair(false, "Aller"));
				waybackMap.insert(make_pair(true, "Retour"));

				AdminActionFunctionRequest<JourneyPatternUpdateAction,JourneyPatternAdmin> updateRequest(_request);
				updateRequest.getAction()->setRoute(const_pointer_cast<JourneyPattern>(_line));
				PropertiesHTMLTable p(updateRequest.getHTMLForm());
				stream << p.open();
				stream << p.cell(
					"Nom",
					p.getForm().getTextInput(JourneyPatternUpdateAction::PARAMETER_NAME, _line->getName())
				);
				stream << p.cell(
					"Mode de transport",
					p.getForm().getSelectInput(
						JourneyPatternUpdateAction::PARAMETER_TRANSPORT_MODE_ID,
						RollingStockTableSync::GetLabels(),
						_line->getRollingStock() ? _line->getRollingStock()->getKey() : optional<RegistryKeyType>()
				)	);
				stream << p.cell(
					"Direction (texte)",
					p.getForm().getTextInput(JourneyPatternUpdateAction::PARAMETER_DIRECTION, _line->getDirection())
				);
				stream << p.cell(
					"Direction (girouette)",
					p.getForm().getTextInput(JourneyPatternUpdateAction::PARAMETER_DIRECTION_ID, _line->getDirectionObj() ? lexical_cast<string>(_line->getDirectionObj()->getKey()) : string())
				);
				stream << p.cell(
					"Sens",
					p.getForm().getRadioInputCollection(
						JourneyPatternUpdateAction::PARAMETER_WAYBACK,
						waybackMap,
						optional<bool>(_line->getWayBack())
				)	);
				stream << p.close();

				PTRuleUserAdmin<JourneyPattern,JourneyPatternAdmin>::Display(stream, _line, _request);

				StaticActionRequest<JourneyPatternUpdateAction> updateOnlyRequest(_request);
				updateOnlyRequest.getAction()->setRoute(const_pointer_cast<JourneyPattern>(_line));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_line, updateOnlyRequest);
			}

			////////////////////////////////////////////////////////////////////
			// TAB INDICES
			if (openTabContent(stream, TAB_INDICES))
			{
				stream << "<style>td.red {background-color:red;width:8px; height:8px; color:white; text-align:center; } td.green {background-color:#008000;width:10px; height:10px; color:white; text-align:center; } .mini {font-size:9px;}</style>";
				HTMLTable::ColsVector cols;
				cols.push_back("Arrêt");
				cols.push_back("D/A");
				for(int i(0); i<=23; ++i)
				{
					cols.push_back(Conversion::ToString(i));
				}
				HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const Path::Edges::value_type& edge, _line->getEdges())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));
					const DesignatedLinePhysicalStop* linePhysicalStop(dynamic_cast<const DesignatedLinePhysicalStop*>(edge));
					const LineArea* lineArea(dynamic_cast<const LineArea*>(edge));
					lineStop.getDepartureFromIndex(false,0);
					lineStop.getDepartureFromIndex(true,0);

					if(lineStop.isArrival())
					{
						stream << t.row();
						stream << t.col(1, string(), true);
						if(linePhysicalStop)
						{
							stream << linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getFullName();
						}
						if(lineArea)
						{
							stream << lineArea->getArea()->getName();
						}
						stream << t.col(1, string(), true) << "A";

						BOOST_FOREACH(const Edge::ArrivalServiceIndices::value_type& index, lineStop.getArrivalIndices())
						{
							stream << t.col();

							if(index.get(false) == lineStop.getParentPath()->getServices().rend())
							{
								stream << "-";
							}
							else
							{
								const Service* service(*index.get(false));
								stream << services[service];
								stream << "<br /><span class=\"mini\">" << service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span>";
							}
						}
					}

					if(lineStop.isDeparture())
					{
						stream << t.row();
						stream << t.col(1, string(), true);
						if(linePhysicalStop)
						{
							stream << linePhysicalStop->getPhysicalStop()->getConnectionPlace()->getFullName();
						}
						if(lineArea)
						{
							stream << lineArea->getArea()->getName();
						}
						stream << t.col(1, string(), true) << "D";

						BOOST_FOREACH(const Edge::DepartureServiceIndices::value_type& index, lineStop.getDepartureIndices())
						{
							stream << t.col();

							if(index.get(false) == lineStop.getParentPath()->getServices().end())
							{
								stream << "-";
							}
							else
							{
								const Service* service(*index.get(false));
								stream << services[service];
								stream << "<br /><span class=\"mini\">" << service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span>";
							}
						}
					}
				}
				stream << t.close();

			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		bool JourneyPatternAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_line.get() == NULL) return false;
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		std::string JourneyPatternAdmin::getTitle() const
		{
			return _line.get() ? "Route " + _line->getName() : DEFAULT_TITLE;
		}



		boost::shared_ptr<const JourneyPattern> JourneyPatternAdmin::getLine() const
		{
			return _line;
		}



		void JourneyPatternAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Arrêts desservis", TAB_STOPS, true));
			_tabs.push_back(Tab("Services à horaire", TAB_SCHEDULED_SERVICES, true, ServiceAdmin::ICON));
			_tabs.push_back(Tab("Services continus", TAB_CONTINUOUS_SERVICES, true, ServiceAdmin::ICON));
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true, "application_form.png"));
			_tabs.push_back(Tab("Index", TAB_INDICES, true));

			_tabBuilded = true;
		}



		void JourneyPatternAdmin::setLine(boost::shared_ptr<const JourneyPattern> value)
		{
			_line = value;
		}



		bool JourneyPatternAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _line->getKey() == static_cast<const JourneyPatternAdmin&>(other)._line->getKey();
		}



		AdminInterfaceElement::PageLinks JourneyPatternAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				ScheduledServiceTableSync::SearchResult services(
					ScheduledServiceTableSync::Search(*_env, _line->getKey())
				);
				BOOST_FOREACH(shared_ptr<const ScheduledService> service, services)
				{
					shared_ptr<ServiceAdmin> p(
						getNewPage<ServiceAdmin>()
					);
					p->setService(service);
					links.push_back(p);
				}
				ContinuousServiceTableSync::SearchResult cservices(
					ContinuousServiceTableSync::Search(*_env, _line->getKey())
				);
				BOOST_FOREACH(shared_ptr<const ContinuousService> service, cservices)
				{
					shared_ptr<ServiceAdmin> p(
						getNewPage<ServiceAdmin>()
					);
					p->setService(service);
					links.push_back(p);
				}
			}
			return links;

		}



		AdminInterfaceElement::PageLinks JourneyPatternAdmin::_getCurrentTreeBranch() const
		{
			shared_ptr<CommercialLineAdmin> p(
				getNewPage<CommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_line->getCommercialLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}
	}
}
