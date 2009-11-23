////////////////////////////////////////////////////////////////////////////////
/// TransportNetworkAdmin class implementation.
///	@file TransportNetworkAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "TransportNetworkAdmin.h"
#include "EnvModule.h"

#include "AdminFunctionRequest.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "CommercialLineTableSync.h"
#include "Line.h"
#include "LineAdmin.h"
#include "TransportNetworkRight.h"
#include "ServiceAdmin.h"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "ScheduledService.h"
#include "SearchFormHTMLTable.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace security;
	using namespace html;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TransportNetworkAdmin>::FACTORY_KEY("TransportNetworkAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::ICON("chart_organisation.png");
		template<> const string AdminInterfaceElementTemplate<TransportNetworkAdmin>::DEFAULT_TITLE("Réseau inconnu");
	}

	namespace env
	{
		const string TransportNetworkAdmin::PARAMETER_SEARCH_NAME("sn");

		
		void TransportNetworkAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(objectWillBeCreatedLater) return;
			
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 100);

			try
			{
				_network = TransportNetworkTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such network");
			}
		}
		
		
		
		server::ParametersMap TransportNetworkAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_NAME,_searchName);
			if(_network.get()) m.insert(Request::PARAMETER_OBJECT_ID, _network->getKey());
			return m;
		}


		
		void TransportNetworkAdmin::display(ostream& stream, VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const
		{
			// Requests
			
			// Search form
			stream << "<h1>Recherche</h1>";
			
			AdminFunctionRequest<TransportNetworkAdmin> searchRequest(_request);
			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			HTMLForm sortedForm(s.getForm());
			stream << s.close();


			// Results display
			stream << "<h1>Lignes du réseau</h1>";
			
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					_getEnv(),
					_network->getKey()
					, string("%"+_searchName+"%"),
					optional<string>(),
					_requestParameters.first
					, _requestParameters.maxSize
					, false
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.raisingOrder				
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,sortedForm,_requestParameters, lines);

			stream << t.open();
			AdminFunctionRequest<CommercialLineAdmin> lineOpenRequest(_request);
			BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
			{
				lineOpenRequest.getPage()->setCommercialLine(line);
				stream << t.row();
				stream << t.col(1, line->getStyle(), true);
				stream << line->getShortName();
				stream << t.col();
				stream << line->getName();
				stream << t.col();
				stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");
			}
			stream << t.close();
		}

		bool TransportNetworkAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return _request.isAuthorized<TransportNetworkRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(moduleKey == EnvModule::FACTORY_KEY && isAuthorized(request))
			{
				TransportNetworkTableSync::SearchResult networks(
					TransportNetworkTableSync::Search(*_env)
				);
				BOOST_FOREACH(shared_ptr<TransportNetwork> network, networks)
				{
					shared_ptr<TransportNetworkAdmin> link(
						getNewOtherPage<TransportNetworkAdmin>(false)
					);
					link->_network = network;
					AddToLinks(links, link);
				}
			}
				
			return links;
		}

		std::string TransportNetworkAdmin::getTitle() const
		{
			return _network.get() ? _network->getName() : DEFAULT_TITLE;
		}
				

		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			const LineAdmin* la(
				dynamic_cast<const LineAdmin*>(&currentPage)
			);
			
			const CommercialLineAdmin* ca(
				dynamic_cast<const CommercialLineAdmin*>(&currentPage)
			);

			const TransportNetworkAdmin* na(
				dynamic_cast<const TransportNetworkAdmin*>(&currentPage)
			);

			const ServiceAdmin* sa(
				dynamic_cast<const ServiceAdmin*>(&currentPage)
			);

			if(	sa &&
				sa->getService().get() &&
				dynamic_cast<const Line*>(sa->getService()->getPath()) &&
				dynamic_cast<const Line*>(sa->getService()->getPath())->getCommercialLine() &&
				dynamic_cast<const Line*>(sa->getService()->getPath())->getCommercialLine()->getNetwork() &&
				dynamic_cast<const Line*>(sa->getService()->getPath())->getCommercialLine()->getNetwork()->getKey() == _network->getKey() ||
				la &&
				la->getLine().get() &&
				la->getLine()->getCommercialLine() &&
				la->getLine()->getCommercialLine()->getNetwork() &&
				la->getLine()->getCommercialLine()->getNetwork()->getKey() == _network->getKey() ||
				ca &&
				ca->getCommercialLine().get() &&
				ca->getCommercialLine()->getNetwork() &&
				ca->getCommercialLine()->getNetwork()->getKey() == _network->getKey() ||
				na &&
				na->_network.get() &&
				na->_network->getKey() == _network->getKey()
			){
				CommercialLineTableSync::SearchResult lines(
					CommercialLineTableSync::Search(
						*_env,
						_network->getKey(),
						optional<string>(),
						optional<string>(),
						0,
						optional<size_t>(),
						true, false, true,
						UP_LINKS_LOAD_LEVEL
				)	);
				BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
				{
					shared_ptr<CommercialLineAdmin> p(
						getNewOtherPage<CommercialLineAdmin>()
					);
					p->setCommercialLine(line);
					AddToLinks(links, p);
				}
			}
			return links;
		}
		
		
		bool TransportNetworkAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _network == static_cast<const TransportNetworkAdmin&>(other)._network;
		}
		
	}
}
