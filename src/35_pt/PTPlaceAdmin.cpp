
//////////////////////////////////////////////////////////////////////////
/// PTPlaceAdmin class implementation.
///	@file PTPlaceAdmin.cpp
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

#include "PTPlaceAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "StopAreaTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "StopArea.hpp"
#include "ResultHTMLTable.h"
#include "StopPoint.hpp"
#include "PTPlacesAdmin.h"
#include "City.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "JunctionTableSync.hpp"
#include "StopAreaUpdateAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "StopPointAdmin.hpp"
#include "LineStop.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "StopPointAddAction.hpp"
#include "HTMLMap.hpp"
#include "StaticActionRequest.h"
#include "DBModule.h"
#include "StopAreaTransferAddAction.h"
#include "ImportableAdmin.hpp"
#include "StopAreaTransferRemoveAction.hpp"
#include "JunctionUpdateAction.hpp"
#include "RemoveObjectAction.hpp"
#include "StopPointUpdateAction.hpp"
#include "PlaceAliasUpdateAction.hpp"
#include "PlaceAliasTableSync.h"
#include "MapSource.hpp"

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace html;
	using namespace db;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTPlaceAdmin>::FACTORY_KEY("PTPlaceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTPlaceAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTPlaceAdmin>::DEFAULT_TITLE("Arrêt");
	}

	namespace pt
	{
		const string PTPlaceAdmin::TAB_ADDRESSES("ta");
		const string PTPlaceAdmin::TAB_GENERAL("tg");
		const string PTPlaceAdmin::TAB_STOPS("ts");
		const string PTPlaceAdmin::TAB_TRANSFER("tt");
		const string PTPlaceAdmin::TAB_ALIAS("tl");



		PTPlaceAdmin::PTPlaceAdmin()
			: AdminInterfaceElementTemplate<PTPlaceAdmin>()
		{ }



		void PTPlaceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				RegistryTableType tableId(decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)));

				setConnectionPlace(Env::GetOfficialEnv().get<StopArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)));
			}
			catch(ObjectNotFoundException<StopArea> e)
			{
				throw AdminParametersException("No such connection place");
			}

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTPlaceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_connectionPlace.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _connectionPlace->getKey());
			}

			return m;
		}



		bool PTPlaceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTPlaceAdmin::display(
			ostream& stream,
			const AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_GENERAL))
			{
				if(_connectionPlace.get())
				{
					stream << "<h1>Carte</h1>";

					StaticActionRequest<StopPointUpdateAction> moveAction(request);

					shared_ptr<Point> mapCenter(_connectionPlace->getPoint());
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
					HTMLMap map(*mapCenter, 200, true, true, true, true);
					map.setMapSource(MapSource::GetSessionMapSource(*request.getSession()));

					BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it, _connectionPlace->getPhysicalStops())
					{
						if(!it.second->getGeometry().get())
						{
							continue;
						}

						moveAction.getAction()->setStop(Env::GetOfficialEnv().getEditableSPtr(const_cast<StopPoint*>(it.second)));

						stringstream popupcontent;
						set<const CommercialLine*> lines;
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
						{
							if(!dynamic_cast<const LineStop*>(edge.second))
							{
								continue;
							}
							lines.insert(
								static_cast<const LineStop*>(edge.second)->getLine()->getCommercialLine()
							);
						}
						BOOST_FOREACH(const CommercialLine* line, lines)
						{
							popupcontent <<
								"<span class=\"line " << line->getStyle() << "\">" <<
								line->getShortName() <<
								"</span>"
							;
						}
						map.addPoint(HTMLMap::MapPoint(*it.second->getGeometry(), "marker-blue.png", "marker.png", "marker-gold.png", moveAction.getURL(), it.second->getName() + "<br />" + popupcontent.str(), 21, 25));
					}
					/*
					BOOST_FOREACH(const AddressablePlace::Addresses::value_type& address, _addressablePlace->getAddresses())
					{
						map.addPoint(HTMLMap::Point(*address, "marker-green.png", "marker.png", "marker-gold.png", string(), string()));
					}
					*/
					/// @todo Station entrances
					map.draw(stream, request);

					StaticActionRequest<StopPointAddAction> stopPointAddRequest(request);
					stopPointAddRequest.getAction()->setPlace(const_pointer_cast<StopArea>(_connectionPlace));
					stream << map.getAddPointLink(stopPointAddRequest.getURL(), "Ajouter arrêt");


					AdminActionFunctionRequest<StopAreaUpdateAction,PTPlaceAdmin> updateRequest(request);
					updateRequest.getAction()->setPlace(const_pointer_cast<StopArea>(_connectionPlace));

					stream << "<h1>Propriétés</h1>";

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("ID", lexical_cast<string>(_connectionPlace->getKey()));
					stream << t.title("Localisation");
					stream << t.cell("Localité", _connectionPlace->getCity()->getName());
					stream << t.cell("Localité", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_CITY_ID, lexical_cast<string>(_connectionPlace->getCity()->getKey())));
					stream << t.cell("Arrêt principal", t.getForm().getOuiNonRadioInput(StopAreaUpdateAction::PARAMETER_IS_MAIN, _connectionPlace->getCity()->includes(_connectionPlace.get())));
					stream << t.cell("Nom", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_NAME, _connectionPlace->getName()));
					stream << t.title("Destination sur afficheur");
					stream << t.cell("Nom court", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_SHORT_NAME, _connectionPlace->getName13()));
					stream << t.cell("Nom moyen", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_LONG_NAME, _connectionPlace->getName26()));
					stream << t.cell("Nom pour fiche horaire", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_TIMETABLE_NAME, _connectionPlace->getTimetableName()));
					stream << t.close();
				}


				StaticActionRequest<StopAreaUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setPlace(const_pointer_cast<StopArea>(_connectionPlace));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_connectionPlace, updateOnlyRequest);
			}

			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				AdminFunctionRequest<StopPointAdmin> openRequest(request);

				AdminActionFunctionRequest<StopPointAddAction,PTPlaceAdmin> addRequest(request);
				addRequest.getAction()->setPlace(const_pointer_cast<StopArea>(_connectionPlace));

				AdminActionFunctionRequest<RemoveObjectAction,PTPlaceAdmin> removeRequest(request);
				
				HTMLForm f(addRequest.getHTMLForm());
				stream << f.open();

				HTMLTable::ColsVector c;
				c.push_back("");
				c.push_back("Nom");
				c.push_back("Code exploitant");
				c.push_back("Lon");
				c.push_back("Lat");
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Lignes");
				c.push_back("Actions");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it, _connectionPlace->getPhysicalStops())
				{
					const StopPoint* stop(it.second);
					openRequest.getPage()->setStop(Env::GetOfficialEnv().getSPtr(stop));

					stream << fixed;
					stream << t.row();

					// Open button
					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), StopPointAdmin::ICON);

					// Name cell
					stream << t.col() << stop->getName();
					stream << t.col() << stop->getCodeBySources();

					if(stop->getGeometry().get())
					{
						shared_ptr<Point> pt(DBModule::GetStorageCoordinatesSystem().convertPoint(*stop->getGeometry()));
						stream << t.col() << pt->getX();
						stream << t.col() << pt->getY();
						stream << t.col() << stop->getGeometry()->getX();
						stream << t.col() << stop->getGeometry()->getY();
					}
					else
					{
						stream << t.col();
						stream << t.col();
						stream << t.col();
						stream << t.col();
					}

					// Lines cell
					stream << t.col();
					set<const CommercialLine*> lines;
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop->getDepartureEdges())
					{
						if(!dynamic_cast<const LineStop*>(edge.second))
						{
							continue;
						}
						lines.insert(
							static_cast<const LineStop*>(edge.second)->getLine()->getCommercialLine()
						);
					}
					BOOST_FOREACH(const CommercialLine* line, lines)
					{
						stream <<
							"<span class=\"line " << line->getStyle() << "\">" <<
							line->getShortName() <<
							"</span>"
						;
					}

					// Remove button only if the stop is not used by any route
					stream << t.col();
					if(stop->getDepartureEdges().empty() && stop->getArrivalEdges().empty())
					{
						removeRequest.getAction()->setObjectId(stop->getKey());
						stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'arrêt ?");
					}
				}
				stream << t.row();
				stream << t.col();
				stream << t.col() << f.getTextInput(StopPointAddAction::PARAMETER_NAME, string());
				stream << t.col() << f.getTextInput(StopPointAddAction::PARAMETER_OPERATOR_CODE, string());
				stream << t.col() << f.getTextInput(StopPointAddAction::PARAMETER_X, string());
				stream << t.col() << f.getTextInput(StopPointAddAction::PARAMETER_Y, string());
				stream << t.col();
				stream << t.col();
				stream << t.col() << f.getSubmitButton("Ajouter");
				stream << t.col();

				stream << t.close() << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB ADDRESSES
			if (openTabContent(stream, TAB_ADDRESSES))
			{	/// @todo Station entrances
/*				HTMLTable::ColsVector c;
				c.push_back("Longitude");
				c.push_back("Latitude");
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Rues");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const AddressablePlace::Addresses::value_type& address, _addressablePlace->getAddresses())
				{
					stream << t.row();
					stream << t.col() << address->getLongitude();
					stream << t.col() << address->getLatitude();
					stream << t.col() << address->x;
					stream << t.col() << address->y;
					stream << t.col();
				}
				stream << t.close();
*/			}

			////////////////////////////////////////////////////////////////////
			// TAB TRANSFER
			if (openTabContent(stream, TAB_TRANSFER))
			{
				if(_connectionPlace.get())
				{
					stream << "<h1>Propriétés</h1>";

					AdminActionFunctionRequest<StopAreaUpdateAction,PTPlaceAdmin> updateRequest(request);
					updateRequest.getAction()->setPlace(const_pointer_cast<StopArea>(_connectionPlace));

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Correspondance autorisée", t.getForm().getOuiNonRadioInput(StopAreaUpdateAction::PARAMETER_ALLOWED_CONNECTIONS, _connectionPlace->getAllowedConnection()));
					stream << t.cell("Délai de correspondance par défaut (minutes)", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_connectionPlace->getDefaultTransferDelay().total_seconds() / 60)));
					stream << t.close();
				}

				stream << "<h1>Transferts internes (correspondances)</h1>";
				{
					AdminActionFunctionRequest<StopAreaTransferAddAction,PTPlaceAdmin> addTransferRequest(request);

					AdminActionFunctionRequest<StopAreaTransferRemoveAction,PTPlaceAdmin> removeTransferRequest(request);

					HTMLForm f(addTransferRequest.getHTMLForm("addtransfer"));
					stream << f.open();

					HTMLTable::ColsVector c;
					c.push_back("Quai départ");
					c.push_back("Quai arrivée");
					c.push_back("Durée");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					stream << t.row();
					stream << t.col(2) << "<b>Valeur par défaut</b>";
					stream << t.col() << (_connectionPlace->getDefaultTransferDelay().total_seconds() / 60) << " min";
					stream << t.col();

					BOOST_FOREACH(const StopArea::TransferDelaysMap::value_type& it, _connectionPlace->getTransferDelays())
					{
						shared_ptr<StopPoint> startStop(Env::GetOfficialEnv().getEditable<StopPoint>(it.first.first));
						shared_ptr<StopPoint> endStop(Env::GetOfficialEnv().getEditable<StopPoint>(it.first.second));
						removeTransferRequest.getAction()->setFrom(startStop);
						removeTransferRequest.getAction()->setTo(endStop);

						stream << t.row();
						stream << t.col() << startStop->getCodeBySources() << " / " << startStop->getName();
						stream << t.col() << endStop->getCodeBySources() << " / " << endStop->getName();
						stream << t.col() << (it.second.total_seconds() / 60) << " min";
						stream << t.col() << HTMLModule::getLinkButton(removeTransferRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le délai de correspondance ?");
					}

					stream << t.row();
					stream << t.col() << f.getSelectInput(StopAreaTransferAddAction::PARAMETER_FROM_ID, _connectionPlace->getPhysicalStopLabels(), optional<RegistryKeyType>());
					stream << t.col() << f.getSelectInput(StopAreaTransferAddAction::PARAMETER_TO_ID, _connectionPlace->getPhysicalStopLabels(), optional<RegistryKeyType>());
					stream << t.col() << f.getTextInput(StopAreaTransferAddAction::PARAMETER_DURATION,string(),"(minutes ou F)");
					stream << t.col() << f.getSubmitButton("Ajouter");

					stream << t.close();
					stream << f.close();
				}

				if(_connectionPlace.get())
				{
					AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

					AdminActionFunctionRequest<JunctionUpdateAction,PTPlaceAdmin> addJunctionRequest(request);

					AdminActionFunctionRequest<RemoveObjectAction,PTPlaceAdmin> removeJunctionRequest(request);

					stream << "<h1>Transferts externes (jonctions)</h1>";

					JunctionTableSync::SearchResult startings(
						JunctionTableSync::Search(Env::GetOfficialEnv(), _connectionPlace->getKey())
					);
					JunctionTableSync::SearchResult endings(
						JunctionTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), _connectionPlace->getKey())
					);
					JunctionTableSync::SearchResult allJunctions(startings);
					allJunctions.insert(allJunctions.end(), endings.begin(), endings.end());

					HTMLForm f(addJunctionRequest.getHTMLForm("addJunction"));
					stream << f.open();

					HTMLTable::ColsVector c;
					c.push_back("Quai");
					c.push_back("Arrêt");
					c.push_back("Quai");
					c.push_back("Longueur");
					c.push_back("Durée");
					c.push_back("Action");

					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					stream << t.row();
					stream << t.col(6, string(), true) << "Jonctions bidirectionnelles";
					BOOST_FOREACH(shared_ptr<Junction> junction, allJunctions)
					{
						if(!junction->getBack())
						{
							continue;
						}
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(junction->getEnd()->getConnectionPlace())
						);
						removeJunctionRequest.getAction()->setObjectId(junction->getKey());

						stream << t.row();
						stream << t.col() << junction->getStart()->getName();
						stream << t.col() <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								junction->getEnd()->getConnectionPlace()->getFullName()
							);
						stream << t.col() << junction->getEnd()->getName();
						stream << t.col() << junction->getLength() << " m";
						stream << t.col() << (junction->getDuration().total_seconds() / 60) << " min";
						stream << t.col() << HTMLModule::getLinkButton(
							removeJunctionRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le transfert ?"
						);

					}

					stream << t.row();
					stream << t.col(6, string(), true) << "Jonctions au départ de " << _connectionPlace->getFullName();
					BOOST_FOREACH(shared_ptr<Junction> junction, startings)
					{
						if(junction->getBack())
						{
							continue;
						}
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(junction->getEnd()->getConnectionPlace())
						);
						removeJunctionRequest.getAction()->setObjectId(junction->getKey());

						stream << t.row();
						stream << t.col() << junction->getStart()->getName();
						stream << t.col() << HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								junction->getEnd()->getConnectionPlace()->getFullName()
							);
						stream << t.col() << junction->getEnd()->getName();
						stream << t.col() << junction->getLength() << " m";
						stream << t.col() << (junction->getDuration().total_seconds() / 60) << " min";
						stream << t.col() << HTMLModule::getLinkButton(
							removeJunctionRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le transfert ?"
						);
					}

					stream << t.row();
					stream << t.col(6, string(), true) << "Jonctions vers " << _connectionPlace->getFullName();
					BOOST_FOREACH(shared_ptr<Junction> junction, endings)
					{
						if(junction->getBack())
						{
							continue;
						}
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(junction->getStart()->getConnectionPlace())
						);
						removeJunctionRequest.getAction()->setObjectId(junction->getKey());

						stream << t.row();
						stream << t.col() << junction->getEnd()->getName();
						stream << t.col() <<
							HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								junction->getStart()->getConnectionPlace()->getFullName()
							);
						stream << t.col() << junction->getStart()->getName();
						stream << t.col() << junction->getLength() << " m";
						stream << t.col() << (junction->getDuration().total_seconds() / 60) << " min";
						stream << t.col() << HTMLModule::getLinkButton(
							removeJunctionRequest.getURL(),
							"Supprimer",
							"Etes-vous sûr de vouloir supprimer le transfert ?"
						);
					}

					stream << t.row();
					stream << t.col() << f.getSelectInput(JunctionUpdateAction::PARAMETER_FROM_ID, _connectionPlace->getPhysicalStopLabels(), optional<RegistryKeyType>());
					stream << t.col(2) << f.getTextInput(JunctionUpdateAction::PARAMETER_TO_ID, string());
					stream << t.col() << f.getTextInput(JunctionUpdateAction::PARAMETER_LENGTH, string());
					stream << t.col() << f.getTextInput(JunctionUpdateAction::PARAMETER_TIME, string());
					stream << t.col() << f.getOuiNonRadioInput(JunctionUpdateAction::PARAMETER_BIDIRECTIONAL,true);
					stream << f.getSubmitButton("Ajouter");

					stream << t.close();
					stream << f.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB ALIAS
			if (openTabContent(stream, TAB_ALIAS))
			{
				HTMLTable::ColsVector c;
				c.push_back("Commune");
				c.push_back("Nom");
				c.push_back("Principal");
				c.push_back("Actions");

				AdminActionFunctionRequest<RemoveObjectAction,PTPlaceAdmin> removeRequest(request);

				AdminActionFunctionRequest<PlaceAliasUpdateAction,PTPlaceAdmin> addRequest(request);
				addRequest.getAction()->setStopArea(_connectionPlace);

				HTMLForm f(addRequest.getHTMLForm("addAlias"));
				stream << f.open();
				
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				PlaceAliasTableSync::SearchResult aliases(
					PlaceAliasTableSync::Search(
						*_env,
						_connectionPlace->getKey()
				)	);
				BOOST_FOREACH(shared_ptr<PlaceAlias> alias, aliases)
				{
					removeRequest.getAction()->setObjectId(alias->getKey());
					stream << t.row();
					stream << t.col() << alias->getCity()->getName();
					stream << t.col() << alias->getName();
					stream << t.col() << (alias->getCity()->includes(alias.get()) ? "OUI" : "NON");
					stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer l'alias ?");
				}

				stream << t.row();
				stream << t.col() << f.getTextInput(PlaceAliasUpdateAction::PARAMETER_CITY_ID, lexical_cast<string>(_connectionPlace->getCity()->getKey()));
				stream << t.col() << f.getTextInput(PlaceAliasUpdateAction::PARAMETER_NAME, string());
				stream << t.col() << f.getOuiNonRadioInput(PlaceAliasUpdateAction::PARAMETER_IS_CITY_MAIN_PLACE, false);
				stream << t.col() << f.getSubmitButton("Ajouter");

				stream << t.close();
				stream << f.close();
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTPlaceAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;


			BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it, _connectionPlace->getPhysicalStops())
			{
				shared_ptr<StopPointAdmin> p(getNewPage<StopPointAdmin>());
				p->setStop(Env::GetOfficialEnv().getSPtr(it.second));
				links.push_back(p);
			}

			return links;
		}


		std::string PTPlaceAdmin::getTitle() const
		{
			if(_connectionPlace.get()) return _connectionPlace->getFullName();
			return DEFAULT_TITLE;
		}



		void PTPlaceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			_tabs.push_back(Tab("Général", TAB_GENERAL, true));
			if(_connectionPlace.get())
			{
				_tabs.push_back(Tab("Arrêts", TAB_STOPS, true));
			}
			_tabs.push_back(Tab("Adresses", TAB_ADDRESSES, true));
			_tabs.push_back(Tab("Transferts", TAB_TRANSFER, true));
			_tabs.push_back(Tab("Alias", TAB_ALIAS, true));
			_tabBuilded = true;
		}



		bool PTPlaceAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			if(_connectionPlace.get() && static_cast<const PTPlaceAdmin&>(other)._connectionPlace.get())
			{
				return
					_connectionPlace->getKey() == static_cast<const PTPlaceAdmin&>(other)._connectionPlace->getKey();
			}
			return false;
		}



		AdminInterfaceElement::PageLinks PTPlaceAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			shared_ptr<PTPlacesAdmin> p(getNewPage<PTPlacesAdmin>());
			p->setCity(Env::GetOfficialEnv().getSPtr(
					_connectionPlace->getCity()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(getNewCopiedPage());

			return links;
		}
	}
}
