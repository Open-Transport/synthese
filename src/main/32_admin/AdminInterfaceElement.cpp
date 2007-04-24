
/** AdminInterfaceElement class implementation.
	@file AdminInterfaceElement.cpp

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

#include "05_html/HTMLModule.h"

#include "30_server/FunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminInterfaceElement.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace html;

	namespace admin
	{
		AdminInterfaceElement::AdminInterfaceElement(const std::string& superior, AdminInterfaceElement::DisplayMode everDisplayed)
			: _superior(superior)
			, _everDisplayed(everDisplayed)
		{		}

		const AdminInterfaceElement::DisplayMode AdminInterfaceElement::getDisplayMode() const
		{
			return _everDisplayed;
		}

		const std::string& AdminInterfaceElement::getSuperior() const
		{
			return _superior;
		}

/*		std::string AdminInterfaceElement::getHTMLLink(const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			stringstream str;
			FunctionRequest<AdminRequest> linkRequest(request);
			linkRequest.getFunction()->setPage(this);
			str << HTMLModule::getHTMLLink(linkRequest.getURL(), getTitle());
			linkRequest.getFunction()->setPage(NULL); // To avoid the deletion of the page which can be used later
			return str.str();
		}
*/
		void AdminInterfaceElement::_setSuperior( const std::string& superior )
		{
			_superior = superior;
		}

		std::string AdminInterfaceElement::getIcon() const
		{
			return "application_form.png";
		}
	}
}
