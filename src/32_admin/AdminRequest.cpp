
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

#include "01_util/Conversion.h"
#include "01_util/FactoryException.h"

#include "05_html/HTMLForm.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePage.h"

#include "12_security/GlobalRight.h"

#include "30_server/RequestException.h"
#include "30_server/Action.h"

#include "32_admin/HomeAdmin.h"
#include "32_admin/AdminInterfacePage.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminRequest.h"
#include "32_admin/AdminParametersException.h"

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
		const std::string AdminRequest::PARAMETER_PAGE = "rub";
		const std::string AdminRequest::PARAMETER_ACTION_FAILED_PAGE = "afp";
		
		ParametersMap AdminRequest::_getParametersMap() const
		{
			ParametersMap result(RequestWithInterfaceAndRequiredSession::_getParametersMap());

			result.insert(PARAMETER_PAGE, _page->getFactoryKey());
			if (_actionFailedPage.get())
				result.insert(PARAMETER_ACTION_FAILED_PAGE, _actionFailedPage->getFactoryKey());
			const map<string,string> adminMap(_page->getParametersMap().getMap());
			for (map<string,string>::const_iterator it(adminMap.begin()); it != adminMap.end(); ++it)
				result.insert(it->first,it->second);
			return result;
		}

		void AdminRequest::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterfaceAndRequiredSession::_setFromParametersMap(map);

			try
			{
				// Page
				string pageKey;
				if (_request->getActionException())
				{	// Prepare the KO page
					pageKey = map.getString(PARAMETER_ACTION_FAILED_PAGE, false, FACTORY_KEY);
					if (pageKey.empty())
						pageKey = map.getString(PARAMETER_PAGE, false, FACTORY_KEY);
				}
				else
				{	// Prepare the OK page

					// Saving of the action failed page for url output purposes
					pageKey = map.getString(PARAMETER_ACTION_FAILED_PAGE, false, FACTORY_KEY);
					if (!pageKey.empty())
						_actionFailedPage.reset(Factory<AdminInterfaceElement>::create(pageKey));

					pageKey = map.getString(PARAMETER_PAGE, false, FACTORY_KEY);
				}
				shared_ptr<AdminInterfaceElement> page(pageKey.empty()
					? new HomeAdmin
					: Factory<AdminInterfaceElement>::create(pageKey)
				);
				page->setFromParametersMap(map);
				_page = page;
				
				// Parameters saving
				_parameters = map;
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
					const AdminInterfacePage* const aip = _interface->getPage<AdminInterfacePage>();
					aip->display(stream, &const_pointer_cast<const AdminInterfaceElement>(_page), _request->getObjectId(), (const FunctionRequest<AdminRequest>*) _request);
				}
				else
				{
					VariablesMap variables;
					_page->display(stream, variables, (const FunctionRequest<AdminRequest>*) _request);
				}
			}
			catch (Exception e)
			{
				throw RequestException("Admin interface page not implemented in database");
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

		void AdminRequest::setParameter( const std::string& name, const std::string value )
		{
			_parameters.insert(name,value);
		}

		void AdminRequest::setActionFailedPage(shared_ptr<AdminInterfaceElement> aie )
		{
			_actionFailedPage = aie;
		}

		bool AdminRequest::_isAuthorized() const
		{
			return 
				_request->isAuthorized<GlobalRight>(USE, UNKNOWN_RIGHT_LEVEL)
				&& _page->isAuthorized(static_cast<const FunctionRequest<AdminRequest>* >(_request));
		}
	}
}
