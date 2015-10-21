
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
#include "CommercialLineAdmin.h"
#include "RollingStock.hpp"
#include "RollingStockTableSync.hpp"
#include "LineStopTableSync.h"
#include "Profile.h"
#include "ScheduledService.h"
#include "ServiceAdmin.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "AnalysisModule.hpp"
#include "TransportNetworkRight.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "StopPointAdmin.hpp"
#include "ResultHTMLTable.h"
#include "StopArea.hpp"
#include "StopAreaMergeAction.hpp"
#include "RoadPlace.h"
#include "Junction.hpp"
#include "City.h"
#include "HTMLModule.h"
#include "AdminFunctionRequest.hpp"
#include "PTPlacesAdmin.h"
#include "PTPlaceAdmin.h"
#include "PTUseRule.h"
#include "CopyGeometriesAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "JourneyPatternRankContinuityRestoreAction.hpp"
#include "JourneyPatternAdmin.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ImportableTemplate.hpp"
#include "DataSource.h"
#include "RemoveObjectAction.hpp"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace analysis;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace road;
	using namespace db;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTQualityControlAdmin>::FACTORY_KEY = "PTQualityControl";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::ICON = "cog.png";
		template<> const string AdminInterfaceElementTemplate<PTQualityControlAdmin>::DEFAULT_TITLE = "Contrôle qualité";
	}

	namespace analysis
	{
		const string PTQualityControlAdmin::PARAM_RUN_CONTROL = "rc";
		const string PTQualityControlAdmin::PARAM_LINES = "lines";
		const string PTQualityControlAdmin::PARAM_DISTANCE = "distance";
		const string PTQualityControlAdmin::PARAM_SPEED = "speed";

		const string PTQualityControlAdmin::TAB_STOPS_TOO_CLOSE = "stops_too_close";
		const string PTQualityControlAdmin::TAB_STOPS_TOO_FAR = "stops_too_far";
		const string PTQualityControlAdmin::TAB_STOPS_WITHOUT_COORDINATE = "stops_without_coordinates";
		const string PTQualityControlAdmin::TAB_STOPS_NOT_PROJECTED = "stops_not_projected";
		const string PTQualityControlAdmin::TAB_STOPAREAS_WITHOUT_CITY = "stopareas_without_city";
		const string PTQualityControlAdmin::TAB_STOPAREAS_HOMONYM = "stopareas_homonym";
		const string PTQualityControlAdmin::TAB_STOPAREAS_NAME_STOPS_NAME = "stopareas_stops_different_names";
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_STREET = "cities_without_street";
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_STOP = "cities_without_stop";
		const string PTQualityControlAdmin::TAB_CITIES_WITHOUT_MAIN_STOP = "cities_without_main_stop";
		const string PTQualityControlAdmin::TAB_LINES_WITHOUT_ITINERARY = "lines_without_itinerary";
		const string PTQualityControlAdmin::TAB_EDGES_WITHOUT_GEOMETRIES = "edges_without_geometries";
		const string PTQualityControlAdmin::TAB_BUS_TOO_FAST = "bus_too_fast";
		const string PTQualityControlAdmin::TAB_BUS_TOO_SLOW = "bus_too_slow";
		const string PTQualityControlAdmin::TAB_BUS_LINES_WITHOUT_SERVICES = "bus_lines_without_services";
		const string PTQualityControlAdmin::TAB_METRO_LINES_WITHOUT_SERVICES = "metro_lines_without_services";
		const string PTQualityControlAdmin::TAB_EDGES_AND_GEOMETRIES = "edges_and_geometries";
		const string PTQualityControlAdmin::TAB_RANK_CONTINUITY = "rank_continuity";
		const string PTQualityControlAdmin::TAB_DOUBLE_ROUTES = "double_routes";
		const string PTQualityControlAdmin::TAB_SPEED = "speed";
		const string PTQualityControlAdmin::TAB_ASCENDING_SCHEDULES = "ascending_schedules";



		PTQualityControlAdmin::PTQualityControlAdmin()
			: AdminInterfaceElementTemplate<PTQualityControlAdmin>(),
			_runControl(false),
			_ignoredLines(),
			_networks(),
			_transportModes(),
			_stopsDistance(),
			_transportSpeed()
		{ }



		void PTQualityControlAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_runControl = map.getDefault<bool>(PARAM_RUN_CONTROL);
			_ignoredLines = map.getDefault<string>(PARAM_LINES);
			_stopsDistance = map.getDefault<int>(PARAM_DISTANCE);
			_transportSpeed = map.getDefault<int>(PARAM_SPEED);

			TransportNetworkTableSync::SearchResult networks(TransportNetworkTableSync::Search(Env::GetOfficialEnv()));
			if (!networks.empty())
			{
				_networks="";
				for (unsigned int id = 1; id <= networks.size(); id++)
					if (map.getDefault<string>("network_"+lexical_cast<string>(id)).size()>1)
						_networks+=map.getDefault<string>("network_"+lexical_cast<string>(id))+",";

			}

			RollingStockTableSync::SearchResult modes(RollingStockTableSync::SearchUsedModes(Env::GetOfficialEnv()));
			if (!_transportModes.empty())
			{
				_transportModes="";
				for (unsigned int id = 1; id <= modes.size()-1; id++)
					if (map.getDefault<string>("mode_"+lexical_cast<string>(id)).size()>1)
						_transportModes+=map.getDefault<string>("mode_"+lexical_cast<string>(id))+",";
			}
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
			// STOPS TOO CLOSE TAB
			// This control displays StopPoint objects which are too close from each others
			// It uses an input to specify the minimal distance between StopPoint objects
			// Feeding the input with a value is optional. (Default distance is 0.5 meters)
			if (openTabContent(stream, TAB_STOPS_TOO_CLOSE))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					/* Get value from input field placed in URL if already specified */
					int distance=0;
					if (_stopsDistance)
						distance = _stopsDistance;

					std::vector<util::RegistryKeyType> Ids;
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt trop proche");
					c.push_back("Arrêt trop proche");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopPoints from database and continue if each has a geometry associated to it */
					StopPointTableSync::SearchResult stops(StopPointTableSync::Search(Env::GetOfficialEnv(),
						boost::optional<util::RegistryKeyType>(),NULL,false));

					BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& it, stops)
					{
						/* If so, search in database for physical stops which are too close from the current one (less than 0.5 meter) */
						if(it->getGeometry() && !it->getGeometry()->isEmpty())
						{
							StopPointTableSync::SearchResult near(StopPointTableSync::SearchByMaxDistance(*it->getGeometry(),
								(distance == 0 ? 0.5 : distance),Env::GetOfficialEnv(),UP_LINKS_LOAD_LEVEL));

							BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& nr, near)
							{
								/* Don't display the result if the physical stop is the same in both loops or has already been checked */
								if ((it == nr) || ((!Ids.empty()) && (std::find(Ids.begin(),Ids.end(),nr->getKey())!=Ids.end())))
								{
									continue;
								}
								else
								{
									/* Remember that this physical stop has been checked in order to avoid twins */
									Ids.push_back(it->getKey());

									AdminFunctionRequest<StopPointAdmin> openStop1Request(request);
									AdminFunctionRequest<StopPointAdmin> openStop2Request(request);
									stream << t.row();

									openStop1Request.getPage()->setStop(nr);
									
									/* Check data isn't corrupted in order to avoid potential crash */
									if(nr->getConnectionPlace()->getCity()!=NULL)
										stream << t.col() << nr->getConnectionPlace()->getCity()->getName();
									else
										stream << t.col() << "<b><font color=red>Pas de commune</font></b>";

									stream << t.col() << HTMLModule::getLinkButton(openStop1Request.getURL(),
										nr->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
									stream << t.col() << nr->getKey();

									openStop2Request.getPage()->setStop(it);
									stream << t.col() << HTMLModule::getLinkButton(openStop2Request.getURL(),
										it->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
									stream << t.col() << it->getKey();
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
					HTMLForm f("distance",runRequest.getURL());
									
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle permet de rechercher les arrêts physiques trop proches les uns des autres selon "
							<< "une distance (en m) rentrée dans le champ de saisie. Si aucune valeur n'est rentrée, la valeur par défaut "
							<< "est de 0,5 mètres.<br /><br />"
							<< f.getTextInput("distance", string())
							<< f.getSubmitButton("OK")
							<< "</p>"
							<< f.close()
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPS TOO FAR TAB
			if (openTabContent(stream, TAB_STOPS_TOO_FAR))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					/* Get value from input field placed in URL if already specified */
					int distance=0;
					if (_stopsDistance)
						distance = _stopsDistance;

					std::vector<util::RegistryKeyType> Ids;
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt physique n°1");
					c.push_back("Arrêt physique n°1");
					c.push_back("Arrêt physique n°2");
					c.push_back("Arrêt physique n°2");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
						
					/* Get all StopPoints from database and loop on each */
					StopPointTableSync::SearchResult stops(StopPointTableSync::Search(Env::GetOfficialEnv(),
						boost::optional<util::RegistryKeyType>(),NULL,false));

					BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& stop, stops)
					{
						if(!stop->getGeometry())
							continue;

						/* Check if the current StopPoint is distant of more than 300 meters from the other StopPoints in its StopArea */
						StopPointTableSync::SearchResult far(StopPointTableSync::SearchDistance(*stop,
							Env::GetOfficialEnv(),true,(distance == 0 ? 300 : distance),UP_LINKS_LOAD_LEVEL));

						BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& fr, far)
						{
							/* Don't display the result if the physical stop has already been seen before */
							if ((!Ids.empty()) && (std::find(Ids.begin(),Ids.end(),stop->getKey())!=Ids.end()))
							{
								continue;
							}
							else
							{
								/* Remember that this physical stop has been checked in order to avoid twins */
								Ids.push_back(fr->getKey());

								AdminFunctionRequest<StopPointAdmin> openStop1Request(request);
								AdminFunctionRequest<StopPointAdmin> openStop2Request(request);
								AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
								
								boost::shared_ptr<const StopArea> sa = Env::GetOfficialEnv().get<StopArea>(stop->getConnectionPlace()->getKey());
								stream << t.row();

								openPlaceRequest.getPage()->setConnectionPlace(sa);
								
								/* Check data isn't corrupted in order to avoid potential crash */
								if(sa->getCity()!=NULL)
									stream << t.col() << sa->getCity()->getName();
								else
									stream << t.col() << "<b><font color=red>Pas de commune</font></b>";

								stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(),
									sa->getName(), string(), "/admin/img/" + PTPlaceAdmin::ICON);
								stream << t.col() << sa->getKey();

								openStop1Request.getPage()->setStop(stop);
								stream << t.col() << HTMLModule::getLinkButton(openStop1Request.getURL(),
									stop->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
								stream << t.col() << stop->getKey();

								openStop2Request.getPage()->setStop(fr);
								stream << t.col() << HTMLModule::getLinkButton(openStop2Request.getURL(),
									fr->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
								stream << t.col() << fr->getKey();
							}
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);
					HTMLForm f("distance",runRequest.getURL());
									
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle permet de rechercher les arrêts physiques trop éloignés les uns des autres selon "
							<< "une distance (en m) rentrée dans le champ de saisie. Si aucune valeur n'est rentrée, la valeur par défaut "
							<< "est de 300 mètres. Cette valeur correspond à un éloignement entre arrêts physiques d'un même arrêt logique.<br /><br />"
							<< f.getTextInput("distance", string())
							<< f.getSubmitButton("OK")
							<< "</p>"
							<< f.close()
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPS WITHOUT COORDINATE TAB
			if (openTabContent(stream, TAB_STOPS_WITHOUT_COORDINATE))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt physique");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopPoints from environment and check if each have a geometry associated to it */
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						if(it.second->getGeometry().get() && !it.second->getGeometry()->isEmpty())
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<StopPointAdmin> openRequest(request);
							AdminActionFunctionRequest<RemoveObjectAction,PTQualityControlAdmin> removeRequest(request);

							stream << t.row();

							openRequest.getPage()->setStop(it.second);
							
							/* Check data isn't corrupted in order to avoid potential crash */
							if(it.second->getConnectionPlace()!=NULL)
							{								
								if(it.second->getConnectionPlace()->getCity()!=NULL)
									stream << t.col() << it.second->getConnectionPlace()->getCity()->getName();
								else
									stream << t.col() << "<b><font color=red>Pas de commune</font></b>";

								stream << t.col() << it.second->getConnectionPlace()->getName();
							}
							else
							{
								stream << t.col() << "<b><font color=red>Pas de commune</font></b>";
								stream << t.col() << "<b><font color=red>Pas de zone d'arrêt</font></b>";
							}
							
							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
								it.second->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);

							stream << t.col() << it.second->getKey();

							/* If stop unused, purpose deletion */
							bool usedStop(false);
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
							{
								if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
								{
									usedStop = true;
									break;
								}
							}
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getArrivalEdges())
							{
								if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
								{
									usedStop = true;
									break;
								}
							}
							if(!usedStop)
							{
								removeRequest.getAction()->setObjectId(it.second->getKey());
								stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), 
									"Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
							}
							else
							{
								stream << t.col();
							}
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les arrêts physiques qui ne possèdent pas de géométries et propose "
							<< "leur suppression dans le cas où ils ne sont pas utilisés pour un itinéraire.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPS NOT PROJECTED TAB
			if (openTabContent(stream, TAB_STOPS_NOT_PROJECTED))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt physique");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopPoints from environment and check if each has a roadchunk associated to it */
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						if(it.second->getProjectedPoint().getRoadChunk() != NULL)
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<StopPointAdmin> openRequest(request);
							AdminActionFunctionRequest<RemoveObjectAction,PTQualityControlAdmin> removeRequest(request);
							stream << t.row();

							openRequest.getPage()->setStop(it.second);

							/* Check data isn't corrupted in order to avoid potential crash */
							if(it.second->getConnectionPlace()!=NULL)
							{								
								if(it.second->getConnectionPlace()->getCity()!=NULL)
									stream << t.col() << it.second->getConnectionPlace()->getCity()->getName();
								else
									stream << t.col() << "<b><font color=red>Pas de commune</font></b>";

								stream << t.col() << it.second->getConnectionPlace()->getName();
							}
							else
							{
								stream << t.col() << "<b><font color=red>Pas de commune</font></b>";
								stream << t.col() << "<b><font color=red>Pas de zone d'arrêt</font></b>";
							}
							
							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
								it.second->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);

							stream << t.col() << it.second->getKey();

							/* If stop unused, purpose deletion */
							bool usedStop(false);
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
							{
								if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
								{
									usedStop = true;
									break;
								}
							}
							BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getArrivalEdges())
							{
								if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
								{
									usedStop = true;
									break;
								}
							}
							if(!usedStop)
							{
								removeRequest.getAction()->setObjectId(it.second->getKey());
								stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), 
									"Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
							}
							else
							{
								stream << t.col();
							}
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les arrêts physiques qui ne sont pas projetés et propose leur "
							<< "suppression dans le cas où ils ne sont pas utilisés pour un itinéraire.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPAREAS_WITHOUT_CITY TAB
			if (openTabContent(stream, TAB_STOPAREAS_WITHOUT_CITY))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Arrêt logique");
					c.push_back("Arrêt logique");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopAreas from database and check if each has a city associated to it */
					StopAreaTableSync::SearchResult stopareas(StopAreaTableSync::Search(Env::GetOfficialEnv(),
						boost::optional<util::RegistryKeyType>(0),logic::indeterminate,NULL,NULL,NULL,false,true,0,0,FIELDS_ONLY_LOAD_LEVEL));

					BOOST_FOREACH(const StopAreaTableSync::SearchResult::value_type& stoparea, stopareas)
					{
						if(stoparea->getCity()!=NULL && stoparea->getCity()->getKey()!=0)
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
							stream << t.row();

							openPlaceRequest.getPage()->setConnectionPlace(stoparea);
							stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(), 
								stoparea->getName(), string(), "/admin/img/" + PTPlaceAdmin::ICON);
							stream << t.col() << stoparea->getKey();
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les arrêts logiques qui ne sont pas associés à une commune.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPAREAS HOMONYM TAB
			if (openTabContent(stream, TAB_STOPAREAS_HOMONYM))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					std::vector<util::RegistryKeyType> Ids;
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Zone d'arrêt");
					c.push_back("Zone d'arrêt");
					c.push_back("Zone d'arrêt homonyme");
					c.push_back("Zone d'arrêt homonyme");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopAreas from database and search identical names for each */
					StopAreaTableSync::SearchResult stopareas(StopAreaTableSync::Search(Env::GetOfficialEnv(),
						boost::optional<util::RegistryKeyType>(),logic::indeterminate,NULL,NULL,NULL,false,true,0,0,UP_LINKS_LOAD_LEVEL));

					BOOST_FOREACH(const StopAreaTableSync::SearchResult::value_type& stoparea, stopareas)
					{
						StopAreaTableSync::SearchResult homonyms(StopAreaTableSync::SearchSameName(*stoparea,
							Env::GetOfficialEnv(),UP_LINKS_LOAD_LEVEL));

						BOOST_FOREACH(const StopAreaTableSync::SearchResult::value_type& homonym, homonyms)
						{
							/* Don't display the result if the stoparea has already been seen before */
							if ((!Ids.empty()) && (std::find(Ids.begin(),Ids.end(),stoparea->getKey())!=Ids.end()))
							{
								continue;
							}
							else
							{
								/* Remember that this stoparea has been checked in order to avoid twins */
								Ids.push_back(homonym->getKey());
								
								AdminFunctionRequest<PTPlaceAdmin> openPlace1Request(request);
								AdminFunctionRequest<PTPlaceAdmin> openPlace2Request(request);
								AdminActionFunctionRequest<StopAreaMergeAction,PTQualityControlAdmin> mergeRequest(request);
			
								stream << t.row();

								openPlace1Request.getPage()->setConnectionPlace(stoparea);
								stream << t.col() << (stoparea->getCity()!=NULL 
									? stoparea->getCity()->getName() : "<b><font color=red>Pas de commune</font></b>");

								stream << t.col() << HTMLModule::getLinkButton(openPlace1Request.getURL(),
									stoparea->getName(), string(), "/admin/img/" + PTPlaceAdmin::ICON);
								stream << t.col() << stoparea->getKey();

								openPlace2Request.getPage()->setConnectionPlace(homonym);
								stream << t.col() << HTMLModule::getLinkButton(openPlace2Request.getURL(),
									stoparea->getName(), string(), "/admin/img/" + PTPlaceAdmin::ICON);
								stream << t.col() << homonym->getKey();
							
								mergeRequest.getAction()->setPlaces(stoparea,homonym);
								stream << t.col() << HTMLModule::getLinkButton(mergeRequest.getURL(),
									"Fusionner", "Attention, la fusion entraînera la suppression d'un des 2 arrêts logiques !");
							}
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les arrêts logiques qui possèdent le même nom et propose de les fusionner.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// STOPAREAS NAME STOPS NAME TAB
			if (openTabContent(stream, TAB_STOPAREAS_NAME_STOPS_NAME))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Zone d'arrêt");
					c.push_back("Zone d'arrêt");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt physique");
					c.push_back("Arrêt physique");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all StopAreas from environment and get each of their StopPoint */
					BOOST_FOREACH(const StopArea::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopArea>())
					{
						const StopArea::PhysicalStops& stops(it.second->getPhysicalStops());
						BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stop, stops)
						{
							/* If the StopPoint associated to this StopArea hasn't the same name, display it */
							if ((stop.second->getName()==it.second->getName()) || (stop.second->getName()==it.second->getName13()))
							{
								continue;
							}
							else
							{
								AdminFunctionRequest<StopPointAdmin> openRequest(request);
								AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);
								AdminActionFunctionRequest<RemoveObjectAction,PTQualityControlAdmin> removeRequest(request);

								boost::shared_ptr<const StopPoint> st = Env::GetOfficialEnv().get<StopPoint>(stop.second->getKey());
								stream << t.row();
			
								openPlaceRequest.getPage()->setConnectionPlace(it.second);
								stream << t.col() << HTMLModule::getLinkButton(openPlaceRequest.getURL(),
									it.second->getName(), string(), "/admin/img/" + PTPlaceAdmin::ICON);
								stream << t.col() << it.second->getKey();

								openRequest.getPage()->setStop(st);
								stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
									st->getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
								stream << t.col() << st->getKey();

								/* If stop unused, purpose deletion */
								bool usedStop(false);
								BOOST_FOREACH(const Vertex::Edges::value_type& edge, st->getDepartureEdges())
								{
									if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
									{
										usedStop = true;
										break;
									}
								}
								BOOST_FOREACH(const Vertex::Edges::value_type& edge, st->getArrivalEdges())
								{
									if(dynamic_cast<const DesignatedLinePhysicalStop*>(edge.second))
									{
										usedStop = true;
										break;
									}
								}
								if(!usedStop)
								{
									removeRequest.getAction()->setObjectId(st->getKey());
									stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), 
										"Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt physique ?");
								}
								else
								{
									stream << t.col();
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

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les arrêts physiques qui ne possèdent pas le même nom que l'"
							<< "arrêt logique associé. Dans le cas où ils ne sont utilisés pour aucun itinéraire, il est possible "
							<< "de supprimer ces arrêts physiques.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// CITIES_WITHOUT_MAIN_STOP TAB
			if (openTabContent(stream, TAB_CITIES_WITHOUT_MAIN_STOP))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Commune");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all cities from environment and check if each has a main StopArea included in it */
					BOOST_FOREACH(const City::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<City>())
					{
						if(it.second->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0)
						{
							continue;
						} 
						else if(!it.second->getIncludedPlaces().empty())
						{
							bool has_main_stop = false;
							BOOST_FOREACH(const City::IncludedPlaces::value_type& ip, it.second->getIncludedPlaces())
							{
								if(dynamic_cast<const StopArea*>(ip))
								{
									has_main_stop = true;
								}
							}
							if(has_main_stop)
							{
								continue;
							}
						}
						else
						{
							AdminFunctionRequest<PTPlacesAdmin> openRequest(request);
							stream << t.row();

							openRequest.getPage()->setCity(it.second);
							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
								it.second->getName(), string(), "/admin/img/" + PTPlacesAdmin::ICON);
							stream << t.col() << it.second->getKey();
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les communes qui ne possèdent aucun arrêt principal.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// CITIES_WITHOUT_STREET TAB
			if (openTabContent(stream, TAB_CITIES_WITHOUT_STREET))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Commune");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					
					/* Get all cities and check each has a street included in it */
					BOOST_FOREACH(const City::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<City>())
					{
						if(it.second->getLexicalMatcher(RoadPlace::FACTORY_KEY).size() > 0)
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<PTPlacesAdmin> openRequest(request);
							stream << t.row();

							openRequest.getPage()->setCity(it.second);
							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), 
								it.second->getName(), string(), "/admin/img/" + PTPlacesAdmin::ICON);
							stream << t.col() << it.second->getKey();
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les communes qui ne possèdent aucune rue.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// CITIES_WITHOUT_STOP TAB
			if (openTabContent(stream, TAB_CITIES_WITHOUT_STOP))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Commune");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					/* Get all cities and check each has a stop included in it */
					BOOST_FOREACH(const City::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<City>())
					{
						if(it.second->getLexicalMatcher(StopArea::FACTORY_KEY).size() > 0)
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<PTPlacesAdmin> openRequest(request);
							stream << t.row();

							openRequest.getPage()->setCity(it.second);
							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
								it.second->getName(), string(), "/admin/img/" + PTPlacesAdmin::ICON);
							stream << t.col() << it.second->getKey();
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les communes qui ne possèdent aucun arrêt logique.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// LINES WITHOUT ITINERARY TAB
			if (openTabContent(stream, TAB_LINES_WITHOUT_ITINERARY))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{	
					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne commerciale");
					c.push_back("Ligne commerciale");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();

					bool commercial_line_used;

					/* Get all CommercialLines from environment and loop on each */
					BOOST_FOREACH(const CommercialLine::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<CommercialLine>())
					{
						/* Get all lines from environment and check if the CommercialLine's ID is associated with one of them */
						commercial_line_used = false;	
						BOOST_FOREACH(const JourneyPattern::Registry::value_type& line, Env::GetOfficialEnv().getRegistry<JourneyPattern>())
						{
							/* If CommercialLine is associated, break the loop and check next occurrence */
							if(it.second->getKey()==line.second->getCommercialLine()->getKey())
							{
								commercial_line_used = true;
								break;
							}
						}
						if(commercial_line_used)
						{
							continue;
						}
						else
						{
							AdminFunctionRequest<CommercialLineAdmin> openRequest(request);
							stream << t.row();

							openRequest.getPage()->setCommercialLine(it.second);
							stream << t.col() << (it.second->getNetwork()!=NULL 
								? it.second->getNetwork()->getName() : "<b><font color=red>Pas de réseau</font></b>");

							stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(),
								it.second->getName(), string(), "/admin/img/" + CommercialLineAdmin::ICON);
							stream << t.col() << it.second->getKey();
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream  << "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les lignes commerciales qui ne possèdent aucun itinéraire.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON)
							<< "</p>"
					;
				}
			}
	

			////////////////////////////////////////////////////////////////////
			// EDGES WITHOUT GEOMETRIES TAB
			if (openTabContent(stream, TAB_EDGES_WITHOUT_GEOMETRIES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					std::vector<string> lines;
					if (!_ignoredLines.empty())
						split(lines, _ignoredLines, is_any_of(","));
					
					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("Itinéraire");
					c.push_back("Itinéraire");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt départ");
					c.push_back("Arrêt arrivée");
					c.push_back("Arrêt arrivée");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					// Scan of each stop
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						// Scan of each edge starting at the stop
						BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, it.second->getDepartureEdges())
						{
							const DesignatedLinePhysicalStop* edge(dynamic_cast<DesignatedLinePhysicalStop*>(itEdge.second));
							if(edge)
							{
								const Edge* nextEdge(itEdge.second->getNext());
								
								// If the edge is a path termination/junction/noId, ignore it
								if(	(	!nextEdge ||
										edge->getLineStop()->get<LineStringGeometry>()
									) ||
									(	itEdge.second->getParentPath() &&
										!itEdge.second->getParentPath()->getKey()
									) || 
									dynamic_cast<Junction*>(edge->getParentPath())
								){
									continue;
								}
								else
								{
									boost::shared_ptr<const JourneyPattern> itinerary = 
										Env::GetOfficialEnv().get<JourneyPattern>(itEdge.second->getParentPath()->getKey());
									/* Check ignored lines from input field */
									bool ignored = false;
									if(!lines.empty())
									{
										std::string line_number = lexical_cast<string>(itinerary->getCommercialLine()->getShortName());
										BOOST_FOREACH(std::string const& it, lines)
										{
											if(line_number.compare(it)==0)
											{
												ignored = true;
												break;
											}
										}
										if(ignored)
											continue;
									}
									StopPoint const& departure(static_cast<StopPoint const&>(*(edge->getFromVertex())));
									StopPoint const& arrival(static_cast<StopPoint const&>(*(nextEdge->getFromVertex())));
									if (itinerary != NULL)
									{
										AdminFunctionRequest<JourneyPatternAdmin> openJourneyRequest(request);
										AdminFunctionRequest<StopPointAdmin> openStop1Request(request);
										AdminFunctionRequest<StopPointAdmin> openStop2Request(request);
										boost::shared_ptr<const StopPoint> st1 = Env::GetOfficialEnv().get<StopPoint>(departure.getKey());
										boost::shared_ptr<const StopPoint> st2 = Env::GetOfficialEnv().get<StopPoint>(arrival.getKey());
										stream << t.row();

										openJourneyRequest.getPage()->setLine(itinerary);
										stream << t.col() << HTMLModule::getLinkButton(openJourneyRequest.getURL(),
											itinerary->getName(), string(), "/admin/img/" + JourneyPatternAdmin::ICON);
										stream << t.col() << itinerary->getKey();

										openStop1Request.getPage()->setStop(st1);
										stream << t.col() << HTMLModule::getLinkButton(openStop1Request.getURL(),
											departure.getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
										stream << t.col() << departure.getKey();

										openStop2Request.getPage()->setStop(st2);
										stream << t.col() << HTMLModule::getLinkButton(openStop2Request.getURL(),
											arrival.getName(), string(), "/admin/img/" + StopPointAdmin::ICON);
										stream << t.col() << arrival.getKey();
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
					HTMLForm f("lines",runRequest.getURL());
									
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les tronçons de ligne qui ne possèdent aucune géométrie. Il est "
							<< "possible d'ignorer des tronçons appartenant à certaines lignes commerciales en rentrant leur "
							<< "numéro de ligne séparé par une virgule.<br /><br />"
							<< f.getTextInput("lines", string())
							<< f.getSubmitButton("OK")
							<< "</p>"
							<< f.close()
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// BUS TOO FAST TAB
			if (openTabContent(stream, TAB_BUS_TOO_FAST))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					/* Get value from input */
					double maxSpeed = 50;
					if (_transportSpeed)
						maxSpeed = _transportSpeed;

					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne");
					c.push_back("Service");
					c.push_back("Arrêt de départ");
					c.push_back("Arrêt d'arrivée");
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
								service.getArrivalSchedule(false, lineStop.getRankInPath()) != lastDepartureTime
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
								if(speed > maxSpeed)
								{
									AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);
									stream << t.row();

									openServiceRequest.getPage()->setService(Env::GetOfficialEnv().getSPtr(&service));
									stream << t.col() << service.getRoute()->getCommercialLine()->getNetwork()->getName();
									stream << t.col() << service.getRoute()->getCommercialLine()->getShortName();
									stream << t.col() << HTMLModule::getLinkButton(openServiceRequest.getURL(), service.getServiceNumber());
									stream << t.col() << dynamic_cast<const StopArea*>(lineStop.getHub())->getFullName();
									stream << t.col() << dynamic_cast<const StopArea*>(lastDepartureLineStop->getHub())->getFullName();
									stream << t.col() << speed;
									stream << t.col() << dist;
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

					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);
					HTMLForm f("speed",runRequest.getURL());
									
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les tronçons de lignes parcourus trop rapidement par les bus. "
							<< "Il est possible de paramétrer la vitesse maximale tolérée en rentrant sa valeur (en km/h) dans le "
							<< "champ de saisie. Par défaut, la valeur maximale tolérée est de 50km/h.<br /><br />"
							<< f.getTextInput("speed", string())
							<< f.getSubmitButton("OK")
							<< "</p>"
							<< f.close()
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// BUS TOO SLOW TAB
			if (openTabContent(stream, TAB_BUS_TOO_SLOW))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					/* Get value from input */
					double maxSpeed = 8;
					if (_transportSpeed)
						maxSpeed = _transportSpeed;

					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne");
					c.push_back("Service");
					c.push_back("Stop de départ");
					c.push_back("Stop d'arrivée");
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
								service.getArrivalSchedule(false, lineStop.getRankInPath()) != lastDepartureTime
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
								if(speed < maxSpeed)
								{
									AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);
									stream << t.row();

									openServiceRequest.getPage()->setService(Env::GetOfficialEnv().getSPtr(&service));
									stream << t.col() << service.getRoute()->getCommercialLine()->getNetwork()->getName();
									stream << t.col() << service.getRoute()->getCommercialLine()->getShortName();
									stream << t.col() << HTMLModule::getLinkButton(openServiceRequest.getURL(), service.getServiceNumber());
									stream << t.col() << dynamic_cast<const StopArea*>(lineStop.getHub())->getFullName();
									stream << t.col() << dynamic_cast<const StopArea*>(lastDepartureLineStop->getHub())->getFullName();
									stream << t.col() << speed;
									stream << t.col() << dist;
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

					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);
					HTMLForm f("speed",runRequest.getURL());
									
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les tronçons de lignes parcourus trop lentement par les bus. "
							<< "Il est possible de paramétrer la vitesse minimale tolérée en rentrant sa valeur (en km/h) dans le "
							<< "champ de saisie. Par défaut, la valeur minimale tolérée est de 8km/h.<br /><br />"
							<< f.getTextInput("speed", string())
							<< f.getSubmitButton("OK")
							<< "</p>"
							<< f.close()
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// BUS LINES WITHOUT SERVICES TAB
			if (openTabContent(stream, TAB_BUS_LINES_WITHOUT_SERVICES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					std::vector<string> networks;
					if (!_networks.empty())
					{
						string network = _networks.substr(0,_networks.length()-1);
						split(networks, network, is_any_of(","));
					}

					std::vector<string> modes;
					if (!_transportModes.empty())
					{
						string mode = _transportModes.substr(0,_transportModes.length()-1);
						split(modes, mode, is_any_of(","));
					}

					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne commerciale");
					c.push_back("Ligne commerciale");
					c.push_back("Ligne commerciale");
					c.push_back("Etat");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					BOOST_FOREACH(const CommercialLine::Registry::value_type& cl, Env::GetOfficialEnv().getRegistry<CommercialLine>())
					{
						/* Filter unselected networks */
						if(!networks.empty())
						{
							bool filter = true;
							BOOST_FOREACH(const string& it, networks)
							{
								if(lexical_cast<string>(cl.second->getNetwork()->getKey()).compare(it)==0)
								{
									filter=false;
									break;
								}
							}
							if (filter)
								continue;
						}

						/* Check itinerary's services are active for 15 days at least */
						int days = 1;
						boost::gregorian::date check_date = boost::gregorian::day_clock::local_day();
						int consecutive_days = 0;
						bool valid_line = true;
						do 
						{
							bool active_route = false;
							BOOST_FOREACH(Path* it, cl.second->getPaths())
							{
								const JourneyPattern& route(static_cast<const JourneyPattern&>(*it));
								
								/* Don't check metro lines */
								if(route.getRollingStock()==NULL)
									break;
								else
								{
									if(route.getRollingStock()->getKey()==13792273858822586)
									{
										valid_line = false;
										break;
									}
									if(!modes.empty())
									{
										bool filter = true;
										BOOST_FOREACH(const string& it, modes)
										{
											if(lexical_cast<string>(route.getRollingStock()->getKey()).compare(it)==0)
												filter = false;
												break;
										}
										if(filter)
										{
											valid_line = false;
											break;
										}
									}
								}
								/* Check inactive services for each itinerary */
								if(route.isActive(check_date))
								{
									active_route = true;
									break;
								}
							}
							if(valid_line)
							{
								if(!active_route)
								{
									consecutive_days++;
								}
								else
								{
									consecutive_days=0;
								}
								days++;
								check_date += boost::gregorian::days(1);
							}
						} while(days<=15 && consecutive_days<3 && valid_line);

						if(valid_line && consecutive_days>=3)
						{
							days-=3;
							check_date -= boost::gregorian::days(3);
							AdminFunctionRequest<CommercialLineAdmin> openCLRequest(request);
							stream << t.row();

							openCLRequest.getPage()->setCommercialLine(cl.second);
							stream << t.col() << (cl.second->getNetwork()!=NULL 
								? cl.second->getNetwork()->getName() : "<b><font color=red>Pas de réseau</font></b>");
							stream << t.col() << HTMLModule::getLinkButton(openCLRequest.getURL(),
								cl.second->getName(), string(), "/admin/img/" + CommercialLineAdmin::ICON);
							stream << t.col() << cl.second->getShortName();
							stream << t.col() << cl.second->getKey();
							stream  << t.col() << check_date << " -- " 
									<< (days<=7 ? "<b><font color=red>CRITICAL</b></font>" : "<font color=orange>ALERTE</font>");
						}
					}
					stream << t.close();
				}
				else
				{
					/* Check number of networks/modes in order to prepare more filters in control */
					TransportNetworkTableSync::SearchResult networks(TransportNetworkTableSync::Search(Env::GetOfficialEnv()));
					RollingStockTableSync::SearchResult modes(RollingStockTableSync::SearchUsedModes(Env::GetOfficialEnv()));

					bool multiple_networks = false;
					int cpt = 1;

					/* Purpose more filters if there are multiple networks */
					if (networks.size() > 1)
						multiple_networks = true;

					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					HTMLForm f("filters",runRequest.getURL());
								
					stream  << f.open() 
							<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les services de bus qui n'ont pas de données rentrées en base dans "
							<< "les 15 prochains jours à compter du jour d'exécution du contrôle. Si il manque des données "
							<< "dans la semaine à venir, le contrôle renverra un CRITICAL sinon un ALERTE.<br /><br />"
					;

					BOOST_FOREACH(const RollingStockTableSync::SearchResult::value_type& it, modes)
					{
						/* Don't consider metro mode */
						if(it->getKey()==13792273858822586)
							continue;

						stream  << f.getCheckBox2("mode_"+lexical_cast<string>(cpt),lexical_cast<string>(it->getKey()),false)
								<< " " << it->getName() << "<br>";
						cpt++;
					}

					if(multiple_networks)
					{
						stream << "<br>-----------------------------------------------------------------------------<br><br>";

						cpt=1;
						BOOST_FOREACH(const TransportNetworkTableSync::SearchResult::value_type& it, networks)
						{
							stream 	<< f.getCheckBox2("network_"+lexical_cast<string>(cpt),lexical_cast<string>(it->getKey()),false) 
									<< " " << it->getName() << "<br>";
							cpt++;
						}
					}

					stream << "<br><br>" << f.getSubmitButton("OK") << "</p>" << f.close();
				}
			}


			////////////////////////////////////////////////////////////////////
			// METRO LINES WITHOUT SERVICES TAB
			if (openTabContent(stream, TAB_METRO_LINES_WITHOUT_SERVICES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("Ligne commerciale");
					c.push_back("Ligne commerciale");
					c.push_back("Etat");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

					stream << t.open();

					BOOST_FOREACH(const CommercialLine::Registry::value_type& cl, Env::GetOfficialEnv().getRegistry<CommercialLine>())
					{
						/* Check itinerary's services are active for 15 days at least */
						int days = 1;
						boost::gregorian::date check_date = boost::gregorian::day_clock::local_day();
						int consecutive_days = 0;
						bool metro_line = false;
						do 
						{
							bool active_route = false;
							BOOST_FOREACH(Path* it, cl.second->getPaths())
							{
								const JourneyPattern& route(static_cast<const JourneyPattern&>(*it));
								
								/* Only check metro lines */
								if(route.getRollingStock()==NULL)
									break;
								else
								{
									if(route.getRollingStock()->getKey()==13792273858822586)
										metro_line = true;
								}
								
								/* Check inactive services for each itinerary */
								if(route.isActive(check_date))
								{
									active_route = true;
									break;
								}
							}
							if(metro_line)
							{
								if(!active_route)
								{
									consecutive_days++;
								}
								else
								{
									consecutive_days=0;
								}
								days++;
								check_date += boost::gregorian::days(1);
							}
						} while(days<=45 && consecutive_days<1 && metro_line);

						if(metro_line && consecutive_days>=1)
						{
							days-=1;
							check_date -= boost::gregorian::days(1);
							AdminFunctionRequest<CommercialLineAdmin> openCLRequest(request);
							stream << t.row();

							openCLRequest.getPage()->setCommercialLine(cl.second);
							stream << t.col() << HTMLModule::getLinkButton(openCLRequest.getURL(),
								cl.second->getName(), string(), "/admin/img/" + CommercialLineAdmin::ICON);
							stream << t.col() << cl.second->getKey();
							stream  << t.col() << check_date << " -- " 
									<< (days<=30 ? "<b><font color=red>CRITICAL</b></font>" : "<font color=orange>ALERTE</font>");
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< "Ce contrôle recherche les services de métro qui n'ont pas de données rentrées en base dans "
							<< "les 45 prochains jours à compter du jour d'exécution du contrôle. Si il manque des données "
							<< "dans le mois à venir, le contrôle renverra un CRITICAL sinon un ALERTE.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// EDGES AND GEOMETRIES TAB
			if (openTabContent(stream, TAB_EDGES_AND_GEOMETRIES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					// Gathering different geometries of similar edges, for each pair of stops
					typedef std::map<std::pair<Vertex*, Vertex*>, set<LineString*> > GeometriesMap;
					GeometriesMap resultMap;
					boost::shared_ptr<const JourneyPattern> itinerary;

					// Scan of each stop
					BOOST_FOREACH(const StopPoint::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<StopPoint>())
					{
						// Scan of each edge starting at the stop
						BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, it.second->getDepartureEdges())
						{
							const DesignatedLinePhysicalStop* edge(dynamic_cast<DesignatedLinePhysicalStop*>(itEdge.second));
							const Edge* nextEdge(edge->getNext());

							// If the edge is a path termination, ignore it
							if(	!edge ||
								!nextEdge ||
								dynamic_cast<Junction*>(edge->getParentPath()))
							{
								continue;
							}
							else
							{
								if(edge->getParentPath()->getKey() != 0)
								{
									itinerary = Env::GetOfficialEnv().get<JourneyPattern>(edge->getParentPath()->getKey());
								}
							}

							Vertex* vertex(edge->getFromVertex());
							Vertex* nextVertex(nextEdge->getFromVertex());

							// Comparison of the edge geometry and other geometries of similar edges
							GeometriesMap::iterator itGeom(resultMap.find(make_pair(vertex, nextVertex)));

							// If no similar edges, immediate storage of the geometry
							if(itGeom == resultMap.end())
							{
								set<LineString*> geoms;
								geoms.insert(edge->getLineStop()->get<LineStringGeometry>().get());
								resultMap.insert(
									make_pair(
										make_pair(vertex, nextVertex),
										geoms
								)	);
							}
							else
							{
								bool toInsert(true);
								if(edge->getLineStop()->get<LineStringGeometry>().get())
								{
									// If a similar edge has the same geometry then ignore the current edge
									BOOST_FOREACH(const GeometriesMap::mapped_type::value_type& itg, itGeom->second)
									{
										if(itg && *edge->getLineStop()->get<LineStringGeometry>()->getCoordinatesRO() == *itg->getCoordinatesRO())
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
									itGeom->second.insert(edge->getLineStop()->get<LineStringGeometry>().get());
								}
							}
						}	
					}

					// Display of the table
					HTMLTable::ColsVector c;
					c.push_back("#");
					c.push_back("Réseau");
					c.push_back("Ligne");
					c.push_back("Itinéraire");
					c.push_back("Arrêt de départ");
					c.push_back("Arrêt de départ");
					c.push_back("Arrêt de départ");
					c.push_back("Arrêt d'arrivée");
					c.push_back("Arrêt d'arrivée");
					c.push_back("Arrêt d'arrivée");
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

						std::string line_number = lexical_cast<string>(itinerary->getCommercialLine()->getShortName());

						stream << t.row();
						stream << t.col() << r++;
						if(itinerary)
						{
							stream << t.col() << itinerary->getNetwork()->getName();
							stream << t.col() << line_number;
							stream << t.col() << itinerary->getName();
						}
						else
						{
							stream << t.col() << t.col() << t.col();
						}
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
							stream << HTMLModule::getLinkButton(copyRequest.getURL(), "Uniformiser",
								"Etes-vous sûr de vouloir uniformiser les géométries ?\\n La géométrie la plus détaillée sera conservée et copiée sur chaque ligne.");
						}
					}
					stream << t.close();
				}
				else
				{
					AdminFunctionRequest<PTQualityControlAdmin> runRequest(request, *this);
					runRequest.getPage()->setRunControl(true);

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
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
							if(lineStop->get<RankInPath>() != rank)
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

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// DOUBLE ROUTES
			if (openTabContent(stream, TAB_DOUBLE_ROUTES))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<JourneyPatternAdmin> openJourneyPatternRequest(request);

					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne");
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

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
					;
				}
			}


			////////////////////////////////////////////////////////////////////
			// SPEED
			if (openTabContent(stream, TAB_SPEED))
			{
				if(_runControl && getCurrentTab() == getActiveTab())
				{
					AdminFunctionRequest<ServiceAdmin> openServiceRequest(request);
					double maxSpeed(0);

					HTMLTable::ColsVector c;
					c.push_back("Réseau");
					c.push_back("Ligne");
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

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
					;
				}
			}


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

					stream 	<< "<p class=\"info\">Les contrôles qualité sont désactivés par défaut.<br /><br />"
							<< HTMLModule::getLinkButton(runRequest.getURL(), "Activer ce contrôle", string(), "/admin/img/" + ICON) 
							<< "</p>"
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
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const AnalysisModule*>(&module) &&
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

			_tabs.push_back(Tab("Arrêts physiques trop proches", TAB_STOPS_TOO_CLOSE, false));
			_tabs.push_back(Tab("Arrêts physiques trop loin", TAB_STOPS_TOO_FAR, false));
			_tabs.push_back(Tab("Arrêts physiques non localisés", TAB_STOPS_WITHOUT_COORDINATE, false));
			_tabs.push_back(Tab("Arrêts physiques non projetés", TAB_STOPS_NOT_PROJECTED, false));
			_tabs.push_back(Tab("Arrêts logiques sans commune", TAB_STOPAREAS_WITHOUT_CITY, false));
			_tabs.push_back(Tab("Arrêts logiques homonymes", TAB_STOPAREAS_HOMONYM, false));
			_tabs.push_back(Tab("Nom d'arrêts logiques et physiques différents", TAB_STOPAREAS_NAME_STOPS_NAME, false));
			_tabs.push_back(Tab("Communes sans rues", TAB_CITIES_WITHOUT_STREET, false));
			_tabs.push_back(Tab("Communes sans arrêts", TAB_CITIES_WITHOUT_STOP, false));
			_tabs.push_back(Tab("Communes sans arrêt principal", TAB_CITIES_WITHOUT_MAIN_STOP, false));
			_tabs.push_back(Tab("Lignes sans itinéraires", TAB_LINES_WITHOUT_ITINERARY, false));
			_tabs.push_back(Tab("Tronçons sans géométries", TAB_EDGES_WITHOUT_GEOMETRIES, false));
			_tabs.push_back(Tab("Bus trop rapide", TAB_BUS_TOO_FAST, false));
			_tabs.push_back(Tab("Bus trop lent", TAB_BUS_TOO_SLOW, false));
			_tabs.push_back(Tab("Lignes de bus sans services", TAB_BUS_LINES_WITHOUT_SERVICES, false));
			_tabs.push_back(Tab("Lignes de métro sans services", TAB_METRO_LINES_WITHOUT_SERVICES, false));
			_tabs.push_back(Tab("Tronçons mal formés", TAB_EDGES_AND_GEOMETRIES, false));
			_tabs.push_back(Tab("Continuité des rangs", TAB_RANK_CONTINUITY, false));
			_tabs.push_back(Tab("Parcours en double", TAB_DOUBLE_ROUTES, false));
			_tabs.push_back(Tab("Vitesse", TAB_SPEED, false));
			_tabs.push_back(Tab("Chronologie des horaires", TAB_ASCENDING_SCHEDULES, false));
			_tabBuilded = true;
		}
	}
}
