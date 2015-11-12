
//////////////////////////////////////////////////////////////////////////
///	PublicPlaceAdmin class implementation.
///	@file PublicPlaceAdmin.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "PublicPlaceAdmin.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "CityListFunction.h"
#include "HTMLMap.hpp"
#include "ImportableAdmin.hpp"
#include "MapSource.hpp"
#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "ProjectAddressAction.hpp"
#include "PropertiesHTMLTable.h"
#include "PTRoadsAdmin.h"
#include "PublicPlaceEntrance.hpp"
#include "PublicPlaceEntranceCreationAction.hpp"
#include "RemoveObjectAction.hpp"
#include "Road.h"
#include "RoadModule.h"
#include "RoadPlace.h"
#include "StaticActionRequest.h"
#include "User.h"

using namespace boost;
using namespace std;
using namespace geos::geom;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace geography;
	using namespace html;
	using namespace impex;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace road;

	template<> const string FactorableTemplate<AdminInterfaceElement, PublicPlaceAdmin>::FACTORY_KEY = "PublicPlaceAdmin";

	namespace admin
	{
		template<>
		const string AdminInterfaceElementTemplate<PublicPlaceAdmin>::ICON = "building.png";

		template<>
		const string AdminInterfaceElementTemplate<PublicPlaceAdmin>::DEFAULT_TITLE = "Lieu public";
	}

	namespace road
	{
		const string PublicPlaceAdmin::TAB_LOCATION = "tab_location";
		const string PublicPlaceAdmin::TAB_PROPERTIES = "tab_properties";
		const string PublicPlaceAdmin::TAB_ENTRANCES = "tab_entrances";



		PublicPlaceAdmin::PublicPlaceAdmin()
			: AdminInterfaceElementTemplate<PublicPlaceAdmin>()
		{}



		void PublicPlaceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_place = Env::GetOfficialEnv().get<PublicPlace>(
					map.getDefault<RegistryKeyType>(
						Request::PARAMETER_OBJECT_ID, 0
				)	);
			}
			catch (ObjectNotFoundException<PublicPlace>&)
			{
				throw AdminParametersException("No such place");
			}
		}



		ParametersMap PublicPlaceAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_place.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _place->getKey());
			}

			return m;
		}



		bool PublicPlaceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		void PublicPlaceAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// LOCATION TAB
			if (openTabContent(stream, TAB_LOCATION))
			{
				stream << "<h1>Carte</h1>";

				StaticActionRequest<ProjectAddressAction> moveEntranceAction(request);
				StaticActionRequest<ObjectUpdateAction> movePlaceAction(request);
				movePlaceAction.getAction()->setObject(*_place);

				boost::shared_ptr<Point> mapCenter(_place->get<PointGeometry>());

				// If the place does not contain any point, it has no coordinate : search the last created place with coordinates
				if(!mapCenter.get() || mapCenter->isEmpty())
				{
					const Registry<PublicPlace>& registry(Env::GetOfficialEnv().getRegistry<PublicPlace>());
					BOOST_REVERSE_FOREACH(Registry<PublicPlace>::value_type stopArea, registry)
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
				std::string editFieldName(ObjectUpdateAction::GetInputName<PointGeometry>());
				std::string addFieldName(ObjectUpdateAction::GetInputName<PointGeometry>());
				HTMLMap map(
					*mapCenter,
					200,
					editFieldName,
					addFieldName,
					true,
					true
				);
				map.setMapSource(MapSource::GetSessionMapSource(*request.getSession()));

				// Place icon
				if(_place->get<PointGeometry>().get())
				{
					map.addPoint(
						HTMLMap::MapPoint(
							*_place->get<PointGeometry>(),
							"/admin/img/marker.png",
							"/admin/img/marker-blue.png",
							"/admin/img/marker-gold.png",
							movePlaceAction.getURL(),
							_place->getName(),
							21, 25
					)	);
				}

				// Entrances icons
				BOOST_FOREACH(const PublicPlace::Entrances::value_type& it, _place->getEntrances())
				{
					if(!it->getGeometry().get())
					{
						continue;
					}

					moveEntranceAction.getAction()->setAddress(
						static_pointer_cast<Address, PublicPlaceEntrance>(
							Env::GetOfficialEnv().getEditableSPtr(it)
					)	);

					map.addPoint(
						HTMLMap::MapPoint(
							*it->getGeometry(),
							"/admin/img/marker-blue.png",
							"/admin/img/marker.png",
							"/admin/img/marker-gold.png",
							moveEntranceAction.getURL(),
							it->getName(),
							21, 25
					)	);
				}
				map.draw(stream, request);

				if(!_place->get<PointGeometry>().get())
				{
					StaticActionRequest<ObjectUpdateAction> locatePlaceRequest(request);
					locatePlaceRequest.getAction()->setObject(*_place);
					stream << map.getAddPointLink(locatePlaceRequest.getURL(), "Placer le lieu public");
				}
				else
				{
					StaticActionRequest<PublicPlaceEntranceCreationAction> entranceAddRequest(request);
					entranceAddRequest.getAction()->setPlace(const_pointer_cast<PublicPlace>(_place));
					stream << map.getAddPointLink(entranceAddRequest.getURL(), "Ajouter entrée");
				}
			}


			////////////////////////////////////////////////////////////////////
			// ENTRANCES TAB
			if (openTabContent(stream, TAB_ENTRANCES))
			{
				// Request
				AdminActionFunctionRequest<RemoveObjectAction, PublicPlaceAdmin> removeEntranceRequest(request, *this);
				AdminFunctionRequest<PTRoadsAdmin> openRoadRequest(request);

				// Table
				HTMLTable::ColsVector c;
				c.push_back("Nom");
				c.push_back("Rue");
				c.push_back("Numéro");
				c.push_back(string());
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				// Entrances icons
				BOOST_FOREACH(const PublicPlace::Entrances::value_type& it, _place->getEntrances())
				{
					// Row opening
					stream << t.row();

					// Name cell
					stream << t.col();
					stream << it->get<Name>();

					// Street cell
					stream << t.col();
					if(	it->getRoadChunk() &&
						it->getRoadChunk()->getRoad() &&
						it->getRoadChunk()->getRoad()->getAnyRoadPlace()
					){
						RoadPlace& roadPlace(*it->getRoadChunk()->getRoad()->getAnyRoadPlace());
						openRoadRequest.getPage()->setRoadPlace(Env::GetOfficialEnv().getSPtr(&roadPlace));
						stream <<
							HTMLModule::getHTMLLink(
								openRoadRequest.getURL(),
								roadPlace.getName()
							)
						;
					}

					// Number
					stream << t.col();
					if(it->getHouseNumber())
					{
						stream << *it->getHouseNumber();
					}

					// Remove button
					stream << t.col();
					removeEntranceRequest.getAction()->setObjectId(it->get<Key>());
					stream << HTMLModule::getLinkButton(
						removeEntranceRequest.getURL(),
						"Supprimer",
						"Etes-vous sûr de vouloir supprimer l'entrée "+ it->getName() +" ?"
					);
				}

				// Table closing
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<ObjectUpdateAction, PublicPlaceAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setObject(*_place);

				// General properties
				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_place->getKey()));
				if(_place->getCity())
				{
					stream << t.cell("Localité", _place->getCity()->getName());
				}
				stream << t.cell(
					"Localité",
					t.getForm().getTextInputAutoCompleteFromService(
						ObjectUpdateAction::_GetInputName(NamedPlaceField::FIELDS[1].name),
						_place->getCity() ? lexical_cast<string>(_place->getCity()->getKey()) : string(),
						_place->getCity() ? _place->getCity()->getName() : string(),
						pt_website::CityListFunction::FACTORY_KEY,
						pt_website::CityListFunction::DATA_CITIES,
						pt_website::CityListFunction::DATA_CITY,
						string(), string(),
						false, true, true, true
				)	);
				stream << t.cell(
					"Nom",
					t.getForm().GetTextInput(
						ObjectUpdateAction::_GetInputName(NamedPlaceField::FIELDS[0].name),
						_place->getName()
				)	);
				stream << t.close();

				// Importable admin
				StaticActionRequest<ObjectUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setObject(*_place);
				ImportableAdmin::DisplayDataSourcesTab(stream, *_place, updateOnlyRequest);
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PublicPlaceAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const RoadModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()) &&
				dynamic_cast<const PublicPlaceAdmin*>(&currentPage)
			){
				boost::shared_ptr<PublicPlaceAdmin> page(getNewCopiedPage());
				page->setPlace(dynamic_cast<const PublicPlaceAdmin&>(currentPage)._place);
				links.push_back(page);
			}

			return links;
		}



		std::string PublicPlaceAdmin::getTitle() const
		{
			return _place.get() ? _place->getName() : DEFAULT_TITLE;
		}



		bool PublicPlaceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _place->getKey() == static_cast<const PublicPlaceAdmin&>(other)._place->getKey();
		}



		void PublicPlaceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			_tabs.push_back(Tab("Localisation", TAB_LOCATION, true));
			_tabs.push_back(Tab("Entrées", TAB_ENTRANCES, true));
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));

			_tabBuilded = true;
		}
}	}

