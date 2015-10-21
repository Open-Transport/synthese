
//////////////////////////////////////////////////////////////////////////
/// StopAreaMergeAction class implementation.
/// @file StopAreaMergeAction.cpp
/// @date 2013
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

#include "StopAreaMergeAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRule.h"
#include "User.h"
#include "Request.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
    using namespace pt;
	using namespace impex;
	using namespace geography;
    using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaMergeAction>::FACTORY_KEY("StopAreaMergeAction");
	}

	namespace pt
	{
		const string StopAreaMergeAction::PARAMETER_PLACE1_ID = Action_PARAMETER_PREFIX + "id1";
		const string StopAreaMergeAction::PARAMETER_PLACE2_ID = Action_PARAMETER_PREFIX + "id2";



		ParametersMap StopAreaMergeAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place1.get())
			{
				map.insert(PARAMETER_PLACE1_ID, _place1->getKey());
			}
			if(_place2.get())
			{
				map.insert(PARAMETER_PLACE2_ID, _place2->getKey());
			}
			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void StopAreaMergeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place1 = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE1_ID), Env::GetOfficialEnv());
				_place2 = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE2_ID), Env::GetOfficialEnv());
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such place");
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void StopAreaMergeAction::run(
			Request& request
		){
			if (_place1 && _place2)
			{
				DBTransaction transaction;

				// Importable
				_doImportableUpdate(*_place1, request);
				// Importable
				_doImportableUpdate(*_place2, request);

				/* Copy all StopPoint objects from _place2 to _place1 */
				if (_place2->getPhysicalStops().size()>0)
				{
					BOOST_FOREACH(const StopArea::PhysicalStops::value_type& st,_place2->getPhysicalStops())
					{
						_place2->removePhysicalStop(*(st.second));
						_place1->addPhysicalStop(*(st.second));
						boost::shared_ptr<StopPoint> stop = StopPointTableSync::GetEditable(st.second->getKey(), Env::GetOfficialEnv());
						stop->setHub(_place1.get());
						StopPointTableSync::Save(stop.get());
					}
					StopAreaTableSync::Save(_place1.get());
				}
				/* Unlink _place2 with its related City if it exists */
				if (_place2->getCity())
				{
					StopAreaTableSync::Unlink(_place2.get());
				}
				/* Delete definitly _place2 from database */
				StopAreaTableSync::Remove(request.getSession().get(), _place2->getKey(), transaction, false);
				transaction.run();
			}
		}



		bool StopAreaMergeAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
