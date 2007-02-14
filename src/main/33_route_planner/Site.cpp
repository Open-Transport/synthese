
/** Site class implementation.
	@file Site.cpp

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

#include "04_time/DateTime.h"

#include "33_route_planner/Site.h"

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;
	using namespace interfaces;

	namespace routeplanner
	{
		Site::Site( const uid& id)
			: Registrable<uid, Site>(id)
		{
			
		}

		const Interface* Site::getInterface() const
		{
			return _interface;
		}

		const synthese::env::Environment* Site::getEnvironment() const
		{
			return _env;
		}

		bool Site::onlineBookingAllowed() const
		{
			return _onlineBookingAllowed;
		}

		bool Site::dateControl() const
		{
			DateTime tempDate;
			return tempDate.getDate() >= _startValidityDate && tempDate.getDate() <= _endValidityDate;
		}

		void Site::setEnvironment( synthese::env::Environment* environment )
		{
			_env = environment;
		}

		void Site::setInterface( const synthese::interfaces::Interface* interf )
		{
			_interface = interf;
		}

		void Site::setStartDate( const synthese::time::Date& dateDebut )
		{
			_startValidityDate = dateDebut;
		}

		void Site::setEndDate( const synthese::time::Date& dateFin )
		{
			_endValidityDate = dateFin;
		}

		void Site::setOnlineBookingAllowed( const bool valeur )
		{
			_onlineBookingAllowed = valeur;
		}

		void Site::setClientURL( const std::string& clientURL )
		{
			_clientURL = clientURL;
		}

		void Site::setPastSolutionsDisplayed( bool pastSolutions)
		{
			_pastSolutionsDisplayed = pastSolutions;
		}

		void Site::setName( const std::string& name )
		{
			_name = name;
		}

		const std::string& Site::getClientURL() const
		{
			return _clientURL;
		}
	}
}

/* To put in request parsing

			it = map.find(PARAMETER_SITE);
			if (it == map.end())
				throw RequestException("Site not specified");
			try
			{
				request->_site = ServerModule::getSites().get(Conversion::ToLongLong(it->second));
			}
			catch (Site::RegistryKeyException e)
			{
				throw RequestException("Site not found");
			}
			// Site validity control
			if (!request->_site->dateControl())
				throw RequestException("Site is deactivated");
			map.erase(it);
*/
