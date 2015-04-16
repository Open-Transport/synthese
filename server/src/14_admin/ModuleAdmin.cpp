////////////////////////////////////////////////////////////////////////////////
/// ModuleAdmin class implementation.
///	@file ModuleAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ModuleAdmin.h"
#include "ModuleClass.h"
#include "HTMLModule.h"
#include "AdminParametersException.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace admin;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ModuleAdmin>::FACTORY_KEY("ModuleAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ModuleAdmin>::ICON("brick.png");
		template<> const string AdminInterfaceElementTemplate<ModuleAdmin>::DEFAULT_TITLE("Module inconnu");
	}

	namespace admin
	{
		const string ModuleAdmin::PARAMETER_MODULE("mod");

		ModuleAdmin::ModuleAdmin()
			: AdminInterfaceElementTemplate<ModuleAdmin>()
		{ }

		void ModuleAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Module class
			try
			{
				setModuleClass(boost::shared_ptr<ModuleClass>(
						Factory<ModuleClass>::create(map.get<string>(PARAMETER_MODULE))
				)	);
			}
			catch(...)
			{
				throw AdminParametersException("Invalid Module Key");
			}
			if(!_moduleClass.get())
			{
				throw AdminParametersException("The module class must be specified");
			}

			// Parameters
			_moduleClass->setAdminFromParametersMap(map);
		}



		util::ParametersMap ModuleAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_moduleClass.get())
			{
				// Parameters
				m.merge(_moduleClass->getAdminParametersMap());

				// Module class
				m.insert(PARAMETER_MODULE, _moduleClass->getFactoryKey());
			}
			return m;
		}



		void ModuleAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{
			_moduleClass->displayAdmin(stream, request, *this);
		}



		bool ModuleAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		AdminInterfaceElement::PageLinks ModuleAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			Factory<AdminInterfaceElement>::ObjectsCollection pages(Factory<AdminInterfaceElement>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<AdminInterfaceElement>& page, pages)
			{
				page->setEnv(_env);
				PageLinks l(
					page->getSubPagesOfModule(*_moduleClass, currentPage, request)
				);
				BOOST_FOREACH(const boost::shared_ptr<const AdminInterfaceElement>& link, l)
				{
					links.push_back(link);
				}
			}
			return links;
		}



		std::string ModuleAdmin::getTitle() const
		{
			return _moduleClass.get() ? _moduleClass->getName() : DEFAULT_TITLE;
		}



		bool ModuleAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			return true;
		}



		bool ModuleAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _moduleClass->getFactoryKey() == static_cast<const ModuleAdmin&>(other)._moduleClass->getFactoryKey();
		}
}	}
