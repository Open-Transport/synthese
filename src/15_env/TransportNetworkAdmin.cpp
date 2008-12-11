
/** TransportNetworkAdmin class implementation.
	@file TransportNetworkAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TransportNetworkAdmin.h"
#include "EnvModule.h"

#include "QueryString.h"
#include "Request.h"

#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineAdmin.h"
#include "CommercialLineTableSync.h"
#include "Line.h"
#include "LineAdmin.h"
#include "TransportNetworkRight.h"

#include "AdminRequest.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"

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

		TransportNetworkAdmin::TransportNetworkAdmin()
			: AdminInterfaceElementTemplate<TransportNetworkAdmin>()
		{ }
		
		void TransportNetworkAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_network = TransportNetworkTableSync::Get(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such network");
			}

			_searchName = map.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY);
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 30);
		}
		
		void TransportNetworkAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<TransportNetworkAdmin>();
			searchRequest.setObjectId(_network->getKey());

			FunctionRequest<AdminRequest> lineOpenRequest(request);
			lineOpenRequest.getFunction()->setPage<CommercialLineAdmin>();


			// Results
			Env env;
			CommercialLineTableSync::Search(
				env,
				_network->getKey()
				, "%"+_searchName+"%"
				, _requestParameters.first
				, _requestParameters.maxSize
				, false
				, _requestParameters.orderField == PARAMETER_SEARCH_NAME
				, _requestParameters.raisingOrder				
			);
			ResultHTMLTable::ResultParameters	_resultParameters;
			_resultParameters.setFromResult(_requestParameters, env.getEditableRegistry<CommercialLine>());


			// Search form
			stream << "<h1>Recherche</h1>";
			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			HTMLForm sortedForm(s.getForm());
			stream << s.close();


			// Results display
			stream << "<h1>Résultat de la recherche</h1>";
			
			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(h,sortedForm,_requestParameters, _resultParameters);

			stream << t.open();
			BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
			{
				lineOpenRequest.setObjectId(line->getKey());
				stream << t.row();
				stream << t.col(1, line->getStyle(), true);
				stream << line->getShortName();
				stream << t.col();
				stream << line->getLongName();
				stream << t.col();
				stream << HTMLModule::getLinkButton(lineOpenRequest.getURL(), "Ouvrir", string(), "chart_line_edit.png");
			}
			stream << t.close();



		}

		bool TransportNetworkAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TransportNetworkRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == EnvModule::FACTORY_KEY)
			{
				Env env;
				TransportNetworkTableSync::Search(env);
				BOOST_FOREACH(shared_ptr<TransportNetwork> network, env.getRegistry<TransportNetwork>())
				{
					PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.name = network->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(network->getKey());
					links.push_back(link);
				}
			}
				
			return links;
		}

		std::string TransportNetworkAdmin::getTitle() const
		{
			return _network.get() ? _network->getName() : DEFAULT_TITLE;
		}

		std::string TransportNetworkAdmin::getParameterName() const
		{
			return _network.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string TransportNetworkAdmin::getParameterValue() const
		{
			return _network.get() ? Conversion::ToString(_network->getKey()) : string();
		}

		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if (currentPage.getFactoryKey() == FACTORY_KEY && _network->getKey() == static_cast<const TransportNetworkAdmin&>(currentPage)._network->getKey()
				|| currentPage.getFactoryKey() == CommercialLineAdmin::FACTORY_KEY && _network->getKey() == static_cast<const CommercialLineAdmin&>(currentPage).getCommercialLine()->getNetwork()->getKey()
				|| currentPage.getFactoryKey() == LineAdmin::FACTORY_KEY && _network->getKey() == static_cast<const LineAdmin&>(currentPage).getLine()->getCommercialLine()->getNetwork()->getKey()
			)
			{
				Env env;
				CommercialLineTableSync::Search(env, _network->getKey());
				BOOST_FOREACH(shared_ptr<CommercialLine> line, env.getRegistry<CommercialLine>())
				{
					PageLink link;
					link.factoryKey = CommercialLineAdmin::FACTORY_KEY;
					link.icon = CommercialLineAdmin::ICON;
					link.name = line->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString(line->getKey());
					links.push_back(link);
				}
			}
			return links;
		}
	}
}
