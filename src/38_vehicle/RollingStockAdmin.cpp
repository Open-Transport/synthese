
//////////////////////////////////////////////////////////////////////////
/// RollingStockAdmin class implementation.
///	@file RollingStockAdmin.cpp
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

#include "RollingStockAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "User.h"
#include "RollingStock.hpp"
#include "RollingStockUpdateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "StopArea.hpp"
#include "ImportableAdmin.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace html;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, RollingStockAdmin>::FACTORY_KEY("RollingStockAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<RollingStockAdmin>::ICON("database_edit.png");
		template<> const string AdminInterfaceElementTemplate<RollingStockAdmin>::DEFAULT_TITLE("Mode");
	}

	namespace vehicle
	{
		RollingStockAdmin::RollingStockAdmin()
			: AdminInterfaceElementTemplate<RollingStockAdmin>()
		{ }



		void RollingStockAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_rollingStock = Env::GetOfficialEnv().get<RollingStock>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<RollingStock>&)
			{
				throw AdminParametersException("No such rolling stock");
			}
		}



		ParametersMap RollingStockAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_rollingStock.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _rollingStock->getKey());
			}

			return m;
		}



		bool RollingStockAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true; // TODO create vehicle right
			// return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void RollingStockAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			{
				stream << "<h1>Propriétés</h1>";

				AdminActionFunctionRequest<RollingStockUpdateAction, RollingStockAdmin> updateRequest(request, *this);
				updateRequest.getAction()->setRollingStock(const_pointer_cast<RollingStock>(_rollingStock));

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_rollingStock->getKey()));
				stream << t.cell("Nom", t.getForm().GetTextInput(RollingStockUpdateAction::PARAMETER_NAME, _rollingStock->getName()));
				stream << t.cell("Article", t.getForm().GetTextInput(RollingStockUpdateAction::PARAMETER_ARTICLE, _rollingStock->getArticle()));
				stream << t.cell("Emission de CO2 en g/km/voyageur", t.getForm().GetTextInput(RollingStockUpdateAction::PARAMETER_CO2_EMISSIONS, lexical_cast<string>(_rollingStock->getCO2Emissions())));
				stream << t.cell("Consommation énergétique en L/100km équivalent pétrole", t.getForm().GetTextInput(RollingStockUpdateAction::PARAMETER_ENERGY_CONSUMPTION, lexical_cast<string>(_rollingStock->getEnergyConsumption())));
				stream << t.close();

				StaticActionRequest<RollingStockUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setRollingStock(const_pointer_cast<RollingStock>(_rollingStock));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_rollingStock, updateOnlyRequest);
			}
		}



		std::string RollingStockAdmin::getTitle() const
		{
			return _rollingStock.get() ? _rollingStock->getName() : DEFAULT_TITLE;
		}



		bool RollingStockAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _rollingStock->getKey() == static_cast<const RollingStockAdmin&>(other)._rollingStock->getKey();
		}
}	}
