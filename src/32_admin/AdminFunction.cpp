
/** AdminRequest class implementation.
	@file AdminRequest.cpp

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

#include <sstream>
#include <assert.h>

#include "AdminFunction.h"
#include "Conversion.h"
#include "FactoryException.h"
#include "HTMLForm.h"
#include "Interface.h"
#include "InterfacePage.h"
#include "RequestException.h"
#include "Action.h"
#include "HomeAdmin.h"
#include "AdminInterfacePage.h"
#include "AdminInterfaceElement.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace html;
	using namespace server;
	using namespace interfaces;
	using namespace security;

	template<> const string util::FactorableTemplate<RequestWithInterface,admin::AdminFunction>::FACTORY_KEY("admin");

	namespace admin
	{
		const string AdminFunction::PARAMETER_PAGE("rub");
		const string AdminFunction::PARAMETER_TAB("tab");
		const string AdminFunction::PARAMETER_ACTION_FAILED_PAGE("afp");
		const string AdminFunction::PARAMETER_ACTION_FAILED_TAB("aft");
		
		ParametersMap AdminFunction::_getParametersMap() const
		{
			ParametersMap result(RequestWithInterface::_getParametersMap());

			if (_page.get())
			{
				result.insert(PARAMETER_PAGE, _page->getFactoryKey());
				result.insert(PARAMETER_TAB, _page->getActiveTab());
				const map<string,string> adminMap(_page->getParametersMap().getMap());
				for (map<string,string>::const_iterator it(adminMap.begin()); it != adminMap.end(); ++it)
				{
					result.insert(it->first,it->second);
				}
			}
			if (_actionFailedPage.get())
			{
				result.insert(PARAMETER_ACTION_FAILED_PAGE, _actionFailedPage->getFactoryKey());
				result.insert(PARAMETER_ACTION_FAILED_TAB, _actionFailedPage->getActiveTab());
			}
			return result;
		}

		void AdminFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterface::_setFromParametersMap(map);

			try
			{
				// Page
				string pageKey;
				if (map.getDefault<bool>(Request::PARAMETER_ACTION_FAILED, false))
				{	// Prepare the KO page
					pageKey = map.getDefault<string>(PARAMETER_ACTION_FAILED_PAGE);
					if (pageKey.empty())
						pageKey = map.getDefault<string>(PARAMETER_PAGE);
				}
				else
				{	// Prepare the OK page

					// Saving of the action failed page for url output purposes
					pageKey = map.getDefault<string>(PARAMETER_ACTION_FAILED_PAGE);
					if (!pageKey.empty())
					{
						_actionFailedPage.reset(Factory<AdminInterfaceElement>::create(pageKey));
						_actionFailedPage->setActiveTab(
							map.getDefault<string>(PARAMETER_ACTION_FAILED_TAB)
						);
					}

					pageKey = map.getDefault<string>(PARAMETER_PAGE);
				}
				_page.reset(pageKey.empty()
					? new HomeAdmin
					: Factory<AdminInterfaceElement>::create(pageKey)
				);
				_page->setEnv(shared_ptr<Env>(new Env));
				_errorMessage = map.getOptional<string>(Request::PARAMETER_ERROR_MESSAGE);
				_activeTab = map.getDefault<string>(PARAMETER_TAB);
				_page->setFromParametersMap(map);
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Admin page not found");
			}
			catch (AdminParametersException e)
			{
				throw RequestException("Admin page parameters error : " + e.getMessage());
			}
		}

		void AdminFunction::run( std::ostream& stream, const Request& request ) const
		{
			try
			{
				if(request.getSession())
				{
					_page->_buildTabs(*request.getSession()->getUser()->getProfile());
					_page->setActiveTab(_activeTab);
					if (_interface && _interface->getPage<AdminInterfacePage>())
					{
						_interface->getPage<AdminInterfacePage>()->display(
							stream,
							_page.get(),
							_errorMessage,
							StaticFunctionRequest<AdminFunction>(request, false)
						);
					}
					else
					{
						VariablesMap variables;
						_page->display(
							stream,
							variables,
							StaticFunctionRequest<AdminFunction>(request, false)
						);
				}	} else if(
					_interface &&
					_interface->getPage<AdminInterfacePage>()
				){
					_interface->getPage<AdminInterfacePage>()->display(
						stream,
						NULL,
						_errorMessage,
						StaticFunctionRequest<AdminFunction>(request, false)
					);
			}	}
			catch (Exception e)
			{
				throw RequestException("Admin interface page not implemented in database"+ e.getMessage());
			}
		}

		void AdminFunction::setPage(shared_ptr<AdminInterfaceElement> aie )
		{
			_page = aie;
		}

		
		shared_ptr<AdminInterfaceElement> AdminFunction::getPage() const
		{
			return _page;
		}

		void AdminFunction::setActionFailedPage(shared_ptr<AdminInterfaceElement> aie )
		{
			_actionFailedPage = aie;
		}

		bool AdminFunction::isAuthorized(const server::Session* session) const
		{
			return
				_page.get() &&
				(	!session ||
					session->hasProfile() &&
					_page->isAuthorized(
						*session->getUser()
				)	)
			;
		}



		std::string AdminFunction::getOutputMimeType() const
		{
			return "text/html";
		}

		void AdminFunction::_copy( boost::shared_ptr<const Function> function )
		{
			RequestWithInterface::_copy(function);
			_page = static_pointer_cast<const AdminFunction>(function)->_page;
		}
	}
}
