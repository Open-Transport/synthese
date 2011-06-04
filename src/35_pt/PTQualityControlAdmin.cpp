
//////////////////////////////////////////////////////////////////////////
/// PTQualityControlAdmin class implementation.
///	@file PTQualityControlAdmin.cpp
///	@author RCSobility
///	@date 2011
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

#include <geos/geom/LineString.h>

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
							if(!nextEdge)
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
										if(itg && *edge->getGeometry()->getCoordinates() == *itg->getCoordinates())
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
					c.push_back("Arrêt départ");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt arrivée");
					c.push_back("Arrêt arrivée");
					c.push_back("Arrêt arrivée");
					c.push_back("Statut");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();
					BOOST_FOREACH(const GeometriesMap::value_type& itResult, resultMap)
					{
						if(itResult.second.size() == 1 && *itResult.second.begin())
						{
							continue;
						}

						const StopPoint& first(static_cast<const StopPoint&>(*itResult.first.first));
						const StopPoint& second(static_cast<const StopPoint&>(*itResult.first.second));

						stream << t.row();
						stream << t.col() << first.getConnectionPlace()->getCity()->getName();
						stream << t.col() << first.getConnectionPlace()->getName();
						stream << t.col() << first.getName();
						stream << t.col() << second.getConnectionPlace()->getCity()->getName();
						stream << t.col() << second.getConnectionPlace()->getName();
						stream << t.col() << second.getName();
						stream << t.col() << ((itResult.second.size() == 1 && !*itResult.second.begin()) ? "Pas de géométrie" : "Plusieurs géométries");
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

			_tabBuilded = true;
		}
	}
}
