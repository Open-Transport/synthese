
/** BaseAdminFunctionRequest class implementation.
	@file BaseAdminFunctionRequest.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "BaseAdminFunctionRequest.hpp"

#include "AdminPageDisplayService.hpp"

namespace synthese
{
	using namespace util;

	namespace admin
	{
		BaseAdminFunctionRequest::BaseAdminFunctionRequest(
			const server::Request& request,
			boost::shared_ptr<AdminInterfaceElement> page
		):	server::Request(request),
			server::StaticFunctionRequest<cms::WebPageDisplayFunction>(request, true),
			_page(page),
			_noCMS(request.getFunction()->getFactoryKey() == AdminPageDisplayService::FACTORY_KEY)
		{}



		BaseAdminFunctionRequest::BaseAdminFunctionRequest(
			const server::Request& request
		):	server::Request(request),
			server::StaticFunctionRequest<cms::WebPageDisplayFunction>(request, true),
			_noCMS(request.getFunction()->getFactoryKey() == AdminPageDisplayService::FACTORY_KEY)
		{}



		util::ParametersMap BaseAdminFunctionRequest::_getParametersMap() const
		{
			ParametersMap result(Request::_getParametersMap());

			if(_noCMS)
			{
				result.insert(Request::PARAMETER_SERVICE, AdminPageDisplayService::FACTORY_KEY);
				result.insert(AdminPageDisplayService::PARAMETER_NO_CMS, true);
			}

			if(_page.get())
			{
				result.insert(AdminPageDisplayService::PARAMETER_PAGE, _page->getFactoryKey());
				if(_page->getCurrentTab().empty())
				{
					result.insert(AdminPageDisplayService::PARAMETER_TAB, _page->getActiveTab());
				}
				else
				{
					result.insert(AdminPageDisplayService::PARAMETER_TAB, _page->getCurrentTab());
				}

				ParametersMap::Map pm(_page->getParametersMap().getMap());
				for (ParametersMap::Map::const_iterator it = pm.begin(); it != pm.end(); ++it)
				{
					result.insert(it->first, it->second);
				}
			}

			if (_actionFailedPage.get())
			{
				result.insert(AdminPageDisplayService::PARAMETER_ACTION_FAILED_PAGE, _actionFailedPage->getFactoryKey());
				result.insert(AdminPageDisplayService::PARAMETER_ACTION_FAILED_TAB, _actionFailedPage->getActiveTab());
			}

			return result;
		}
}	}

