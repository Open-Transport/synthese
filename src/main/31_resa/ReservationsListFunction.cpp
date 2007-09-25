
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

#include "31_resa/ReservationsListInterfacePage.h"

#include "30_server/RequestException.h"
#include "30_server/RequestMissingParameterException.h"
#include "30_server/LoginInterfacePage.h"
#include "30_server/Request.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"

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
			map.insert(make_pair(PARAMETER_DATE, _startDateTime.toInternalString()));
			if (_line.get())
				map.insert(make_pair(PARAMETER_LINE_ID, Conversion::ToString(_line->getKey())));
			if (_user.get())
				map.insert(make_pair(PARAMETER_USER_ID, Conversion::ToString(_user->getKey())));
			map.insert(make_pair(PARAMETER_USER_NAME, _userName));
			map.insert(make_pair(PARAMETER_DISPLAY_CANCELLED, Conversion::ToString(_displayCancelled)));

			return map;
		}

		void ReservationsListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Site
			FunctionWithSite::_setFromParametersMap(map);

			// Date
			try
			{
				DateTime dateTime(Request::getDateTimeFromParameterMap(map, PARAMETER_DATE, false, getFactoryKey()));
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
			uid lineId(Request::getUidFromParameterMap(map, PARAMETER_LINE_ID, false, getFactoryKey()));
			if (lineId != UNKNOWN_VALUE)
			{
				try
				{
					_line = CommercialLineTableSync::get(lineId);
				}
				catch (...)
				{
					throw RequestException("Bad value for line ID");
				}
			}

			// Customer
			uid customerId(Request::getUidFromParameterMap(map, PARAMETER_USER_ID, false, getFactoryKey()));
			if (customerId != UNKNOWN_VALUE)
			{
				try
				{
					_user = UserTableSync::get(customerId);
				}
				catch (...)
				{
					throw RequestException("Bad value for customer ID");
				}
			}

			// Customer name
			_userName = Request::getStringFormParameterMap(map, PARAMETER_USER_NAME, false, getFactoryKey());

			// Test if at least one parameter is defined
			if (!_line.get() && !_user.get() && _userName.empty() && _request->getUser().get())
				_user = _request->getUser();
			
			// Display cancelled
			_displayCancelled = Request::getBoolFromParameterMap(map, PARAMETER_DISPLAY_CANCELLED, false, false, getFactoryKey());
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
	}
}
