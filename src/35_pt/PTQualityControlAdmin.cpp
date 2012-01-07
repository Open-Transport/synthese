
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

#include "JourneyPatternAdmin.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTQualityControlAdmin.hpp"
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
#include "Importable.h"
#include "DataSource.h"

#include <geos/geom/LineString.h>

using namespace boost;
using namespace std;
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
		template<> const string FactorableTemplate<AdminInterfaceElement, PTQualityControlAdmin>::FACTORY_KEY("PTQualityControlAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::ICON("cog.png");
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::DEFAULT_TITLE("Contrôle qualité");
	}

	namespace pt
	{
		const string PTQualityControlAdmin::PARAM_RUN_CONTROL("rc");

		const string PTQualityControlAdmin::TAB_STOPS_WITHOUT_COORDINATE("swc");
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_MAIN_STOP("cwm");
		const string PTQualityControlAdmin::TAB_EDGES_AND_GEOMETRIES("eag");
		const string PTQualityControlAdmin::TAB_RANK_CONTINUITY("rank_continuity");
		const string PTQualityControlAdmin::TAB_DOUBLE_ROUTES("double_routes");



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
			const admin::AdminRequest& request
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
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), StopPointAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
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
						stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), PTPlacesAdmin::ICON);
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
					runRequest.getPage()->setRunControl(true);

					stream <<
						"<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />" <<
						HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), ICON) <<
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

					AdminActionFunctionRequest<CopyGeometriesAction,PTQualityControlAdmin> copyRequest(request);

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
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
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

					AdminActionFunctionRequest<JourneyPatternRankContinuityRestoreAction,PTQualityControlAdmin> fixRequest(request);
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
					BOOST_FOREACH(const shared_ptr<JourneyPattern>& journeyPattern, journeyPatterns)
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
						BOOST_FOREACH(const shared_ptr<LineStop>& lineStop, lineStops)
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
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
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
											route2.getCodesBySource(*link.first).size() == 1 && route2.getACodeBySource(*link.first).empty() ||
											route2.hasCodeBySource(*link.first, link.second)
										) &&
										route2 == route
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
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request);
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
			const admin::AdminRequest& request
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
			_tabBuilded = true;
		}
	}
}
