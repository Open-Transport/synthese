
//////////////////////////////////////////////////////////////////////////
/// InterfacePageAdmin class implementation.
///	@file InterfacePageAdmin.cpp
///	@author Hugues
///	@date 2010
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

#include "InterfacePageAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "InterfaceRight.h"
#include "InterfacePage.h"
#include "Interface.h"
#include "InterfacePageUpdateAction.h"
#include "PropertiesHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace interfaces;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, InterfacePageAdmin>::FACTORY_KEY("InterfacePageAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<InterfacePageAdmin>::ICON("page.png");
		template<> const string AdminInterfaceElementTemplate<InterfacePageAdmin>::DEFAULT_TITLE("Page d'interface");
	}

	namespace interfaces
	{
		// const string InterfacePageAdmin::PARAM_SEARCH_XXX("xx");



		InterfacePageAdmin::InterfacePageAdmin()
			: AdminInterfaceElementTemplate<InterfacePageAdmin>()
		{ }



		void InterfacePageAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_page = Env::GetOfficialEnv().get<InterfacePage>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<InterfacePage> e)
			{
				throw AdminParametersException("no such page");
			}
		}



		ParametersMap InterfacePageAdmin::getParametersMap() const
		{
			ParametersMap m;

			if(_page.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _page->getKey());
			}

			return m;
		}



		bool InterfacePageAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<InterfaceRight>(READ);
		}



		void InterfacePageAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			AdminActionFunctionRequest<InterfacePageUpdateAction,InterfacePageAdmin> updateRequest(_request, *this);
			updateRequest.getAction()->setPage(const_pointer_cast<InterfacePage>(_page));

			PropertiesHTMLTable p(updateRequest.getHTMLForm());
			stream << p.open();
			stream << p.cell("Code source", p.getForm().getTextAreaInput(InterfacePageUpdateAction::PARAMETER_SOURCE, _page->getSource(), 50, 100, false));
			stream << p.close();

		}



		std::string InterfacePageAdmin::getTitle() const
		{
			return _page.get() ? _page->getFactoryKey() + (_page->getPageCode().empty() ? string() : ("/" + _page->getPageCode())) : DEFAULT_TITLE;
		}



		bool InterfacePageAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _page->getKey() == static_cast<const InterfacePageAdmin&>(other)._page->getKey();
		}



		void InterfacePageAdmin::setPage( boost::shared_ptr<const InterfacePage> page )
		{
			_page = page;
		}
	}
}
