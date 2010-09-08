
/** TimetableRowDeleteAction class implementation.
	@file TimetableRowDeleteAction.cpp
	@author Hugues
	@date 2009

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "TimetableRowDeleteAction.h"
#include "TimetableRight.h"
#include "Request.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableRowDeleteAction>::FACTORY_KEY("TimetableRowDeleteAction");
	}

	namespace timetables
	{
		const string TimetableRowDeleteAction::PARAMETER_ROW_ID = Action_PARAMETER_PREFIX + "ri";
		
		
		
		ParametersMap TimetableRowDeleteAction::getParametersMap() const
		{
			ParametersMap map;
			if(_element.get()) map.insert(PARAMETER_ROW_ID, _element->getKey());
			return map;
		}
		
		
		
		void TimetableRowDeleteAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_element = TimetableRowTableSync::Get(map.get<RegistryKeyType>(PARAMETER_ROW_ID), *_env);
			}
			catch(ObjectNotFoundException<TimetableRow>& e)
			{
				throw ActionException("No such calendar template element", e, *this);
			}

		}
		
		
		
		void TimetableRowDeleteAction::run(Request& request)
		{
			TimetableRowTableSync::Shift(_element->getTimetableId(), _element->getRank(), -1);

			TimetableRowTableSync::Remove(_element->getKey());
		}
		
		
		
		bool TimetableRowDeleteAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}



		void TimetableRowDeleteAction::setElement( boost::shared_ptr<const TimetableRow> value )
		{
			_element = value;
		}
	}
}
