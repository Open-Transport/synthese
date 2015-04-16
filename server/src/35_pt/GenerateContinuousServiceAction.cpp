
//////////////////////////////////////////////////////////////////////////
/// GenerateContinuousServiceAction class implementation.
/// @file GenerateContinuousServiceAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "GenerateContinuousServiceAction.hpp"
#include "Request.h"
#include "CommercialLineTableSync.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::GenerateContinuousServiceAction>::FACTORY_KEY("GenerateContinuousServiceAction");
	}

	namespace pt
	{
		const string GenerateContinuousServiceAction::PARAMETER_LINE = Action_PARAMETER_PREFIX + "_line";
		const string GenerateContinuousServiceAction::PARAMETER_MIN_NUMBER = Action_PARAMETER_PREFIX + "_min_number";
		const string GenerateContinuousServiceAction::PARAMETER_MIN_WAITING_DIFFERENCE = Action_PARAMETER_PREFIX + "_min_waiting_difference";
		const string GenerateContinuousServiceAction::PARAMETER_WAITING_TIME = Action_PARAMETER_PREFIX + "_waiting_time";



		ParametersMap GenerateContinuousServiceAction::getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(PARAMETER_LINE, _line->getKey());
			}
			return map;
		}



		void GenerateContinuousServiceAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_line = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("No such line");
			}

			_waitingTime = minutes(map.get<int>(PARAMETER_WAITING_TIME));
			_minNumber = map.get<size_t>(PARAMETER_MIN_NUMBER);
		}



		void GenerateContinuousServiceAction::run(
			Request& request
		){
/*			UPDATE EXAMPLE
			stringstream text;
			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			_object->setAttribute(_value);
			ObjectTableSync::Save(_object.get());
			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
*/

/*			CREATION EXAMPLE
			Object object;
			object.setAttribute(_value);
			ObjectTableSync::Save(&object);
			::AddCreationEntry(object, *request.getUser());
			request.setActionCreatedId(object.getKey());
*/

/*			DELETION EXAMPLE
			ObjectTableSync::Remove(_object->getKey());
			::AddDeleteEntry(*_object, *request.getUser());
*/
		}



		bool GenerateContinuousServiceAction::isAuthorized(
			const Session* session
		) const {
			return true;
			// return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}
}	}
