
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

#include "30_server/QueryString.h"

#include "15_env/TransportNetwork.h"
#include "15_env/TransportNetworkTableSync.h"
#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineAdmin.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/Line.h"
#include "15_env/LineAdmin.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;

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
		}
		
		void TransportNetworkAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
			stream << "Not yet implemented, use treeview to navigate";
		}

		bool TransportNetworkAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return false;
		}
		
		AdminInterfaceElement::PageLinks TransportNetworkAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == EnvModule::FACTORY_KEY)
			{
				vector<shared_ptr<TransportNetwork> > nets(TransportNetworkTableSync::search());
				for (vector<shared_ptr<TransportNetwork> >::const_iterator it(nets.begin()); it != nets.end(); ++it)
				{
					PageLink link;
					link.factoryKey = FACTORY_KEY;
					link.icon = ICON;
					link.name = (*it)->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString((*it)->getKey());
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
				vector<shared_ptr<CommercialLine> > lines(CommercialLineTableSync::search(_network->getKey()));
				for (vector<shared_ptr<CommercialLine> >::const_iterator it(lines.begin()); it != lines.end(); ++it)
				{
					PageLink link;
					link.factoryKey = CommercialLineAdmin::FACTORY_KEY;
					link.icon = CommercialLineAdmin::ICON;
					link.name = (*it)->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString((*it)->getKey());
					links.push_back(link);
				}
			}
			return links;
		}
	}
}
