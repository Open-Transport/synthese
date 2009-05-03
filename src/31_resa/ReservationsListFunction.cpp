
/** ReservationsListFunction class implementation.
	@file ReservationsListFunction.cpp
	@author Hugues Romain
	@date 2007

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

#include "ReservationsListFunction.h"

#include "36_places_list/Site.h"
#include "ResaRight.h"
#include "31_resa/ReservationsListInterfacePage.h"

#include "30_server/RequestException.h"
#include "30_server/RequestMissingParameterException.h"
#include "30_server/LoginInterfacePage.h"
#include "30_server/Request.h"
#include "Conversion.h"
#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "11_interfaces/Interface.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace server;
	using namespace time;
	using namespace env;
	using namespace security;
	using namespace db;

	template<> const string util::FactorableTemplate<transportwebsite::FunctionWithSite,resa::ReservationsListFunction>::FACTORY_KEY("reservations_list");

	namespace resa
	{
		const string ReservationsListFunction::PARAMETER_DATE("da");
		const string ReservationsListFunction::PARAMETER_LINE_ID("li");
		const string ReservationsListFunction::PARAMETER_USER_ID("ui");
		const string ReservationsListFunction::PARAMETER_USER_NAME("un");
		const string ReservationsListFunction::PARAMETER_DISPLAY_CANCELLED("dc");

		ParametersMap ReservationsListFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			map.insert(PARAMETER_DATE, _startDateTime);
			if (_line.get())
				map.insert(PARAMETER_LINE_ID, _line->getKey());
			if (_user.get())
				map.insert(PARAMETER_USER_ID, _user->getKey());
			map.insert(PARAMETER_USER_NAME, _userName);
			map.insert(PARAMETER_DISPLAY_CANCELLED, _displayCancelled);

			return map;
		}

		void ReservationsListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Site
			FunctionWithSite::_setFromParametersMap(map);
			if(!_site->getInterface())
			{
				throw RequestException("Site "+ Conversion::ToString(_site->getKey()) + " is corrupted : it has no interface");
			}

			// Date
			try
			{
				DateTime dateTime(map.getDateTime(PARAMETER_DATE, false, getFactoryKey()));
				if (!dateTime.isUnknown())
				{
					_startDateTime = dateTime;
					_endDateTime = DateTime(dateTime.getDate(), Hour(TIME_MAX));
				}
			}
			catch (...)
			{
				throw RequestException("Bad value for date");
			}

			// Line
			uid lineId(map.getUid(PARAMETER_LINE_ID, false, getFactoryKey()));
			if (lineId != UNKNOWN_VALUE)
			{
				try
				{
					_line = CommercialLineTableSync::Get(lineId, _env);
				}
				catch (...)
				{
					throw RequestException("Bad value for line ID");
				}
			}

			// Customer
			uid customerId(map.getUid(PARAMETER_USER_ID, false, getFactoryKey()));
			if (customerId != UNKNOWN_VALUE)
			{
				try
				{
					_user = UserTableSync::Get(customerId, _env);
				}
				catch (...)
				{
					throw RequestException("Bad value for customer ID");
				}
			}

			// Customer name
			_userName = map.getString(PARAMETER_USER_NAME, false, getFactoryKey());

			// Test if at least one parameter is defined
			if (!_line.get() && !_user.get() && _userName.empty() && _request->getUser().get())
				_user = _request->getUser();
			
			// Display cancelled
			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, getFactoryKey());
		}

		void ReservationsListFunction::_run( std::ostream& stream ) const
		{
			if (_request->getSession())
			{
				const ReservationsListInterfacePage* page(_site->getInterface()->getPage<ReservationsListInterfacePage>());
				VariablesMap vm;

				page->display(
					stream
					, _line
					, _user
					, _userName
					, _startDateTime.getDate()
					, _displayCancelled
					, vm
					, _request
				);
			}
			else
			{
				const LoginInterfacePage* page(_site->getInterface()->getPage<LoginInterfacePage>());
				VariablesMap vm;

				page->display(
					stream
					, this
					, "Réservations"
					, vm
					, _request
				);
			}
		}

		ReservationsListFunction::ReservationsListFunction()
			: _startDateTime(TIME_CURRENT)
			, _endDateTime(TIME_CURRENT, TIME_CURRENT, TIME_CURRENT, TIME_MAX, TIME_MAX)
		{
		}

		void ReservationsListFunction::setLine(shared_ptr<const CommercialLine> line )
		{
			_line = line;
		}



		bool ReservationsListFunction::_isAuthorized(
		) const {
			if (_line.get() != NULL)
			{
				return _request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_line->getKey()));
			}
			return false;
		}

		std::string ReservationsListFunction::getOutputMimeType() const
		{
			return (_site.get() && _site->getInterface() && _site->getInterface()->getPage<LoginInterfacePage>()) ?
				_site->getInterface()->getPage<LoginInterfacePage>()->getMimeType():
				"text/plain";
		}
	}
}
