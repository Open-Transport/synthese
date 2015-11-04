
//////////////////////////////////////////////////////////////////////////
/// TimetableTransferUpdateAction class implementation.
/// @file TimetableTransferUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "TimetableTransferUpdateAction.hpp"

#include "ActionException.h"
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

	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableTransferUpdateAction>::FACTORY_KEY("TimetableTransferUpdateAction");
	}

	namespace timetables
	{
		const string TimetableTransferUpdateAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "id";
		const string TimetableTransferUpdateAction::PARAMETER_TRANSFER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "tt";
		const string TimetableTransferUpdateAction::PARAMETER_BEFORE = Action_PARAMETER_PREFIX + "be";


		TimetableTransferUpdateAction::TimetableTransferUpdateAction():
		_before(true)
		{

		}



		ParametersMap TimetableTransferUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_BEFORE, _before);
			if(_timetable.get())
			{
				map.insert(PARAMETER_TIMETABLE_ID, _timetable->getKey());
			}
			if(_transferTimetable.get())
			{
				map.insert(PARAMETER_TRANSFER_TIMETABLE_ID, _transferTimetable->getKey());
			}
			return map;
		}



		void TimetableTransferUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			_before = map.get<bool>(PARAMETER_BEFORE);

			try
			{
				_timetable = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Timetable>&)
			{
				throw ActionException("No such timetable");
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_TRANSFER_TIMETABLE_ID, 0))
			{
				try
				{
					_transferTimetable = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TRANSFER_TIMETABLE_ID), *_env);
				}
				catch(ObjectNotFoundException<Timetable>&)
				{
					throw ActionException("No such transfer timetable");
				}
			}
		}



		void TimetableTransferUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_before)
			{
				if(_transferTimetable.get())
				{
					_timetable->set<TransferTimetableBefore>(*_transferTimetable);
				}
				else
				{
					_timetable->set<TransferTimetableBefore>(TransferTimetableBefore::Type());
				}
			}
			else
			{
				if(_transferTimetable.get())
				{
					_timetable->set<TransferTimetableAfter>(*_transferTimetable);
				}
				else
				{
					_timetable->set<TransferTimetableAfter>(TransferTimetableAfter::Type());
				}
			}

			TimetableTableSync::Save(_timetable.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool TimetableTransferUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}
}	}
