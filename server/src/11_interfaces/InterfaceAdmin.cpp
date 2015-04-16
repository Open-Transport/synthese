
//////////////////////////////////////////////////////////////////////////
/// InterfaceAdmin class implementation.
///	@file InterfaceAdmin.cpp
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

#include "InterfaceAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "InterfaceModule.h"
#include "InterfaceRight.h"
#include "Interface.h"
#include "InterfaceTableSync.h"
#include "InterfacePage.h"
#include "InterfacePageTableSync.h"
#include "InterfacePageAdmin.h"
#include "ResultHTMLTable.h"
#include "HTMLModule.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "InterfacePageAddAction.h"

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
		template<> const string FactorableTemplate<AdminInterfaceElement, InterfaceAdmin>::FACTORY_KEY("InterfaceAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<InterfaceAdmin>::ICON("layout.png");
		template<> const string AdminInterfaceElementTemplate<InterfaceAdmin>::DEFAULT_TITLE("Interface");
	}

	namespace interfaces
	{
		InterfaceAdmin::InterfaceAdmin()
			: AdminInterfaceElementTemplate<InterfaceAdmin>()
		{ }



		void InterfaceAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_interface = Env::GetOfficialEnv().get<Interface>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<Interface> e)
			{
				throw AdminParametersException("no such interface");
			}
		}



		ParametersMap InterfaceAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_interface.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _interface->getKey());
			}

			return m;
		}



		bool InterfaceAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<InterfaceRight>(READ);
		}



		void InterfaceAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			HTMLTable::ColsVector c;
			c.push_back("Classe");
			c.push_back("Variante");
			c.push_back("Actions");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);


			AdminFunctionRequest<InterfacePageAdmin> openRequest(_request);

			AdminActionFunctionRequest<InterfacePageAddAction,InterfacePageAdmin> addRequest(_request);
			addRequest.getAction()->setInterface(const_pointer_cast<Interface>(_interface));
			addRequest.setActionWillCreateObject();

			HTMLForm f(addRequest.getHTMLForm("addpage"));

			stream << f.open();
			stream << t.open();

			InterfacePageTableSync::SearchResult pages(InterfacePageTableSync::Search(Env::GetOfficialEnv(), _interface->getKey()));
			BOOST_FOREACH(const boost::shared_ptr<InterfacePage>& page, pages)
			{
				openRequest.getPage()->setPage(const_pointer_cast<const InterfacePage>(page));

				stream << t.row();
				stream << t.col() << page->getFactoryKey();
				stream << t.col() << page->getPageCode();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + InterfacePageAdmin::ICON);
			}
			stream << t.row();
			stream << t.col() << f.getTextInput(InterfacePageAddAction::PARAMETER_CLASS, string());
			stream << t.col() << f.getTextInput(InterfacePageAddAction::PARAMETER_VARIANT, string());
			stream << t.col() << f.getSubmitButton("Ajouter");

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks InterfaceAdmin::getSubPagesOfModule(
			const server::ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if (dynamic_cast<const InterfaceModule*>(&module) &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){

				InterfaceTableSync::SearchResult interfaces(InterfaceTableSync::Search(Env::GetOfficialEnv()));
				BOOST_FOREACH(const boost::shared_ptr<Interface>& interf, interfaces)
				{
					boost::shared_ptr<InterfaceAdmin> p(getNewPage<InterfaceAdmin>());
					p->_interface = const_pointer_cast<const Interface>(interf);
					links.push_back(p);
				}
			}

			return links;
		}



		AdminInterfaceElement::PageLinks InterfaceAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			InterfacePageTableSync::SearchResult pages(InterfacePageTableSync::Search(Env::GetOfficialEnv(), _interface->getKey()));
			BOOST_FOREACH(const boost::shared_ptr<InterfacePage>& page, pages)
			{
				boost::shared_ptr<InterfacePageAdmin> p(getNewPage<InterfacePageAdmin>());
				p->setPage(const_pointer_cast<const InterfacePage>(page));
				links.push_back(p);
			}

			return links;
		}


		std::string InterfaceAdmin::getTitle() const
		{
			return _interface.get() ? _interface->getName() : DEFAULT_TITLE;
		}



		bool InterfaceAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _interface->getKey() == static_cast<const InterfaceAdmin&>(other)._interface->getKey();
		}
	}
}
