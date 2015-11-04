
/** TimetableRowAddAction class implementation.
	@file TimetableRowAddAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableRowAddAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "RoadModule.h"
#include "Session.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableTableSync.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableRowAddAction>::FACTORY_KEY("TimetableRowAddAction");
	}

	namespace timetables
	{
		const string TimetableRowAddAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";
		const string TimetableRowAddAction::PARAMETER_IS_COMPULSORY = Action_PARAMETER_PREFIX + "ic";
		const string TimetableRowAddAction::PARAMETER_IS_SUFFICIENT = Action_PARAMETER_PREFIX + "is";
		const string TimetableRowAddAction::PARAMETER_IS_DISPLAYED = Action_PARAMETER_PREFIX + "ii";
		const string TimetableRowAddAction::PARAMETER_IS_ARRIVAL = Action_PARAMETER_PREFIX + "ia";
		const string TimetableRowAddAction::PARAMETER_IS_DEPARTURE = Action_PARAMETER_PREFIX + "id";
		const string TimetableRowAddAction::PARAMETER_PLACE_NAME = Action_PARAMETER_PREFIX + "pn";
		const string TimetableRowAddAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";
		const string TimetableRowAddAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "ti";


		TimetableRowAddAction::TimetableRowAddAction():
			util::FactorableTemplate<Action, TimetableRowAddAction>(),
			_place(NULL)
		{
		}



		ParametersMap TimetableRowAddAction::getParametersMap() const
		{
			ParametersMap map;
			if (_timetable.get())
				map.insert(PARAMETER_TIMETABLE_ID, _timetable->getKey());
			return map;
		}



		void TimetableRowAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_timetable = TimetableTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID), *_env);
			}
			catch (...)
			{
				throw ActionException("No such timetable");
			}

			// Rank
			optional<size_t> existingRank(
				TimetableRowTableSync::GetMaxRank(_timetable->getKey())
			);
			_rank = map.getDefault<size_t>(
				PARAMETER_RANK,
				existingRank ? *existingRank + 1 : 0
			);

			_isArrival = map.getDefault<bool>(PARAMETER_IS_ARRIVAL, false);
			_isDeparture = map.getDefault<bool>(PARAMETER_IS_DEPARTURE, false);
			_isCompulsory = map.getDefault<bool>(PARAMETER_IS_COMPULSORY, false);
			_isSufficient = map.getDefault<bool>(PARAMETER_IS_SUFFICIENT, false);
			_isDisplayed = map.getDefault<bool>(PARAMETER_IS_DISPLAYED, false);

			_place = dynamic_cast<const StopArea*>(RoadModule::FetchPlace(
				map.get<string>(PARAMETER_CITY_NAME),
				map.get<string>(PARAMETER_PLACE_NAME)
			).get());

			if (_place == NULL)
				throw ActionException("No such place");
		}



		void TimetableRowAddAction::run(Request& request)
		{
			TimetableRow r;
			r.setTimetableId(_timetable->getKey());
			r.setRank(_rank);
			r.setPlace(_place);
			r.setIsDeparture(_isDeparture);
			r.setIsArrival(_isArrival);
			r.setCompulsory(_isSufficient ? PassageSuffisant : PassageFacultatif);

			// rank shifting
			TimetableRowTableSync::Shift(_timetable->getKey(), _rank, 1);

			TimetableRowTableSync::Save(&r);
		}



		void TimetableRowAddAction::setTimetable( boost::shared_ptr<const Timetable> timetable )
		{
			_timetable = timetable;
		}


		bool TimetableRowAddAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}

	}
}
