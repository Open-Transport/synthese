
//////////////////////////////////////////////////////////////////////////
/// StopPointAdmin class implementation.
///	@file StopPointAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "StopPointAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "StopPoint.hpp"
#include "StopPointUpdateAction.hpp"
#include "StopArea.hpp"
#include "ImportableAdmin.hpp"
#include "HTMLMap.hpp"
#include "CommercialLine.h"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "MapSource.hpp"
#include "AdminFunctionRequest.hpp"
#include "JourneyPatternAdmin.hpp"
#include "CommercialLineAdmin.h"
#include "PTPlaceAdmin.h"
#include "JourneyPatternCopy.hpp"

#include <boost/lexical_cast.hpp>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace impex;
	using namespace geography;
	using namespace graph;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, StopPointAdmin>::FACTORY_KEY("StopPointAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<StopPointAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<StopPointAdmin>::DEFAULT_TITLE("Arrêt physique");
	}

	namespace pt
	{
		const string StopPointAdmin::TAB_LINKS("li");
		const string StopPointAdmin::TAB_PROPERTIES("pr");
		const string StopPointAdmin::TAB_ROUTES("tab_routes");



		StopPointAdmin::StopPointAdmin()
			: AdminInterfaceElementTemplate<StopPointAdmin>()
		{ }



		void StopPointAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_stop = Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<StopPoint>&)
			{
				throw AdminParametersException("No such physical stop");
			}
		}



		ParametersMap StopPointAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_stop.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _stop->getKey());
			}
			return m;
		}



		bool StopPointAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void StopPointAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Carte</h1>";

				shared_ptr<Point> mapCenter(_stop->getConnectionPlace()->getPoint());
				if(!mapCenter.get() || mapCenter->isEmpty()) // If the place does not contain any point, it has no coordinate : search the last created place with coordinates
				{
					const Registry<StopArea>& registry(Env::GetOfficialEnv().getRegistry<StopArea>());
					BOOST_REVERSE_FOREACH(Registry<StopArea>::value_type stopArea, registry)
					{
						if(stopArea.second->getPoint() && !stopArea.second->getPoint()->isEmpty())
						{
							mapCenter = stopArea.second->getPoint();
							break;
						}
					}
				}
				if(!mapCenter.get() || mapCenter->isEmpty())
				{
					mapCenter = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(0,0);
				}

				HTMLMap map(*mapCenter, 18, true, true, false, true);
				map.setMapSource(MapSource::GetSessionMapSource(*request.getSession()));

				StaticActionRequest<StopPointUpdateAction> moveAction(request);
				moveAction.getAction()->setStop(const_pointer_cast<StopPoint>(_stop));

				if(_stop->getGeometry().get())
				{
					stringstream popupcontent;
					BOOST_FOREACH(const CommercialLine* line, _stop->getCommercialLines())
					{
						popupcontent <<
							"<span class=\"line " << line->getStyle() << "\">" <<
							line->getShortName() <<
							"</span>"
						;
					}
					map.addPoint(HTMLMap::MapPoint(*_stop->getGeometry(), "marker-blue.png", "marker.png", "marker-gold.png", moveAction.getURL(), _stop->getName() + "<br />" + popupcontent.str(), 21, 25));
				}

				map.draw(stream, request);

				if(!_stop->getGeometry().get())
				{
					stream << map.getAddPointLink(moveAction.getURL(), "Placer l'arrêt");
				}

				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<StopPointUpdateAction, StopPointAdmin> updateRequest(request);
				updateRequest.getAction()->setStop(const_pointer_cast<StopPoint>(_stop));

				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_stop->getKey()));
				if(_stop->getConnectionPlace())
				{
					stream << t.cell(
						"Zone d'arrêt",
						_stop->getConnectionPlace()->getFullName()
					);
				}
				stream << t.cell(
					"Zone d'arrêt",
					t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_STOP_AREA, _stop->getConnectionPlace() ? lexical_cast<string>(_stop->getConnectionPlace()->getKey()) : string())
				);
				stream << t.cell("Nom", t.getForm().getTextInput(StopPointUpdateAction::PARAMETER_NAME, _stop->getName()));
				stream << t.cell(
					"X",
					t.getForm().getTextInput(
						StopPointUpdateAction::PARAMETER_X,
						_stop->getGeometry().get() ? lexical_cast<string>(_stop->getGeometry()->getX()) : string()
				)	);
				stream << t.cell(
					"Y",
					t.getForm().getTextInput(
						StopPointUpdateAction::PARAMETER_Y,
						_stop->getGeometry().get() ? lexical_cast<string>(_stop->getGeometry()->getY()) : string()
				)	);
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// OPERATOR CODES TAB
			if (openTabContent(stream, ImportableAdmin::TAB_DATA_SOURCES))
			{
				StaticActionRequest<StopPointUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setStop(const_pointer_cast<StopPoint>(_stop));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_stop, updateOnlyRequest);
			}


			////////////////////////////////////////////////////////////////////
			// ROUTES TAB
			if (openTabContent(stream, TAB_ROUTES))
			{
				// Declarations
				AdminFunctionRequest<JourneyPatternAdmin> openJourneyPatternRequest(request);
				AdminFunctionRequest<CommercialLineAdmin> openLineRequest(request);
				AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

				// Table
				HTMLTable::ColsVector c;
				c.push_back("");
				c.push_back("Ligne");
				c.push_back("Origine");
				c.push_back("Destination");
				c.push_back("A");
				c.push_back("D");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const StopPoint::JourneyPatternsMap::value_type& it, _stop->getJourneyPatterns())
				{
					// Declarations
					const JourneyPattern& journeyPattern(*it.first);

					// Avoid sublines
					if(dynamic_cast<const JourneyPatternCopy*>(&journeyPattern))
					{
						continue;
					}

					// Row
					stream << t.row();
					
					// Open button
					openJourneyPatternRequest.getPage()->setLine(Env::GetOfficialEnv().getSPtr(&journeyPattern));
					stream <<
						t.col() <<
						HTMLModule::getLinkButton(
							openJourneyPatternRequest.getURL(),
							"Ouvrir",
							string(),
							JourneyPatternAdmin::ICON
						)
					;

					// Line
					openLineRequest.getPage()->setCommercialLine(Env::GetOfficialEnv().getSPtr(journeyPattern.getCommercialLine()));
					stream <<
						t.col() <<
						"<span class=\"line " << journeyPattern.getCommercialLine()->getStyle() << "\">" <<
						HTMLModule::getHTMLLink(
							openLineRequest.getURL(),
							journeyPattern.getCommercialLine()->getShortName()
						) <<
						"</span>"
					;

					// Origin
					stream << t.col();
					if(journeyPattern.getEdges().size() >= 2)
					{
						const StopArea& stopArea(
							*static_cast<StopPoint*>(journeyPattern.getEdge(0)->getFromVertex())->getConnectionPlace()
						);
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(
								&stopArea
						)	);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								stopArea.getFullName()
							)
						;
					}

					// Destination
					stream << t.col();
					if(journeyPattern.getEdges().size() >= 2)
					{
						const StopArea& stopArea(
							*static_cast<StopPoint*>(journeyPattern.getLastEdge()->getFromVertex())->getConnectionPlace()
						);
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(
								&stopArea
						)	);
						stream <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								stopArea.getFullName()
							)
						;
					}

					// Arrival
					stream <<
						t.col() <<
						(	it.second.first ?
							HTMLModule::getHTMLImage("bullet_green.png","Arrivée possible") :
							HTMLModule::getHTMLImage("bullet_white.png", "Arrivée impossible")
						)
					;

					// Departure
					stream <<
						t.col() <<
						(	it.second.second ?
							HTMLModule::getHTMLImage("bullet_green.png","Départ possible") :
							HTMLModule::getHTMLImage("bullet_white.png", "Départ impossible")
						)
					;
				}
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		std::string StopPointAdmin::getTitle() const
		{
			return
				_stop.get() ?
				(_stop->getName().empty() ?
					lexical_cast<string>(_stop->getKey()) :
					_stop->getName()) :
				DEFAULT_TITLE
			;
		}



		bool StopPointAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _stop->getKey() == static_cast<const StopPointAdmin&>(other)._stop->getKey();
		}



		void StopPointAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Sources de données", ImportableAdmin::TAB_DATA_SOURCES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));
			_tabs.push_back(Tab("Lignes", TAB_ROUTES, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks StopPointAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			shared_ptr<PTPlaceAdmin> p(getNewPage<PTPlaceAdmin>());
			p->setConnectionPlace(Env::GetOfficialEnv().getSPtr(
					_stop->getConnectionPlace()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewCopiedPage());

			return links;

		}
	}
}
