
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
#include "AddressablePlace.h"
#include "ConnectionPlaceTableSync.h"
#include "PublicPlaceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PublicPlace.h"
#include "ResultHTMLTable.h"
#include "PhysicalStop.h"
#include "PTPlacesAdmin.h"
#include "City.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace env;
	using namespace geography;
	using namespace road;
	using namespace html;

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



		PTPlaceAdmin::PTPlaceAdmin()
			: AdminInterfaceElementTemplate<PTPlaceAdmin>()
		{ }


		
		void PTPlaceAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			try
			{
				int tableId(decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)));
				
				if(tableId == ConnectionPlaceTableSync::TABLE.ID)
				{
					setConnectionPlace(Env::GetOfficialEnv().get<PublicTransportStopZoneConnectionPlace>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)));
				}
				else if(tableId == PublicPlaceTableSync::TABLE.ID)
				{
					setPublicPlace(Env::GetOfficialEnv().get<PublicPlace>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)));
				}
				else
				{
					throw AdminParametersException("Invalid ID");
				}
			}
			catch(ObjectNotFoundException<PublicPlace> e)
			{
				throw AdminParametersException("No such public place");
			}
			catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace> e)
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
			if(_publicPlace.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _publicPlace->getKey());
			}

			return m;
		}


		
		bool PTPlaceAdmin::isAuthorized(
			const FunctionRequest<AdminRequest>& request
		) const	{
			return request.isAuthorized<TransportNetworkRight>(READ);
		}



		void PTPlaceAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const FunctionRequest<AdminRequest>& request
		) const	{
		
			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_GENERAL))
			{
			}

			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				HTMLTable::ColsVector c;
				c.push_back("Nom");
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Lignes");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& it, _connectionPlace->getPhysicalStops())
				{
					const PhysicalStop* stop(it.second);
					stream << t.row();
					stream << t.col() << stop->getName();
					stream << t.col() << stop->getX();
					stream << t.col() << stop->getY();
					stream << t.col();
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB ADDRESSES
			if (openTabContent(stream, TAB_ADDRESSES))
			{
			}

			////////////////////////////////////////////////////////////////////
			// TAB TRANSFER
			if (openTabContent(stream, TAB_TRANSFER))
			{
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}


		
		AdminInterfaceElement::PageLinks PTPlaceAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const PTPlaceAdmin* ua(
			//	dynamic_cast<const PTPlaceAdmin*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<PTPlaceAdmin> p(getNewOtherPage<PTPlaceAdmin>());
			//	AddToLinks(links, p);
			// }
			
			return links;
		}


		std::string PTPlaceAdmin::getTitle() const
		{
			if(_connectionPlace.get()) return _connectionPlace->getFullName();
			if(_publicPlace.get()) return _publicPlace->getFullName();
			return DEFAULT_TITLE;
		}



		void PTPlaceAdmin::_buildTabs( const server::FunctionRequest<admin::AdminRequest>& request ) const
		{
			_tabs.clear();
			_tabs.push_back(Tab("Propriétés", TAB_GENERAL, true));
			if(_connectionPlace.get())
			{
				_tabs.push_back(Tab("Arrêts", TAB_STOPS, true));
			}
			_tabs.push_back(Tab("Adresses", TAB_ADDRESSES, true));
			_tabs.push_back(Tab("Transferts", TAB_TRANSFER, true));
			_tabBuilded = true;
		}



		bool PTPlaceAdmin::_hasSameContent( const AdminInterfaceElement& other ) const
		{
			if(_connectionPlace.get() && static_cast<const PTPlaceAdmin&>(other)._connectionPlace.get())
			{
				return 
					_connectionPlace->getKey() == static_cast<const PTPlaceAdmin&>(other)._connectionPlace->getKey();
			}
			if(_publicPlace.get() && static_cast<const PTPlaceAdmin&>(other)._publicPlace.get())
			{
				return 
					_publicPlace->getKey() == static_cast<const PTPlaceAdmin&>(other)._publicPlace->getKey();
			}
			return false;
		}



		void PTPlaceAdmin::setConnectionPlace( boost::shared_ptr<const env::PublicTransportStopZoneConnectionPlace> value )
		{
			_connectionPlace = value;
			_addressablePlace = static_pointer_cast<const AddressablePlace, const PublicTransportStopZoneConnectionPlace>(_connectionPlace);
		}



		void PTPlaceAdmin::setPublicPlace( boost::shared_ptr<const road::PublicPlace> value )
		{
			_publicPlace = value;
			_addressablePlace = static_pointer_cast<const AddressablePlace, const PublicPlace>(_publicPlace);
		}



		AdminInterfaceElement::PageLinks PTPlaceAdmin::_getCurrentTreeBranch() const
		{
			PageLinks links;

			shared_ptr<PTPlacesAdmin> p(getNewOtherPage<PTPlacesAdmin>(false));
			p->setCity(Env::GetOfficialEnv().getSPtr(
					_connectionPlace.get() ? _connectionPlace->getCity() : _publicPlace->getCity()
			)	);
			links = p->_getCurrentTreeBranch();
			links.push_back(p);
			links.push_back(getNewPage());

			return links;
		}
	}
}
