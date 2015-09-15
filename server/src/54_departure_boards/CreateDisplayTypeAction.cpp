
/** CreateDisplayTypeAction class implementation.
	@file CreateDisplayTypeAction.cpp

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

#include "CreateDisplayTypeAction.h"

#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "ArrivalDepartureTableLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;


	namespace util
	{
		template<> const string FactorableTemplate<server::Action, departure_boards::CreateDisplayTypeAction>::FACTORY_KEY("createdisplaytype");
	}

	namespace departure_boards
	{
		const string CreateDisplayTypeAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CreateDisplayTypeAction::PARAMETER_ROWS_NUMBER = Action_PARAMETER_PREFIX + "ro";



		ParametersMap CreateDisplayTypeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_ROWS_NUMBER, _rows_number);
			return map;
		}



		void CreateDisplayTypeAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Name
			_name = map.get<string>(PARAMETER_NAME);
			if (_name.empty())
				throw ActionException("Le nom ne peut être vide.");
			Env env;
			DisplayTypeTableSync::Search(env, _name, 0, 1);
			if (!env.getRegistry<DisplayType>().empty())
				throw ActionException("Un type portant le nom spécifié existe déjà. Veuillez utiliser un autre nom.");

			// Rows number
			_rows_number = map.get<int>(PARAMETER_ROWS_NUMBER);
			if (_rows_number < 0)
				throw ActionException("Un nombre positif de lignes doit être choisi");
		}



		void CreateDisplayTypeAction::run(Request& request)
		{
			DisplayType dt;
			dt.set<Name>(_name);
			dt.set<RowsNumber>(_rows_number);
			DisplayTypeTableSync::Save(&dt);

			// Request update
			request.setActionCreatedId(dt.getKey());

			// Log
			ArrivalDepartureTableLog::addCreateEntry(dt, *request.getUser());
		}



		bool CreateDisplayTypeAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
		}
	}
}
