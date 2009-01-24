
/** TimetableRowAddAction class implementation.
	@file TimetableRowAddAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "TimetableRowAddAction.h"

#include "35_timetables/Timetable.h"
#include "35_timetables/TimetableTableSync.h"
#include "35_timetables/TimetableRow.h"
#include "35_timetables/TimetableRowTableSync.h"

#include "15_env/EnvModule.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace env;
	
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

		
		TimetableRowAddAction::TimetableRowAddAction()
			: util::FactorableTemplate<Action, TimetableRowAddAction>()
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
			uid id(map.getUid(PARAMETER_TIMETABLE_ID, true, FACTORY_KEY));
			try
			{
				_timetable = TimetableTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("No such timetable");
			}

			_rank = map.getInt(PARAMETER_RANK, false, FACTORY_KEY);
			if (_rank < 0)
				_rank = TimetableRowTableSync::GetMaxRank(_timetable->getKey())+1;

			_isArrival = map.getBool(PARAMETER_IS_ARRIVAL, false, true, FACTORY_KEY);
			_isDeparture = map.getBool(PARAMETER_IS_DEPARTURE, false, true, FACTORY_KEY);
			_isCompulsory = map.getBool(PARAMETER_IS_COMPULSORY, false, false, FACTORY_KEY);
			_isSufficient = map.getBool(PARAMETER_IS_SUFFICIENT, false, true, FACTORY_KEY);
			_isDisplayed = map.getBool(PARAMETER_IS_DISPLAYED, false, true, FACTORY_KEY);

			_place = dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(EnvModule::FetchPlace(
				map.getString(PARAMETER_CITY_NAME, true, FACTORY_KEY)
				, map.getString(PARAMETER_PLACE_NAME, true, FACTORY_KEY)
			));

			if (_place == NULL)
				throw ActionException("No such place");
		}
		
		
		
		void TimetableRowAddAction::run()
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

			TimetableRowTableSync::save(&r);
		}



		void TimetableRowAddAction::setTimetable( boost::shared_ptr<const Timetable> timetable )
		{
			_timetable = timetable;
		}
	}
}
