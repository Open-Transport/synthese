
//////////////////////////////////////////////////////////////////////////
///	DriverActivityAdmin class implementation.
///	@file DriverActivityAdmin.cpp
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

#include "DriverActivityAdmin.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "DriverActivity.hpp"
#include "ImportableAdmin.hpp"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace db;
	using namespace html;
	using namespace impex;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;

	template<> const string FactorableTemplate<AdminInterfaceElement, DriverActivityAdmin>::FACTORY_KEY = "DriverActivity";

	namespace admin
	{
		template<>
		const string AdminInterfaceElementTemplate<DriverActivityAdmin>::ICON = "building.png";

		template<>
		const string AdminInterfaceElementTemplate<DriverActivityAdmin>::DEFAULT_TITLE = "Activité";
	}

	namespace pt_operation
	{
		DriverActivityAdmin::DriverActivityAdmin()
			: AdminInterfaceElementTemplate<DriverActivityAdmin>()
		{ }



		void DriverActivityAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_activity = Env::GetOfficialEnv().get<DriverActivity>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch(ObjectNotFoundException<DriverActivity>&)
			{
				throw AdminParametersException("No such activity");
			}
		}



		ParametersMap DriverActivityAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_activity.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _activity->get<Key>());
			}
			return m;
		}



		bool DriverActivityAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		void DriverActivityAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			stream << "<h1>Propriétés</h1>";

			AdminActionFunctionRequest<ObjectUpdateAction, DriverActivityAdmin> updateRequest(request, *this);
			updateRequest.getAction()->setObject(*_activity);

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update_form"));
			stream << t.open();
			stream << t.cell("ID", lexical_cast<string>(_activity->get<Key>()));
			stream << t.cell(
				"Nom",
				t.getForm().getTextInput(ObjectUpdateAction::GetInputName<Name>(), _activity->get<Name>())
			);
			stream << t.close();

			{
				StaticActionRequest<ObjectUpdateAction> updateOnlyRequest(request);
				updateOnlyRequest.getAction()->setObject(*_activity);
				ImportableAdmin::DisplayDataSourcesTab(stream, *_activity, updateOnlyRequest);
			}


		}



		std::string DriverActivityAdmin::getTitle() const
		{
			return _activity.get() ? _activity->get<Name>() : DEFAULT_TITLE;
		}



		bool DriverActivityAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _activity->getKey() == static_cast<const DriverActivityAdmin&>(other)._activity->getKey();
		}
}	}

