
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

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "CalendarTemplateAdmin.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "DRTArea.hpp"
#include "DRTAreaAdmin.hpp"
#include "GeometryField.hpp"
#include "HTMLMap.hpp"
#include "ImportableAdmin.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternAdmin.hpp"
#include "JourneyPatternCopy.hpp"
#include "LineStop.h"
#include "MapSource.hpp"
#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTModule.h"
#include "PTPlaceAdmin.h"
#include "PTRuleUserAdmin.hpp"
#include "RoadPlaceTableSync.h"
#include "RemoveObjectAction.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointInaccessibilityCalendar.hpp"
#include "StopPointInaccessibilityCalendarAdmin.hpp"
#include "StopPointUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

#include <boost/lexical_cast.hpp>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace html;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace security;
	using namespace server;
	using namespace util;

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
		const string StopPointAdmin::TAB_INACCESSIBILITY("tab_inaccessibility");



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
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Carte</h1>";

				boost::shared_ptr<Point> mapCenter(_stop->getConnectionPlace()->getPoint());

				// If the place does not contain any point, it has no coordinate : search the last created place with coordinates
				if(!mapCenter.get() || mapCenter->isEmpty())
				{
					// Last created stop in the city
					if(_stop->getConnectionPlace()->getCity())
					{
						Env newEnv;
						StopAreaTableSync::SearchResult stops(
							StopAreaTableSync::Search(
								newEnv,
								_stop->getConnectionPlace()->getCity()->getKey()
						)	);
						const Registry<StopArea>& registry(newEnv.getRegistry<StopArea>());
						BOOST_REVERSE_FOREACH(Registry<StopArea>::value_type stopArea, registry)
						{
							boost::shared_ptr<const StopArea> envStopArea(Env::GetOfficialEnv().get<StopArea>(stopArea.first));
							if(envStopArea->getPoint() && !envStopArea->getPoint()->isEmpty())
							{
								mapCenter = envStopArea->getPoint();
								break;
							}
						}
					}
				}

				// Envelope defined by the roads of the city
				if(!mapCenter.get() || mapCenter->isEmpty())
				{
					if(_stop->getConnectionPlace()->getCity())
					{
						RoadPlaceTableSync::SearchResult roads(
							RoadPlaceTableSync::Search(
								Env::GetOfficialEnv(),
								_stop->getConnectionPlace()->getCity()->getKey()
						)	);
						const Registry<RoadPlace>& registry(Env::GetOfficialEnv().getRegistry<RoadPlace>());
						Envelope e;
						BOOST_FOREACH(Registry<RoadPlace>::value_type roadPlace, registry)
						{
							if(roadPlace.second->getPoint() && !roadPlace.second->getPoint()->isEmpty())
							{
								e.expandToInclude(*roadPlace.second->getPoint()->getCoordinate());
							}
						}
						if(e.getArea() > 0)
						{
							Coordinate c;
							e.centre(c);
							mapCenter = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(c.x, c.y);
						}
					}
				}

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

				HTMLMap map(
					*mapCenter,
					18,
					ObjectUpdateAction::GetInputName<PointGeometry>(),
					ObjectUpdateAction::GetInputName<PointGeometry>(),
					false,
					true
				);
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
					map.addPoint(
						HTMLMap::MapPoint(
							*_stop->getGeometry(),
							"/admin/img/marker-blue.png",
							"/admin/img/marker.png",
							"/admin/img/marker-gold.png",
							moveAction.getURL(),
							_stop->getName() + "<br />" + popupcontent.str(),
							21,
							25
					)	);
				}

				map.draw(stream, request);

				if(!_stop->getGeometry().get())
				{
					stream << map.getAddPointLink(moveAction.getURL(), "Placer l'arrêt");
				}

				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<StopPointUpdateAction, StopPointAdmin> updateRequest(request, *this);
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

				// Accessibility admin
				PTRuleUserAdmin<StopPoint, StopPointAdmin>::Display(stream, _stop, request, false, true, false);
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
				AdminFunctionRequest<DRTAreaAdmin> openDRTAreaRequest(request);

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
							"/admin/img/" + JourneyPatternAdmin::ICON
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
					if(	journeyPattern.getEdges().size() >= 2
					){
						if(dynamic_cast<StopPoint*>(journeyPattern.getEdge(0)->getFromVertex()))
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
						else if(dynamic_cast<DRTArea*>(journeyPattern.getEdge(0)->getFromVertex()))
						{
							const DRTArea& drtArea(
								*static_cast<DRTArea*>(journeyPattern.getEdge(0)->getFromVertex())
							);
							openDRTAreaRequest.getPage()->setArea(
								Env::GetOfficialEnv().getSPtr(
									&drtArea
							)	);
							stream <<
								HTMLModule::getHTMLLink(
									openDRTAreaRequest.getURL(),
									drtArea.getName()
								)
							;
						}
					}

					// Destination
					stream << t.col();
					if(	journeyPattern.getEdges().size() >= 2
					){
						if(dynamic_cast<StopPoint*>(journeyPattern.getLastEdge()->getFromVertex()))
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
						else if(dynamic_cast<DRTArea*>(journeyPattern.getLastEdge()->getFromVertex()))
						{
							const DRTArea& drtArea(
								*static_cast<DRTArea*>(journeyPattern.getLastEdge()->getFromVertex())
							);
							openDRTAreaRequest.getPage()->setArea(
								Env::GetOfficialEnv().getSPtr(
									&drtArea
							)	);
							stream <<
								HTMLModule::getHTMLLink(
									openDRTAreaRequest.getURL(),
									drtArea.getName()
								)
							;
						}
					}

					// Arrival
					stream <<
						t.col() <<
						(	it.second.first ?
							HTMLModule::getHTMLImage("/admin/img/bullet_green.png","Arrivée possible") :
							HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Arrivée impossible")
						)
					;

					// Departure
					stream <<
						t.col() <<
						(	it.second.second ?
							HTMLModule::getHTMLImage("/admin/img/bullet_green.png","Départ possible") :
							HTMLModule::getHTMLImage("/admin/img/bullet_white.png", "Départ impossible")
						)
					;
				}
				stream << t.close();
			}

			if (openTabContent(stream, TAB_INACCESSIBILITY))
			{
				AdminActionFunctionRequest<ObjectCreateAction, StopPointAdmin> addRequest(request, *this);
				addRequest.getAction()->setTable<StopPointInaccessibilityCalendar>();
				addRequest.getAction()->set<StopPointer>(const_cast<StopPoint&>(*_stop));
				addRequest.setActionFailedPage<StopPointAdmin>();

				AdminFunctionRequest<StopPointInaccessibilityCalendarAdmin> modifyRequest(request);

				AdminActionFunctionRequest<RemoveObjectAction, StopPointAdmin> deleteRequest(request, *this);

				AdminFunctionRequest<calendar::CalendarTemplateAdmin> openRequest(request);

				HTMLForm addForm(addRequest.getHTMLForm("add"));

				map<boost::optional<graph::UserClassCode>, string> userClassMap;
				userClassMap.insert(make_pair(0, "Toutes"));
				userClassMap.insert(make_pair(USER_PEDESTRIAN, "Piétons"));
				userClassMap.insert(make_pair(USER_HANDICAPPED, "PMR"));
				userClassMap.insert(make_pair(USER_BIKE, "Vélos"));
				userClassMap.insert(make_pair(USER_CAR, "Voitures"));

				map<boost::optional<string>, string> timeMap;
				timeMap.insert(make_pair(string(), string("-")));
				for(int i = 0 ; i < 1440 ; i+=30)
				{
					stringstream timeString;
					timeString << std::setw(2) << std::setfill('0') << int(i / 60) << ":";
					timeString << std::setw(2) << std::setfill('0') << int(i % 60);
					timeMap.insert(make_pair(timeString.str(), timeString.str()));
				}

				HTMLTable::ColsVector columns;
				columns.push_back("Calendrier");
				columns.push_back("Classes d'utilisateur");
				columns.push_back("Heure de début");
				columns.push_back("Heure de fin");
				columns.push_back("Filtre horaire quotidien");
				columns.push_back(string());
				HTMLTable tableAddDisplay(columns, ResultHTMLTable::CSS_CLASS);

				stream << "<h1>Périodes d'inaccessibilité</h1>";
				stream << addForm.open();
				stream << tableAddDisplay.open();

				BOOST_FOREACH(const StopPoint::AccessibilityMask::value_type& mask, _stop->getAccessibilityMask())
				{
					stream << tableAddDisplay.row();

					stream << tableAddDisplay.col();
					openRequest.getPage()->setCalendar(
						Env::GetOfficialEnv().getSPtr(&(*mask->get<CalendarTemplatePointer>()))
					);
					stream << HTMLModule::getLinkButton(
						openRequest.getURL(),
						mask->get<CalendarTemplatePointer>()->getName(),
						string(),
						"/admin/img/" + calendar::CalendarTemplateAdmin::ICON
					);

					stream << tableAddDisplay.col();
					if(userClassMap.find(mask->get<UserClass>()) != userClassMap.end())
					{
						stream << userClassMap[mask->get<UserClass>()];
					}
					else
					{
						stream << mask->get<UserClass>();
					}

					stream << tableAddDisplay.col();
					if(!mask->get<StartHour>().is_not_a_date_time())
					{
						stream << mask->get<StartHour>();
					}
					else
					{
						stream << "-";
					}

					stream << tableAddDisplay.col();
					if(!mask->get<EndHour>().is_not_a_date_time())
					{
						stream << mask->get<EndHour>();
					}
					else
					{
						stream << "-";
					}

					stream << tableAddDisplay.col();
					stream << (mask->get<DailyTimeFilter>() ? "Oui" : "Non");

					stream << tableAddDisplay.col();
					modifyRequest.getPage()->setStopPointInaccessibilityCalendar(
						Env::GetOfficialEnv().getSPtr(mask)
					);
					deleteRequest.getAction()->setObjectId(mask->getKey());
					stream << HTMLModule::getLinkButton(modifyRequest.getURL(), "Modifier", "", "/admin/img/cog_edit.png");
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer cette période d'inaccessibilité ?", "/admin/img/delete.png");
				}

				// New row for object creation
				stream << tableAddDisplay.row();

				stream << tableAddDisplay.col() << addForm.getSelectInput(
					ObjectCreateAction::GetInputName<CalendarTemplatePointer>(),
					calendar::CalendarTemplateTableSync::GetCalendarTemplatesList(),
					optional<RegistryKeyType>(0)
				);

				stream << tableAddDisplay.col() << addForm.getSelectInput(
					ObjectCreateAction::GetInputName<UserClass>(),
					userClassMap,
					boost::optional<graph::UserClassCode>(0)
				);

				stream << tableAddDisplay.col() << addForm.getSelectInput(
					ObjectCreateAction::GetInputName<StartHour>(),
					timeMap,
					boost::optional<string>()
				);

				stream << tableAddDisplay.col() << addForm.getSelectInput(
					ObjectCreateAction::GetInputName<EndHour>(),
					timeMap,
					boost::optional<string>()
				);

				stream << tableAddDisplay.col() << addForm.getOuiNonRadioInput(
					ObjectCreateAction::GetInputName<DailyTimeFilter>(),
					false
				);
				stream << tableAddDisplay.col() << addForm.getSubmitButton("Ajouter");

				stream << tableAddDisplay.close();
				stream << addForm.close();
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
			_tabs.push_back(Tab("Périodes d'inaccessibilité", TAB_INACCESSIBILITY, profile.isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}



		AdminInterfaceElement::PageLinks StopPointAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			boost::shared_ptr<PTPlaceAdmin> p(getNewPage<PTPlaceAdmin>());
			p->setConnectionPlace(Env::GetOfficialEnv().getSPtr(
					_stop->getConnectionPlace()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewCopiedPage());

			return links;

		}
	}
}
