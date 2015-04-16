
/** LogoutAction class implementation.
	@file LogoutAction.cpp

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

#include "LogoutAction.h"

#include "ParametersMap.h"
#include "Request.h"
#include "Session.h"

namespace synthese
{
	using namespace util;

	template<> const std::string util::FactorableTemplate<server::Action,server::LogoutAction>::FACTORY_KEY("logout");

	namespace server
	{
		ParametersMap LogoutAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void LogoutAction::_setFromParametersMap(const ParametersMap& map )
		{
		}



		void LogoutAction::run(Request& request)
		{
			if(request.getSession())
			{
				request.getSession()->removeSessionIdCookie(request);
				request.deleteSession();
			}
		}



		bool LogoutAction::isAuthorized(
			const Session*
		) const {
			return true;
		}
}	}
