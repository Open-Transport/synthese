
//////////////////////////////////////////////////////////////////////////
/// DepotAdmin class implementation.
///	@file DepotAdmin.cpp
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

#include "DepotAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "PTOperationModule.hpp"
#include "User.h"
#include "Depot.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "DepotUpdateAction.hpp"
#include "DepotTableSync.hpp"
#include "AdminFunctionRequest.hpp"
#include "DepotTableSync.hpp"
#include "ImportableAdmin.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace html;
	using namespace db;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DepotAdmin>::FACTORY_KEY("Depot");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DepotAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<DepotAdmin>::DEFAULT_TITLE("Dépôt");
	}

	namespace pt_operation
	{
		DepotAdmin::DepotAdmin()
			: AdminInterfaceElementTemplate<DepotAdmin>()
		{ }



		void DepotAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_depot = Env::GetOfficialEnv().get<Depot>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<Depot>&)
			{
				throw AdminParametersException("No such vehicle");
			}
		}



		ParametersMap DepotAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_depot.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _depot->getKey());
			}

			return m;
		}



		bool DepotAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void DepotAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			stream << "<h1>Propriétés</h1>";

			AdminActionFunctionRequest<DepotUpdateAction, DepotAdmin> updateRequest(request, *this);
			updateRequest.getAction()->setDepot(const_pointer_cast<Depot>(_depot));

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update_form"));
			stream << t.open();
			stream << t.cell("Nom", t.getForm().getTextInput(DepotUpdateAction::PARAMETER_NAME, _depot->getName()));
			stream << t.close();

			{
				StaticActionRequest<DepotUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setDepot(const_pointer_cast<Depot>(_depot));
				ImportableAdmin::DisplayDataSourcesTab(stream, *_depot, updateOnlyRequest);
			}
		}



		std::string DepotAdmin::getTitle() const
		{
			return _depot.get() ? _depot->getName() : DEFAULT_TITLE;
		}



		bool DepotAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _depot->getKey()  == static_cast<const DepotAdmin&>(other)._depot->getKey();
		}
}	}
