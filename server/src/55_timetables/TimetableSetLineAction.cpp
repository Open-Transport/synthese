
/** TimetableSetLineAction class implementation.
	@file TimetableSetLineAction.cpp
	@author Hugues
	@date 2009

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

#include "TimetableSetLineAction.h"

#include "ActionException.h"
#include "CommercialLine.h"
#include "Env.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableTableSync.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableSetLineAction>::FACTORY_KEY("TimetableSetLineAction");
	}

	namespace timetables
	{
		const string TimetableSetLineAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "ti";
		const string TimetableSetLineAction::PARAMETER_LINE_ID = Action_PARAMETER_PREFIX + "li";



		ParametersMap TimetableSetLineAction::getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get()) map.insert(PARAMETER_TIMETABLE_ID, _timetable->getKey());
			if(_line.get()) map.insert(PARAMETER_LINE_ID, _line->getKey());
			return map;
		}



		void TimetableSetLineAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_timetable = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Timetable>& e)
			{
				throw ActionException("No such timetable", e, *this);
			}

			try
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
			}
			catch(ObjectNotFoundException<CommercialLine>& e)
			{
				throw ActionException("No such line", e, *this);
			}
		}



		void TimetableSetLineAction::run(Request& request)
		{
			if(_timetable->getAuthorizedLines().find(_line.get()) == _timetable->getAuthorizedLines().end())
			{
				_timetable->addAuthorizedLine(_line.get());
			}
			else
			{
				_timetable->removeAuthorizedLine(_line.get());
			}
			TimetableTableSync::Save(_timetable.get());
		}



		bool TimetableSetLineAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}



		void TimetableSetLineAction::setTimetable( boost::shared_ptr<Timetable> value )
		{
			_timetable = value;
		}



		void TimetableSetLineAction::setLine( boost::shared_ptr<const pt::CommercialLine> value )
		{
			_line = value;
		}
	}
}
