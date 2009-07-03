
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

#include "AdminRequest.h"

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

	template<> const string util::FactorableTemplate<RequestWithInterfaceAndRequiredSession,admin::AdminRequest>::FACTORY_KEY("admin");

	namespace admin
	{
		const string AdminRequest::PARAMETER_PAGE("rub");
		const string AdminRequest::PARAMETER_TAB("tab");
		const string AdminRequest::PARAMETER_ACTION_FAILED_PAGE("afp");
		const string AdminRequest::PARAMETER_ACTION_FAILED_TAB("aft");
		
		ParametersMap AdminRequest::_getParametersMap() const
		{
			ParametersMap result(RequestWithInterfaceAndRequiredSession::_getParametersMap());

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

		void AdminRequest::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterfaceAndRequiredSession::_setFromParametersMap(map);

			try
			{
				if (_request->getUser().get() == NULL) return;

				// Page
				string pageKey;
				if (_request->getActionException())
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
				shared_ptr<AdminInterfaceElement> page(pageKey.empty()
					? new HomeAdmin
					: Factory<AdminInterfaceElement>::create(pageKey)
				);
				page->setEnv(shared_ptr<Env>(new Env));
				page->setFromParametersMap(map, true, _request->getActionWillCreateObject());
				if(!_request->getActionWillCreateObject())
				{
					page->_buildTabs(
						static_cast<const FunctionRequest<AdminRequest>* >(_request)
					);
				}
				page->setActiveTab(map.getDefault<string>(PARAMETER_TAB));
				_page = page;
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

		void AdminRequest::_run( std::ostream& stream ) const
		{
			try
			{
				if (_interface != NULL)
				{
					const AdminInterfacePage* const aip(
						_interface->getPage<AdminInterfacePage>()
					);
					aip->display(
						stream,
						&const_pointer_cast<const AdminInterfaceElement>(_page),
						static_cast<const FunctionRequest<AdminRequest>* >(_request)
					);
				}
				else
				{
					VariablesMap variables;
					_page->display(
						stream,
						variables,
						static_cast<FunctionRequest<AdminRequest>& >(*_request)
					);
				}
			}
			catch (Exception e)
			{
				throw RequestException("Admin interface page not implemented in database"+ e.getMessage());
			}
		}

		void AdminRequest::setPage(shared_ptr<AdminInterfaceElement> aie )
		{
			_page = aie;
		}

		
		shared_ptr<AdminInterfaceElement> AdminRequest::getPage() const
		{
			return _page;
		}

		void AdminRequest::setActionFailedPage(shared_ptr<AdminInterfaceElement> aie )
		{
			_actionFailedPage = aie;
		}

		bool AdminRequest::_isAuthorized() const
		{
			return _page->isAuthorized(static_cast<FunctionRequest<AdminRequest>& >(*_request));
		}



		std::string AdminRequest::getOutputMimeType() const
		{
			return "text/html";
		}

		void AdminRequest::_copy( boost::shared_ptr<const Function> function )
		{
			RequestWithInterfaceAndRequiredSession::_copy(function);
			_page = static_pointer_cast<const AdminRequest>(function)->_page;
		}
	}
}
