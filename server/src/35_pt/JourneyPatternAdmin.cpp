////////////////////////////////////////////////////////////////////////////////
/// JourneyPatternAdmin class implementation.
///	@file JourneyPatternAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "JourneyPatternAdmin.hpp"

#include "ActionResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "BaseCalendarAdmin.hpp"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "GeometryField.hpp"
#include "HTMLModule.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternRankContinuityRestoreAction.hpp"
#include "JourneyPatternUpdateAction.hpp"
#include "LineStopTableSync.h"
#include "MalformedSchedulesException.hpp"
#include "ObjectUpdateAction.hpp"
#include "Profile.h"
#include "PTModule.h"
#include "PTPlaceAdmin.h"
#include "Request.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ServiceAddAction.h"
#include "ServiceAdmin.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "TransportNetworkRight.h"
#include "User.h"
#include "PTRuleUserAdmin.hpp"
#include "PropertiesHTMLTable.h"
#include "RollingStockTableSync.hpp"
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
#include "AdminModule.h"
#include "CalendarTemplateTableSync.h"
#include "Destination.hpp"
#include "DestinationTableSync.hpp"
#include "MapSource.hpp"
#include "CityListFunction.h"
#include "PlacesListFunction.h"
#include "DRTAreaTableSync.hpp"

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
	using namespace html;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace vehicle;
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
			_requestParameters.setFromParametersMap(map, ServiceSchedules::FIELD.name);
		}



		util::ParametersMap JourneyPatternAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_line.get()) m.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			return m;
		}



		void JourneyPatternAdmin::display(
			ostream& stream,
			const server::Request& _request
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
					_requestParameters.orderField == ServiceSchedules::FIELD.name,
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
					_requestParameters.orderField == ServiceSchedules::FIELD.name,
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

				AdminActionFunctionRequest<RemoveObjectAction,JourneyPatternAdmin> lineStopRemoveAction(_request, *this);

				AdminActionFunctionRequest<LineStopAddAction,JourneyPatternAdmin> lineStopAddAction(_request, *this);
				lineStopAddAction.getAction()->setRoute(const_pointer_cast<JourneyPattern>(_line));
				lineStopAddAction.getAction()->setWithSchedules(sservices.empty() && cservices.empty());
				HTMLForm f(lineStopAddAction.getHTMLForm("add_journey_pattern_stop"));

				AdminActionFunctionRequest<LineStopUpdateAction,JourneyPatternAdmin> lineStopUpdateAction(_request, *this);

				HTMLTable::ColsVector v;
				v.push_back("Rang");
				v.push_back("Rang");
				v.push_back("Localité");
				v.push_back("Arrêt");
				v.push_back("Quai");
				v.push_back("pm");
				v.push_back("A");
				v.push_back("D");
				v.push_back("H");
				v.push_back("R");
				v.push_back("Action");
				HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

				stream << f.open();
				stream << t.open();

				size_t expectedRank(0);
				bool rankOk(true);
				for(JourneyPattern::LineStops::const_iterator it(_line->getLineStops().begin()); it != _line->getLineStops().end(); ++it)
				{
					boost::shared_ptr<const LineStop> lineStop(
						Env::GetOfficialEnv().getSPtr(*it)
					);

					// Rank check
					if(lineStop->get<RankInPath>() != expectedRank)
					{
						rankOk = false;
					}
					++expectedRank;

					// Place / area
					StopPoint* stopPoint(
						dynamic_cast<StopPoint*>(&*lineStop->get<LineNode>())
					);
					DRTArea* area(
						dynamic_cast<DRTArea*>(&*lineStop->get<LineNode>())
					);

					lineStopRemoveAction.getAction()->setObjectId(lineStop->getKey());
					lineStopUpdateAction.getAction()->setLineStop(const_pointer_cast<LineStop>(lineStop));

					if(	stopPoint &&
						stopPoint->getConnectionPlace()
					){
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(stopPoint->getConnectionPlace())
						);
						if(stopPoint->getConnectionPlace()->getCity())
						{
							openCityRequest.getPage()->setCity(
								Env::GetOfficialEnv().getSPtr(stopPoint->getConnectionPlace()->getCity())
							);
						}
					}

					if(area)
					{
						openDRTAreaRequest.getPage()->setArea(
							Env::GetOfficialEnv().getSPtr(area)
						);
					}

					stream << t.row();

					// Rank selector
					stream << t.col() << f.getRadioInput(LineStopAddAction::PARAMETER_RANK, optional<size_t>(lineStop->get<RankInPath>()), optional<size_t>());

					// Rank
					stream << t.col() << lineStop->get<RankInPath>();

					if(stopPoint)
					{
						// City
						stream << t.col();
						if(	stopPoint->getConnectionPlace() &&
							stopPoint->getConnectionPlace()->getCity()
						){
							stream << HTMLModule::getHTMLLink(
								openCityRequest.getURL(),
								stopPoint->getConnectionPlace()->getCity()->getName()
							);
						}
						else
						{
							stream << "Commune inconnue";
						}

						// Stop area
						stream << t.col();
						if(	stopPoint->getConnectionPlace()
						){
							stream << HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								stopPoint->getConnectionPlace()->getName()
							);
						}
						else
						{
							stream << "Zone d'arrêt inconnue";
						}

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
						BOOST_FOREACH(const StopArea::PhysicalStops::value_type& ps, stopPoint->getConnectionPlace()->getPhysicalStops())
						{
							stream << "<span title=\"" << ps.second->getCodeBySources() << "\">";
							if(ps.second == stopPoint)
							{
								stream << "[";
							}
							lineStopUpdateAction.getAction()->setPhysicalStop(Env::GetOfficialEnv().getEditableSPtr(const_cast<StopPoint*>(ps.second)));
							stream << HTMLModule::getHTMLLink(
								lineStopUpdateAction.getHTMLForm().getURL(),
								ps.second->getName().empty() ? lexical_cast<string>(ps.second->getKey()) : ps.second->getName()
							);
							lineStopUpdateAction.getAction()->setPhysicalStop(boost::shared_ptr<StopPoint>());

							if(ps.second == stopPoint)
							{
								stream << "]";
							}
							stream << "</span> ";
						}
					

						// Metric offset
						stream << t.col();
						stream << lineStop->get<MetricOffsetField>();
						JourneyPattern::LineStops::const_iterator it2(it);
						++it2;
						if(	it2 != _line->getLineStops().end() &&
							lineStop->get<LineStringGeometry>() &&
							(*it2)->get<MetricOffsetField>() - lineStop->get<MetricOffsetField>() != floor(lineStop->get<LineStringGeometry>()->getLength())
						){
							lineStopUpdateAction.getAction()->setReadLengthFromGeometry(true);
							stream <<
								HTMLModule::getHTMLLink(
									lineStopUpdateAction.getHTMLForm().getURL(),
									HTMLModule::getHTMLImage("/admin/img/cog.png", "Générer distance depuis géométrie")
								);
							lineStopUpdateAction.getAction()->setReadLengthFromGeometry(false);
						}
						lineStopUpdateAction.getAction()->setClearGeom(true);
						stream <<
							HTMLModule::getHTMLLink(
								lineStopUpdateAction.getHTMLForm().getURL(),
								HTMLModule::getHTMLImage("/admin/img/cross.png", "Supprimer géométrie"),
								"Etes-vous sûr de vouloir supprimer la géométrie ?"
							);
						lineStopUpdateAction.getAction()->setClearGeom(false);
					}

					// DRT area
					if(area)
					{
						// Area
						stream << t.col();
						stream << HTMLModule::getHTMLImage("/admin/img/" + DRTAreaAdmin::ICON, "Zone TAD");
						stream << HTMLModule::getHTMLLink(
							openDRTAreaRequest.getURL(),
							area->getName()
						);
					
						// Internal service
						stream << t.col(2);
						stream << "Desserte interne : ";
						AdminActionFunctionRequest<LineStopUpdateAction,JourneyPatternAdmin> internalUpdateRequest(_request, *this);
						internalUpdateRequest.getAction()->setLineStop(const_pointer_cast<LineStop>(lineStop));
						if(lineStop->get<InternalService>())
						{
							internalUpdateRequest.getAction()->setAllowedInternal(false);
							stream << "[OUI] " << HTMLModule::getHTMLLink(internalUpdateRequest.getURL(), "NON");
						}
						else
						{
							internalUpdateRequest.getAction()->setAllowedInternal(true);
							stream << HTMLModule::getHTMLLink(internalUpdateRequest.getURL(), "OUI") << " [NON]";
						}

						// Default direction
						lineStopUpdateAction.getAction()->setReverseDRTArea(!lineStop->get<ReverseDRTArea>());
						stream <<
							t.col() <<
							HTMLModule::getHTMLLink(
								lineStopUpdateAction.getHTMLForm().getURL(),
								(lineStop->get<ReverseDRTArea>() ? HTMLModule::getHTMLImage("/admin/img/arrow_up.png","Trajet inversé") : HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "Trajet non inversé"))
							);
						lineStopUpdateAction.getAction()->setReverseDRTArea(lineStop->get<ReverseDRTArea>());

					}

					// Allowed arrival
					lineStopUpdateAction.getAction()->setAllowedArrival(optional<bool>(!lineStop->get<IsArrival>()));
					stream <<
						t.col() <<
						HTMLModule::getHTMLLink(
							lineStopUpdateAction.getHTMLForm().getURL(),
							(lineStop->get<IsArrival>() ? HTMLModule::getHTMLImage("/admin/img/bullet_green.png","Arrivée possible") : HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Arrivée impossible"))
						);
					lineStopUpdateAction.getAction()->setAllowedArrival(optional<bool>());

					// Allowed departure
					lineStopUpdateAction.getAction()->setAllowedDeparture(optional<bool>(!lineStop->get<IsDeparture>()));
					stream <<
						t.col() <<
						HTMLModule::getHTMLLink(
							lineStopUpdateAction.getHTMLForm().getURL(),
							(lineStop->get<IsDeparture>() ? HTMLModule::getHTMLImage("/admin/img/bullet_green.png", "Départ possible") : HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Départ impossible"))
						);
					lineStopUpdateAction.getAction()->setAllowedDeparture(optional<bool>());

					// Scheduled stop
					stream << t.col();
					if(	area)
					{
						stream << HTMLModule::getHTMLImage("/admin/img/time.png", "Horaire fourni (zonal)");
					}
					else if(stopPoint)
					{
						string icon(
							lineStop->get<ScheduleInput>() ?
							HTMLModule::getHTMLImage("/admin/img/time.png", "Horaire fourni à cet arrêt") :
							HTMLModule::getHTMLImage("/admin/img/ftv2vertline.png", "Horaire non fourni à cet arrêt")
						);

						if(	lineStop->get<RankInPath>() != 0 &&
							lineStop->get<RankInPath>() != (*_line->getLineStops().rbegin())->get<RankInPath>()
						){
							lineStopUpdateAction.getAction()->setWithSchedules(!lineStop->get<ScheduleInput>());
							stream << HTMLModule::getHTMLLink(lineStopUpdateAction.getHTMLForm().getURL(), icon);
							lineStopUpdateAction.getAction()->setWithSchedules(optional<bool>());
						}
						else
						{
							stream << icon;
						}
					}

					// Reservation
					stream << t.col();
					if(	area)
					{
						stream << HTMLModule::getHTMLImage("/admin/img/resa_compulsory.png", "Règle de réservation applicable à cette zone");
					}
					else if(stopPoint)
					{
						string icon(
							lineStop->get<ReservationNeeded>() ?
							HTMLModule::getHTMLImage("/admin/img/resa_compulsory.png", "Règle de réservation applicable à cet arrêt") :
							HTMLModule::getHTMLImage("/admin/img/ftv2vertline.png", "Arrêt sans réservation")
						);

						lineStopUpdateAction.getAction()->setReservationNeeded(!lineStop->get<ReservationNeeded>());
						stream << HTMLModule::getHTMLLink(lineStopUpdateAction.getHTMLForm().getURL(), icon);
						lineStopUpdateAction.getAction()->setReservationNeeded(optional<bool>());
					}

					stream << t.col() << HTMLModule::getLinkButton(lineStopRemoveAction.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
				}

				// Stop add form
				stream << t.row();
				stream << t.col(1,string(),false,string(),2) << f.getRadioInput(LineStopAddAction::PARAMETER_RANK, optional<size_t>(_line->getLineStops().size()), optional<size_t>());
				stream << t.col(1,string(),false,string(),2) << _line->getLineStops().size();

				stream << t.col() << f.getTextInputAutoCompleteFromService(
					LineStopAddAction::PARAMETER_CITY_NAME,
					string(),
					string(),
					pt_website::CityListFunction::FACTORY_KEY,
					pt_website::CityListFunction::DATA_CITIES,
					pt_website::CityListFunction::DATA_CITY,
					string(),string(),
					false, false, false
				);

				stream << t.col() << f.getTextInputAutoCompleteFromService(
					LineStopAddAction::PARAMETER_STOP_NAME,
					string(),
					string(),
					pt_website::PlacesListFunction::FACTORY_KEY,
					pt_website::PlacesListFunction::DATA_PLACES,
					pt_website::PlacesListFunction::DATA_PLACE,
					"ct",LineStopAddAction::PARAMETER_CITY_NAME,
					false, false, false
				);

				stream << t.col(1,string(),false,string(),2);
				if(!_line->getAllServices().empty())
				{
					stream << "Durée du trajet nouveau : " << f.getTextInput(LineStopAddAction::PARAMETER_DURATION_TO_ADD, string());
				}

				stream << t.col(1,string(),false,string(),2);
				stream << t.col(1,string(),false,string(),2);
				stream << t.col(1,string(),false,string(),2);
				stream << t.col(1,string(),false,string(),2);
				stream << t.col(1,string(),false,string(),2) << f.getSubmitButton("Ajouter");
				stream << t.row();
				stream << t.col(2) << "ou zone TAD ";
				stream << f.getTextInputAutoCompleteFromTableSync(
					LineStopAddAction::PARAMETER_AREA,
					string(),
					string(),
					lexical_cast<string>(DRTAreaTableSync::TABLE.ID),
					string(),string(),
					false, false, true, false
				);

				stream << t.close();
				stream << f.close();

				// Rank check failed
				if(!rankOk)
				{
					AdminActionFunctionRequest<JourneyPatternRankContinuityRestoreAction, JourneyPatternAdmin> fixRequest(_request, *this);
					fixRequest.getAction()->setJourneyPattern(_line);
					stream <<
						"<p class=\"info\">Les rangs des arrêts sont dicontinus. Cela constitue une corruption de la base de données. La " <<
						HTMLModule::getHTMLLink(
							fixRequest.getURL(),
							"fonction de réparation des rangs"
						) << " permet de réparer ce problème.</p>";
				}


				Envelope e;
				boost::shared_ptr<geos::geom::Point> center;
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
					std::string editFieldName(ObjectUpdateAction::GetInputName<LineStringGeometry>());
					HTMLMap map(
						*center,
						12,
						editFieldName,
						false,
						false,
						false
					);
					map.setMapSource(MapSource::GetSessionMapSource(*_request.getSession()));

					StaticActionRequest<LineStopUpdateAction> lineStopUpdateRequest(_request);
					StaticActionRequest<StopPointUpdateAction> stopPointUpdateRequest(_request);
					for(JourneyPattern::LineStops::const_iterator itEdge(_line->getLineStops().begin()); itEdge!=_line->getLineStops().end(); ++itEdge)
					{
						lineStopUpdateRequest.getAction()->setLineStop(
							Env::GetOfficialEnv().getEditableSPtr(*itEdge)
						);

						StopPoint* stopPoint(
							dynamic_cast<StopPoint*>(&*(*itEdge)->get<LineNode>())
						);
						DRTArea* area(
							dynamic_cast<DRTArea*>(&*(*itEdge)->get<LineNode>())
						);

						JourneyPattern::LineStops::const_iterator it2(itEdge);
						++it2;
						if(it2 != _line->getLineStops().end())
						{
							boost::shared_ptr<LineString> geom((*itEdge)->get<LineStringGeometry>());
							if(!geom && (*itEdge)->getGeneratedLineStops().size() == 1)
							{
								geom = (*(*itEdge)->getGeneratedLineStops().begin())->getRealGeometry();
							}
							if(geom)
							{
								map.addLineString(
									HTMLMap::MapLineString(
										*geom,
										lineStopUpdateRequest.getURL()
								)	);
							}
						}

						if(stopPoint)
						{
							stopPointUpdateRequest.getAction()->setStop(
								Env::GetOfficialEnv().getEditableSPtr(stopPoint)
							);
							if(stopPoint->getGeometry())
							{
								map.addPoint(
									HTMLMap::MapPoint(
										*stopPoint->getGeometry(),
										"/admin/img/arret-rouge-blanc-8px.png",
										"/admin/img/arret-rouge-blanc-8px.png",
										"/admin/img/arret-rouge-blanc-8px.png",
										stopPointUpdateRequest.getURL(),
										stopPoint->getConnectionPlace()->getFullName(),
										10, 10
								)	);
							}
						}
						else if(area)
						{
							BOOST_FOREACH(StopArea* stop, area->get<Stops>())
							{
								if(stop->getPoint())
								{
									map.addPoint(
										HTMLMap::MapPoint(
											*stop->getPoint(),
											"/admin/img/arret-rouge-blanc-8px.png",
											"/admin/img/arret-rouge-blanc-8px.png",
											"/admin/img/arret-rouge-blanc-8px.png",
											string(),
											stop->getFullName() + " (TAD)",
											10, 10
									)	);
								}
							}
						}
					}

					map.draw(stream, _request);
				}
			}


			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULED SERVICES
			if (openTabContent(stream, TAB_SCHEDULED_SERVICES))
			{
				stream << "<h1>Services à horaire</h1>";

				AdminFunctionRequest<JourneyPatternAdmin> searchRequest(_request, *this);
				HTMLForm sortedForm(searchRequest.getHTMLForm());

				AdminActionFunctionRequest<ServiceAddAction, JourneyPatternAdmin> newRequest(_request, *this);
				newRequest.getAction()->setLine(const_pointer_cast<JourneyPattern>(_line));
				newRequest.getAction()->setIsContinuous(false);

				AdminActionFunctionRequest<RemoveObjectAction, JourneyPatternAdmin> removeRequest(_request, *this);

				ActionResultHTMLTable::HeaderVector vs;
				vs.push_back(make_pair(string(), "Num"));
				vs.push_back(make_pair(string(), "Numéro"));
				vs.push_back(make_pair(ServiceSchedules::FIELD.name, "Départ"));
				vs.push_back(make_pair(string(), "Arrivée"));
				vs.push_back(make_pair(string(), "Durée"));
				vs.push_back(make_pair(string(), "Dernier jour"));
				vs.push_back(make_pair(string(), "Actions"));
				vs.push_back(make_pair(string(), "Actions"));
				ActionResultHTMLTable ts(vs, sortedForm, _requestParameters, sservices, newRequest.getHTMLForm(), ServiceAddAction::PARAMETER_TEMPLATE_ID);

				AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

				stream << ts.open();

				size_t i(0);
				BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, sservices)
				{
					serviceRequest.getPage()->setService(service);
					removeRequest.getAction()->setObjectId(service->getKey());

					string number("S"+ lexical_cast<string>(i++));
					services[service.get()] = number;

					stream << ts.row(lexical_cast<string>(service->getKey()));

					stream << ts.col() << number;

					stream << ts.col() << service->getServiceNumber();

					try
					{
						recursive_mutex::scoped_lock lock(service->getSchedulesMutex());
						time_duration ds(service->getDepartureSchedule(false, 0));
						time_duration as(service->getLastArrivalSchedule(false));

						stream << ts.col() << ds;
						stream << ts.col() << as;

						stream << ts.col() << (as - ds);
					}
					catch(MalformedSchedulesException&)
					{
						stream << ts.col(3) << "Horaires corrompus";
					}

					serviceRequest.getPage()->setActiveTab(ServiceAdmin::TAB_CALENDAR);
					stream << ts.col() << HTMLModule::getHTMLLink(serviceRequest.getURL(), to_iso_extended_string(service->getLastActiveDate()));

					serviceRequest.getPage()->setActiveTab(string());
					stream << ts.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), "/admin/img/" + ServiceAdmin::ICON);

					stream << ts.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le service "+ service->getServiceNumber() +" ?");
				}

				stream << ts.row();
				stream << ts.col() << "S" << i;
				stream << ts.col() << ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_NUMBER, string());
				stream << ts.col() << ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_START_DEPARTURE_TIME, string());
				stream << ts.col(3) << "cadence : " << ts.getActionForm().getSelectNumberInput(ServiceAddAction::PARAMETER_PERIOD, 0, 120, optional<size_t>(0), 1, string(), "non") << " " <<
					ts.getActionForm().getTextInput(ServiceAddAction::PARAMETER_END_DEPARTURE_TIME, string(), "(fin cadence)");
				stream << ts.col(2) << ts.getActionForm().getSubmitButton("Créer");

				stream << ts.close();

				stream << "<h1>Mise à jour collective des calendriers</h1>";

				stream << "<p class=\"info\">Cette fonction met également à jour les services continus</p>";
			}


			////////////////////////////////////////////////////////////////////
			// TAB CONTINUOUS SERVICES
			if (openTabContent(stream, TAB_CONTINUOUS_SERVICES))
			{
				AdminFunctionRequest<JourneyPatternAdmin> searchRequest(_request, *this);
				HTMLForm sortedForm(searchRequest.getHTMLForm());

				AdminActionFunctionRequest<ServiceAddAction, JourneyPatternAdmin> newRequest(_request, *this);
				newRequest.getAction()->setLine(const_pointer_cast<JourneyPattern>(_line));
				newRequest.getAction()->setIsContinuous(true);

				AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

				AdminActionFunctionRequest<RemoveObjectAction, JourneyPatternAdmin> removeRequest(_request, *this);

				ActionResultHTMLTable::HeaderVector vc;
				vc.push_back(make_pair(string(), "Num"));
				vc.push_back(make_pair(ServiceSchedules::FIELD.name, "Départ premier"));
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
				BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& service, cservices)
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

					stream << tc.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), "/admin/img/" + ServiceAdmin::ICON);

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

				AdminActionFunctionRequest<JourneyPatternUpdateAction,JourneyPatternAdmin> updateRequest(_request, *this);
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
					"Ligne",
					p.getForm().getSelectInput(
						JourneyPatternUpdateAction::PARAMETER_LINE_ID,
						PTModule::getCommercialLineLabels(
							_request.getUser()->getProfile()->getRightsForModuleClass<TransportNetworkRight>(),
							_request.getUser()->getProfile()->getGlobalPublicRight<TransportNetworkRight>() >= READ,
						READ
						),
						_line->getCommercialLine() ? _line->getCommercialLine()->getKey() : optional<RegistryKeyType>()
				)	);
				stream << p.cell(
					"Direction (texte)",
					p.getForm().getTextInput(JourneyPatternUpdateAction::PARAMETER_DIRECTION, _line->getDirection())
				);
				stream << p.cell(
					"Direction (girouette)",
					p.getForm().getTextInputAutoCompleteFromTableSync(
						JourneyPatternUpdateAction::PARAMETER_DIRECTION_ID,
						_line->getDirectionObj() ? lexical_cast<string>(_line->getDirectionObj()->getKey()) : string(),
						_line->getDirectionObj() ? lexical_cast<string>(_line->getDirectionObj()->get<DisplayedText>()) : string(),
						lexical_cast<string>(DestinationTableSync::TABLE.ID),
						string(),string(),
						false, true, true, true
				)	);
				stream << p.cell(
					"Sens",
					p.getForm().getRadioInputCollection(
						JourneyPatternUpdateAction::PARAMETER_WAYBACK,
						waybackMap,
						optional<bool>(_line->getWayBack())
				)	);
				stream << p.cell(
					"Itinéraire principal de ligne",
					p.getForm().getOuiNonRadioInput(JourneyPatternUpdateAction::PARAMETER_MAIN, _line->getMain())
				);
				stream << p.cell(
					"Longueur planification (TAD)",
					p.getForm().getTextInput(
						JourneyPatternUpdateAction::PARAMETER_PLANNED_LENGTH,
						lexical_cast<string>(_line->getPlannedLength())
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
					cols.push_back(lexical_cast<string>(i));
				}
				HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const Path::Edges::value_type& edge, _line->getEdges())
				{
					const LinePhysicalStop& lineStop(dynamic_cast<const LinePhysicalStop&>(*edge));
					
					if(lineStop.isArrival())
					{
						stream << t.row();
						stream << t.col(1, string(), true);
						stream << static_cast<const StopPoint*>(lineStop.getFromVertex())->getConnectionPlace()->getFullName();
						stream << t.col(1, string(), true) << "A";

						for(int i(0); i<=23; ++i)
						{
							stream << t.col();
							BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, lineStop.getParentPath()->getServiceCollections())
							{
								lineStop.getArrivalFromIndex(*itCollection, false, 0); // Update indices
								Edge::ArrivalServiceIndices::mapped_type& indices(lineStop.getArrivalIndex(*itCollection));
						
								if(indices[i].get(false) == itCollection->getServices().rend())
								{
									stream << "-<br /><span class=\"mini\">-</span><br />";
								}
								else
								{
									const Service* service(*indices[i].get(false));
									stream << services[service];
									stream << "<br /><span class=\"mini\">" << service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span><br />";
								}
							}
						}
					}

					if(lineStop.isDeparture())
					{
						stream << t.row();
						stream << t.col(1, string(), true);
						stream << static_cast<const StopPoint*>(lineStop.getFromVertex())->getConnectionPlace()->getFullName();
						stream << t.col(1, string(), true) << "D";

						for(int i(0); i<=23; ++i)
						{
							stream << t.col();
							BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, lineStop.getParentPath()->getServiceCollections())
							{
								lineStop.getDepartureFromIndex(*itCollection, false, 0); // Update indices
								Edge::DepartureServiceIndices::mapped_type& indices(lineStop.getDepartureIndex(*itCollection));
								
								if(indices[i].get(false) == itCollection->getServices().end())
								{
									stream << "-<br /><span class=\"mini\">-</span><br />";
								}
								else
								{
									const Service* service(*indices[i].get(false));
									stream << services[service];
									stream << "<br /><span class=\"mini\">" << service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span><br />";
								}
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

			if(_line->getPathGroup())
			{
				return user.getProfile()->isAuthorized<TransportNetworkRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(static_cast<const CommercialLine*>(_line->getPathGroup())->getKey()));
			}
			else
			{
				return false;
			}
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
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				ScheduledServiceTableSync::SearchResult services(
					ScheduledServiceTableSync::Search(Env::GetOfficialEnv(), _line->getKey())
				);
				BOOST_FOREACH(const boost::shared_ptr<const ScheduledService>& service, services)
				{
					boost::shared_ptr<ServiceAdmin> p(
						getNewPage<ServiceAdmin>()
					);
					p->setService(service);
					links.push_back(p);
				}
				ContinuousServiceTableSync::SearchResult cservices(
					ContinuousServiceTableSync::Search(Env::GetOfficialEnv(), _line->getKey())
				);
				BOOST_FOREACH(const boost::shared_ptr<const ContinuousService>& service, cservices)
				{
					boost::shared_ptr<ServiceAdmin> p(
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
			boost::shared_ptr<CommercialLineAdmin> p(
				getNewPage<CommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_line->getCommercialLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}
	}
}
