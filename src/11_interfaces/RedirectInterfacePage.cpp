
/** RedirectInterfacePage class implementation.
	@file RedirectInterfacePage.cpp

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

#include "30_server/Request.h"

#include "11_interfaces/RedirectInterfacePage.h"

namespace synthese
{
	using namespace server;

	template<> const std::string util::FactorableTemplate<interfaces::InterfacePage,interfaces::RedirectInterfacePage>::FACTORY_KEY("redir");

	namespace interfaces
	{
		void RedirectInterfacePage::display( std::ostream& stream, VariablesMap& vars, const std::string& url, const server::Request* request /*= NULL*/ ) const
		{
			ParametersVector pv;
			pv.push_back(url);

			InterfacePage::display(stream, pv, vars, NULL, request);
		}

		void RedirectInterfacePage::display( std::ostream& stream, VariablesMap& vars, const server::Request* request /*= NULL*/ ) const
		{
			display(stream, vars, request->getURL(), request);
		}
	}
}
