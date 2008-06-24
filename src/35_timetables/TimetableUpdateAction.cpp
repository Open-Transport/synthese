
/** TimetableUpdateAction class implementation.
	@file TimetableUpdateAction.cpp
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

#include "TimetableUpdateAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableUpdateAction>::FACTORY_KEY("TimetableUpdateAction");
	}

	namespace timetables
	{
		/// @todo Parameters constants definition
		// const string TimetableUpdateAction::PARAMETER_xxx = Action_PARAMETER_PREFIX + "xxx";
		
		
		
		TimetableUpdateAction::TimetableUpdateAction()
			: util::FactorableTemplate<Action, TimetableUpdateAction>()
		{
		}
		
		
		
		ParametersMap TimetableUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}
		
		
		
		void TimetableUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// _xxx = map.getString(PARAMETER_xxx, true, FACTORY_KEY);
			// if (_xxx.empty())
			//	throw ActionException("Bad value for xxx parameter ");	
			// 
		}
		
		
		
		void TimetableUpdateAction::run()
		{
		}
	}
}
