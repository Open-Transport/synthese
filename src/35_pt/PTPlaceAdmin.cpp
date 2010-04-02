
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
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "JunctionTableSync.hpp"
#include "Address.h"
#include "StopAreaUpdateAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaNameUpdateAction.hpp"

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
			const ParametersMap& map
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
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTPlaceAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{
		
			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_GENERAL))
			{
				if(_connectionPlace.get())
				{
					AdminActionFunctionRequest<StopAreaNameUpdateAction,PTPlaceAdmin> updateRequest(request);
					updateRequest.getAction()->setPlace(const_pointer_cast<PublicTransportStopZoneConnectionPlace>(_connectionPlace));

					stream << "<h1>Propriétés</h1>";

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.title("Localisation");
					stream << t.cell("Localité", _connectionPlace->getCity()->getName());
					stream << t.cell("Localité", t.getForm().getTextInput(StopAreaNameUpdateAction::PARAMETER_CITY_ID, lexical_cast<string>(_connectionPlace->getCity()->getKey())));
					stream << t.cell("Nom", t.getForm().getTextInput(StopAreaNameUpdateAction::PARAMETER_NAME, _connectionPlace->getName()));
					stream << t.cell("Code import", t.getForm().getTextInput(StopAreaNameUpdateAction::PARAMETER_CODE, _connectionPlace->getCodeBySource()));
					stream << t.title("Destination sur afficheur");
					stream << t.cell("Nom court", t.getForm().getTextInput(StopAreaNameUpdateAction::PARAMETER_SHORT_NAME, _connectionPlace->getName13()));
					stream << t.cell("Nom moyen", t.getForm().getTextInput(StopAreaNameUpdateAction::PARAMETER_LONG_NAME, _connectionPlace->getName26()));
					stream << t.close();
				}
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
				HTMLTable::ColsVector c;
				c.push_back("X");
				c.push_back("Y");
				c.push_back("Rues");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const AddressablePlace::Addresses::value_type& address, _addressablePlace->getAddresses())
				{
					stream << t.row();
					stream << t.col() << address->getX();
					stream << t.col() << address->getY();
					stream << t.col();
				}
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB TRANSFER
			if (openTabContent(stream, TAB_TRANSFER))
			{
				if(_connectionPlace.get())
				{
					stream << "<h1>Propriétés</h1>";

					AdminActionFunctionRequest<StopAreaUpdateAction,PTPlaceAdmin> updateRequest(request);
					updateRequest.getAction()->setPlace(const_pointer_cast<PublicTransportStopZoneConnectionPlace>(_connectionPlace));

					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Correspondance autorisée", t.getForm().getOuiNonRadioInput(StopAreaUpdateAction::PARAMETER_ALLOWED_CONNECTIONS, _connectionPlace->getAllowedConnection()));
					stream << t.cell("Délai de correspondance par défaut (minutes)", t.getForm().getTextInput(StopAreaUpdateAction::PARAMETER_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_connectionPlace->getDefaultTransferDelay().total_seconds() / 60)));
					stream << t.close();
				}

				stream << "<h1>Transferts internes (correspondances)</h1>";
				{
					HTMLTable::ColsVector c;
					c.push_back("Quai départ");
					c.push_back("Quai arrivée");
					c.push_back("Durée");
					c.push_back("Action");
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					stream << t.open();
					stream << t.row();
					stream << t.col(2) << "<b>Valeur par défaut</b>";
					stream << t.col() << (_addressablePlace->getDefaultTransferDelay().total_seconds() / 60) << " min";
					stream << t.col();

					BOOST_FOREACH(const AddressablePlace::TransferDelaysMap::value_type& it, _addressablePlace->getTransferDelays())
					{
						stream << t.row();
						stream << t.col() << it.first.first;
						stream << t.col() << it.first.second;
						stream << t.col() << (it.second.total_seconds() / 60) << " min";
						stream << t.col() << "Supprimer";
					}

					stream << t.row();
					stream << t.col();
					stream << t.col();
					stream << t.col();
					stream << t.col() << "Ajouter";
					
					stream << t.close();

				}

				if(_connectionPlace.get())
				{
					AdminFunctionRequest<PTPlaceAdmin> openPlaceRequest(request);

					stream << "<h1>Transferts externes (jonctions)</h1>";

					JunctionTableSync::SearchResult startings(
						JunctionTableSync::Search(Env::GetOfficialEnv(), _connectionPlace->getKey())
					);
					JunctionTableSync::SearchResult endings(
						JunctionTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), _connectionPlace->getKey())
					);

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
					BOOST_FOREACH(shared_ptr<Junction> junction, startings)
					{
						if(!junction->getBack())
						{
							continue;
						}
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(junction->getEnd()->getConnectionPlace())
						);

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
						stream << t.col() << "Fusionner l'arrêt";
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
						stream << t.row();
						stream << t.col() << junction->getStart()->getName();
						stream << t.col() << HTMLModule::getHTMLLink(
								openPlaceRequest.getURL(),
								junction->getEnd()->getConnectionPlace()->getFullName()
							);
						stream << t.col() << junction->getEnd()->getName();
						stream << t.col() << junction->getLength() << " m";
						stream << t.col() << (junction->getDuration().total_seconds() / 60) << " min";
						stream << t.col() << "Fusionner l'arrêt";
					}

					stream << t.row();
					stream << t.col(6, string(), true) << "Jonctions vers " << _connectionPlace->getFullName();
					BOOST_FOREACH(shared_ptr<Junction> junction, startings)
					{
						if(junction->getBack())
						{
							continue;
						}
						openPlaceRequest.getPage()->setConnectionPlace(
							Env::GetOfficialEnv().getSPtr(junction->getStart()->getConnectionPlace())
						);
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
						stream << t.col() << "Fusionner l'arrêt";
					}

					stream << t.close();
				}
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



		void PTPlaceAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
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
			links.push_back(getNewPage());

			return links;
		}
	}
}
