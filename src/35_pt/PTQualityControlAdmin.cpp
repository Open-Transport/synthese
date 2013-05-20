
//////////////////////////////////////////////////////////////////////////
/// PTQualityControlAdmin class implementation.
///	@file PTQualityControlAdmin.cpp
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

#include "PTQualityControlAdmin.hpp"

#include "DesignatedLinePhysicalStop.hpp"
#include "DRTArea.hpp"
#include "JourneyPatternAdmin.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "Profile.h"
#include "ScheduledService.h"
#include "ServiceAdmin.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "StopPoint.hpp"
#include "ResultHTMLTable.h"
#include "StopArea.hpp"
#include "City.h"
#include "HTMLModule.h"
#include "AdminFunctionRequest.hpp"
#include "StopPointAdmin.hpp"
#include "PTPlacesAdmin.h"
#include "CopyGeometriesAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternRankContinuityRestoreAction.hpp"
#include "ImportableTemplate.hpp"
#include "DataSource.h"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace geography;
	using namespace graph;
	using namespace impex;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTQualityControlAdmin>::FACTORY_KEY = "PTQualityControl";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::ICON = "cog.png";
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::DEFAULT_TITLE = "Contrôle qualité";
	}

	namespace pt
	{
		const string PTQualityControlAdmin::PARAM_RUN_CONTROL = "rc";

		const string PTQualityControlAdmin::TAB_STOPS_WITHOUT_COORDINATE = "swc";
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_MAIN_STOP = "cwm";
		const string PTQualityControlAdmin::TAB_EDGES_AND_GEOMETRIES = "eag";
		const string PTQualityControlAdmin::TAB_RANK_CONTINUITY = "rank_continuity";
		const string PTQualityControlAdmin::TAB_DOUBLE_ROUTES = "double_routes";
		const string PTQualityControlAdmin::TAB_SPEED = "speed";
		const string PTQualityControlAdmin::TAB_ASCENDING_SCHEDULES = "ascending_schedules";



		PTQualityControlAdmin::PTQualityControlAdmin()
			: AdminInterfaceElementTemplate<PTQualityControlAdmin>(),
			_runControl(false)
		{ }



		void PTQualityControlAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_runControl = map.getDefault<bool>(PARAM_RUN_CONTROL);
		}



		ParametersMap PTQualityControlAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_RUN_CONTROL, _runControl);
			return m;
		}



		bool PTQualityControlAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTQualityControlAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// STOPS WITHOUT COORDINATE TAB
			if (openTabContent(stream, TAB_STOPS_WITHOUT_COORDINATE))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<StopPointAdmin> openRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						if(it.second->getGeometry().get() && !it.second->getGeometry()->isEmpty())
						{
							continue;
						}
						openRequest.getPage()->setStop(it.second);
						stream << t.row();
						stream << t.col() << it.second->getConnectionPlace()->getCity()->getName();
						stream << t.col() << it.second->getConnectionPlace()->getName();
						stream << t.col() << it.second->getName();
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + StopPointAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) <<
						"</p>"
					;
				}
			}

			////////////////////////////////////////////////////////////////////
			// CITIES_WITHOUT_MAIN_STOP TAB
			if (openTabContent(stream, TAB_CITIES_WITHOUT_MAIN_STOP))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<PTPlacesAdmin> openRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					BOOST_FOREACH(const City::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<City>())
					{
						if(it.second->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0 || !it.second->getIncludedPlaces().empty())
						{
							continue;
						}
						openRequest.getPage()->setCity(it.second);
						stream << t.row();
						stream << t.col() << it.second->getName();
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + PTPlacesAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) <<
						"</p>"
					;
				}
			}

			////////////////////////////////////////////////////////////////////
			// CITIES_WITHOUT_MAIN_STOP TAB
			if (openTabContent(stream, TAB_EDGES_AND_GEOMETRIES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					// Gathering different geometries of similar edges, for each pair of stops
					typedef std::map<std::pair<Vertex*, Vertex*>, set<LineString*> > GeometriesMap;
					GeometriesMap resultMap;

					// Scan of each stop
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						// Scan of each edge starting at the stop
						BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, it.second->getDepartureEdges())
						{
							const Edge* edge(itEdge.second);
							const Edge* nextEdge(edge->getNext());

							// If the edge is a path termination, ignore it
							if(!nextEdge || dynamic_cast<JourneyPatternCopy*>(edge->getParentPath()))
							{
								continue;
							}

							Vertex* vertex(edge->getFromVertex());
							Vertex* nextVertex(nextEdge->getFromVertex());

							// Comparison of the edge geometry and other geometries of similar edges
							GeometriesMap::iterator itGeom(resultMap.find(make_pair(vertex, nextVertex)));

							// If no similar edges, immediate storage of the geometry
							if(itGeom == resultMap.end())
							{
								set<LineString*> geoms;
								geoms.insert(edge->getGeometry().get());
								resultMap.insert(
									make_pair(
										make_pair(vertex, nextVertex),
										geoms
								)	);
							}
							else
							{
								bool toInsert(true);
								if(edge->getGeometry().get())
								{
									// If a similar edge has the same geometry then ignore the current edge
									BOOST_FOREACH(const GeometriesMap::mapped_type::value_type& itg, itGeom->second)
									{
										if(itg && *edge->getGeometry()->getCoordinatesRO() == *itg->getCoordinatesRO())
										{
											toInsert = false;
											break;
										}
									}
								}
								else
								{
									// If an edge already has no geometry, ignore the current edge
									BOOST_FOREACH(const GeometriesMap::mapped_type::value_type& itg, itGeom->second)
									{
										if(!itg)
										{
											toInsert = false;
											break;
										}
									}
								}
								if(toInsert)
								{
									itGeom->second.insert(edge->getGeometry().get());
								}
							}
					}	}

					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("#");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt arrivée");
					c.push_back("Arrêt arrivée");
					c.push_back("Arrêt arrivée");
					c.push_back("Statut");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					AdminActionFunctionRequest<CopyGeometriesAction,PTQualityControlAdmin> copyRequest(request, *this);

					stream << t.open();
					size_t r(0);
					BOOST_FOREACH(const GeometriesMap::value_type& itResult, resultMap)
					{
						if(itResult.second.size() == 1 && *itResult.second.begin())
						{
							continue;
						}

						const StopPoint& first(static_cast<const StopPoint&>(*itResult.first.first));
						const StopPoint& second(static_cast<const StopPoint&>(*itResult.first.second));

						bool noGeometry(itResult.second.size() == 1 && !*itResult.second.begin());
						bool severalGeometries(!noGeometry);

						stream << t.row();
						stream << t.col() << r++;
						stream << t.col() << first.getConnectionPlace()->getCity()->getName();
						stream << t.col() << first.getConnectionPlace()->getName();
						stream << t.col() << first.getName();
						stream << t.col() << second.getConnectionPlace()->getCity()->getName();
						stream << t.col() << second.getConnectionPlace()->getName();
						stream << t.col() << second.getName();
						stream << t.col() << (noGeometry ? "Pas de géométrie" : "Plusieurs géométries");

						// Copy geometry button, only for edges with several geometries
						stream << t.col();
						if(severalGeometries)
						{
							copyRequest.getAction()->setStartingStop(
								Env::GetOfficialEnv().getSPtr(&first)
							);
							copyRequest.getAction()->setEndingStop(
								Env::GetOfficialEnv().getSPtr(&second)
							);
							stream << HTMLModule::getLinkButton(copyRequest.getURL(), "Uniformiser", "Etes-vous sûr de vouloir uniformiser les géométries ?\\n La géométrie la plus détaillée sera conservée et copiée sur chaque ligne.");
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// RANK CONTINUITY
			if (openTabContent(stream, TAB_RANK_CONTINUITY))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("Journey Pattern");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					AdminActionFunctionRequest<JourneyPatternRankContinuityRestoreAction,PTQualityControlAdmin> fixRequest(request, *this);
					AdminFunctionRequest<JourneyPatternAdmin> openRequest(request);

					stream << t.open();

					Env env;
					JourneyPatternTableSync::SearchResult journeyPatterns(
						JourneyPatternTableSync::Search(
							env,
							optional<RegistryKeyType>(),
							0,
							optional<size_t>(),
							false,
							true,
							FIELDS_ONLY_LOAD_LEVEL
					)	);
					BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& journeyPattern, journeyPatterns)
					{
						LineStopTableSync::SearchResult lineStops(
							LineStopTableSync::Search(
								env,
								journeyPattern->getKey(),
								optional<RegistryKeyType>(),
								0,
								optional<size_t>(),
								true,
								true,
								FIELDS_ONLY_LOAD_LEVEL
						)	);
						size_t rank(0);
						bool ok(true);
						BOOST_FOREACH(const boost::shared_ptr<LineStop>& lineStop, lineStops)
						{
							if(lineStop->getRankInPath() != rank)
							{
								ok = false;
								break;
							}
							++rank;
						}
						if(!ok)
						{
							fixRequest.getAction()->setJourneyPattern(const_pointer_cast<const JourneyPattern>(journeyPattern));
							openRequest.getPage()->setLine(const_pointer_cast<const JourneyPattern>(journeyPattern));
							stream << t.row();
							stream << t.col() << HTMLModule::getHTMLLink(openRequest.getURL(), lexical_cast<string>(journeyPattern->getKey()));
							stream << t.col() << HTMLModule::getLinkButton(fixRequest.getURL(), "Réparer");
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
			}	}


			////////////////////////////////////////////////////////////////////
			// DOUBLE ROUTES
			if (openTabContent(stream, TAB_DOUBLE_ROUTES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<JourneyPatternAdmin> openJourneyPatternRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Network");
					c.push_back("Line");
					c.push_back("Route 1");
					c.push_back("Route 2");
					c.push_back("Source");
					c.push_back("Code");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					BOOST_FOREACH(const CommercialLine::Registry::value_type& itline, Env::GetOfficialEnv().getRegistry<CommercialLine>())
					{
						BOOST_FOREACH(Path* itroute, itline.second->getPaths())
						{
							const JourneyPattern& route(static_cast<const JourneyPattern&>(*itroute));
							BOOST_FOREACH(const Importable::DataSourceLinks::value_type& link, route.getDataSourceLinks())
							{
								BOOST_FOREACH(Path* itroute2, itline.second->getPaths())
								{
									const JourneyPattern& route2(static_cast<const JourneyPattern&>(*itroute2));
									if(	&route2 != &route &&
										route2.hasLinkWithSource(*link.first) &&
										(	link.second.empty() ||
											(route2.getCodesBySource(*link.first).size() == 1 && route2.getACodeBySource(*link.first).empty()) ||
											route2.hasCodeBySource(*link.first, link.second)
										) &&
										(route2 == route)
									){
										stream << t.row();

										stream << t.col() << itline.second->getNetwork()->getName();

										stream << t.col() << itline.second->getShortName();

										// Route 1
										stream << t.col();
										openJourneyPatternRequest.getPage()->setLine(
											Env::GetOfficialEnv().getSPtr(&route)
										);
										stream << HTMLModule::getHTMLLink(openJourneyPatternRequest.getURL(), lexical_cast<string>(route.getKey()));

										// Route 2
										stream << t.col();
										openJourneyPatternRequest.getPage()->setLine(
											Env::GetOfficialEnv().getSPtr(&route2)
										);
										stream << HTMLModule::getHTMLLink(openJourneyPatternRequest.getURL(), lexical_cast<string>(route2.getKey()));

										// Code
										stream << t.col() << link.first->getName();

										stream << t.col() << link.second;
									}
								}
							}
						}
					}

					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
			}	}


			////////////////////////////////////////////////////////////////////
			// SPEED
			if (openTabContent(stream, TAB_SPEED))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);
					double maxSpeed(0);

					HTMLTable::ColsVector c;
					c.push_back("Network");
					c.push_back("Line");
					c.push_back("Service");
					c.push_back("Stop 1");
					c.push_back("Stop 2");
					c.push_back("Vitesse");
					c.push_back("Distance");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					BOOST_FOREACH(const ScheduledService::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<ScheduledService>())
					{
						time_duration lastDepartureTime(not_a_date_time);
						const DesignatedLinePhysicalStop* lastDepartureLineStop(NULL);
						const ScheduledService& service(*item.second);

						BOOST_FOREACH(Edge* edge, service.getPath()->getEdges())
						{
							if(!dynamic_cast<DesignatedLinePhysicalStop*>(edge))
							{
								break;
							}
							const DesignatedLinePhysicalStop& lineStop(
								dynamic_cast<DesignatedLinePhysicalStop&>(*edge)
							);

							if(	lineStop.isArrivalAllowed() &&
								lineStop.getScheduleInput() &&
								lastDepartureLineStop &&
								lastDepartureLineStop->getFromVertex()->getGeometry().get() &&
								lineStop.getFromVertex()->getGeometry().get() &&
								service.getArrivalSchedule(false, lineStop.getRankInPath()) != lastDepartureTime &&
								lineStop.getFromVertex()->getGeometry()->distance(
									lastDepartureLineStop->getFromVertex()->getGeometry().get()
								) > 5000
							){
								double dist(
									lineStop.getFromVertex()->getGeometry()->distance(
										lastDepartureLineStop->getFromVertex()->getGeometry().get()
								)	);
								double speed(
									3.6 * dist / (
										service.getArrivalSchedule(false, lineStop.getRankInPath()).total_seconds() -
										lastDepartureTime.total_seconds()
								)	);
								if(speed > 150)
								{
									stream << t.row();
									stream << t.col() << service.getRoute()->getCommercialLine()->getNetwork()->getName();
									stream << t.col() << service.getRoute()->getCommercialLine()->getShortName();

									// Service cell
									openServiceRequest.getPage()->setService(Env::GetOfficialEnv().getSPtr(&service));
									stream << t.col() << HTMLModule::getLinkButton(openServiceRequest.getURL(), service.getServiceNumber());

									stream << t.col() << dynamic_cast<const StopArea*>(lineStop.getHub())->getFullName();
									stream << t.col() << dynamic_cast<const StopArea*>(lastDepartureLineStop->getHub())->getFullName();
									stream << t.col() << speed;
									stream << t.col() << dist;
								}
								if(speed > maxSpeed)
								{
									maxSpeed = speed;
								}
							}
							if(	lineStop.isDepartureAllowed() &&
								lineStop.getScheduleInput()
							){
								lastDepartureTime = service.getDepartureSchedule(false, lineStop.getRankInPath());
								lastDepartureLineStop = &lineStop;
							}
						}
					}

					stream << t.row();
					stream << t.col(5, string(), true) << "Max";
					stream << t.col(1, string(), true) << maxSpeed;
					stream << t.col(1, string(), true);

					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
			}	}


			////////////////////////////////////////////////////////////////////
			// ASCENDING SCHEDULES
			if (openTabContent(stream, TAB_ASCENDING_SCHEDULES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne");
					c.push_back("Service");
					c.push_back("Lieu départ");
					c.push_back("Heure départ");
					c.push_back("Lieu arrivée");
					c.push_back("Heure arrivée");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					// Loop on scheduled services
					BOOST_FOREACH(const ScheduledService::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<ScheduledService>())
					{
						// Get the current service
						const ScheduledService& service(*item.second);

						// Loop on departure edges
						BOOST_FOREACH(Edge* departureEdge, service.getPath()->getEdges())
						{
							// Avoid non departure edges
							if(!departureEdge->isDepartureAllowed())
							{
								continue;
							}

							// Get departure schedule
							time_duration departureSchedule(
								service.getDepartureSchedule(false, departureEdge->getRankInPath())
							);

							// Loop on following arrival edges
							for(Edge* arrivalEdge(departureEdge->getFollowingArrivalForFineSteppingOnly());
								arrivalEdge != NULL;
								arrivalEdge = arrivalEdge->getFollowingArrivalForFineSteppingOnly()
							){
								// Get arrival schedule
								time_duration arrivalSchedule(
									service.getArrivalSchedule(false, arrivalEdge->getRankInPath())
								);

								// Mistake detected
								if(arrivalSchedule < departureSchedule)
								{
									// Row
									stream << t.row();

									// Network
									stream << t.col() << service.getRoute()->getCommercialLine()->getNetwork()->getName();

									// Line
									stream << t.col() << service.getRoute()->getCommercialLine()->getShortName();

									// Service cell
									openServiceRequest.getPage()->setService(Env::GetOfficialEnv().getSPtr(&service));
									stream << t.col() << HTMLModule::getLinkButton(openServiceRequest.getURL(), service.getServiceNumber());

									// Departure place
									stream << t.col();
									if(dynamic_cast<const StopArea*>(departureEdge->getHub()))
									{
										stream << dynamic_cast<const StopArea*>(departureEdge->getHub())->getFullName();
									}
									else if(dynamic_cast<const DRTArea*>(departureEdge->getHub()))
									{
										stream << dynamic_cast<const DRTArea*>(departureEdge->getHub())->getName();
									}

									// Departure schedule
									stream << t.col() << departureSchedule;

									// Arrival place
									stream << t.col();
									if(dynamic_cast<const StopArea*>(arrivalEdge->getHub()))
									{
										stream << dynamic_cast<const StopArea*>(arrivalEdge->getHub())->getFullName();
									}
									else if(dynamic_cast<const DRTArea*>(arrivalEdge->getHub()))
									{
										stream << dynamic_cast<const DRTArea*>(arrivalEdge->getHub())->getName();
									}

									// Arrival schedule
									stream << t.col() << arrivalSchedule;
								}

								// Break the loop if the edge is a later departure too
								if(	arrivalEdge->isDepartureAllowed() &&
									service.getDepartureSchedule(false, arrivalEdge->getRankInPath()) > departureSchedule
								){
									break;
								}
							}
						}
					}

					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
						"</p>"
					;
			}	}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTQualityControlAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		std::string PTQualityControlAdmin::getTitle() const
		{
			return DEFAULT_TITLE + (_runControl ? (" (" + _activeTab +")") : string());
		}



		void PTQualityControlAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Arrêts non localisés", TAB_STOPS_WITHOUT_COORDINATE, false));
			_tabs.push_back(Tab("Localités sans arrêt principal", TAB_CITIES_WITHOUT_MAIN_STOP, false));
			_tabs.push_back(Tab("Itinéraires de lignes", TAB_EDGES_AND_GEOMETRIES, false));
			_tabs.push_back(Tab("Continuité des rangs", TAB_RANK_CONTINUITY, false));
			_tabs.push_back(Tab("Parcours en double", TAB_DOUBLE_ROUTES, false));
			_tabs.push_back(Tab("Vitesse", TAB_SPEED, false));
			_tabs.push_back(Tab("Chronologie des horaires", TAB_ASCENDING_SCHEDULES, false));
			_tabBuilded = true;
		}
	}
}
