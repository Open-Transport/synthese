
//////////////////////////////////////////////////////////////////////////
/// TimetableRowGroupItemAddAction class implementation.
/// @file TimetableRowGroupItemAddAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "TimetableRowGroupItemAddAction.hpp"

#include "ActionException.h"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "PlacesListService.hpp"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "TimetableRight.h"
#include "TimetableRowGroupTableSync.hpp"
#include "TimetableRowGroupItemTableSync.hpp"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace geography;
	using namespace pt;
	using namespace pt_website;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, timetables::TimetableRowGroupItemAddAction>::FACTORY_KEY = "TimetableRowGroupItemAdd";

	namespace timetables
	{
		const string TimetableRowGroupItemAddAction::PARAMETER_ROWGROUP_ID = Action_PARAMETER_PREFIX + "_rowgroup_id";
		const string TimetableRowGroupItemAddAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "_rank";
		const string TimetableRowGroupItemAddAction::PARAMETER_PLACE_NAME = Action_PARAMETER_PREFIX + "_place_name";
		
		
		
		ParametersMap TimetableRowGroupItemAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_RANK, _rank);
			if(_rowGroup.get())
			{
				map.insert(PARAMETER_ROWGROUP_ID, _rowGroup->getKey());
			}
			return map;
		}
		
		
		
		void TimetableRowGroupItemAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Rank
			_rank = map.get<size_t>(PARAMETER_RANK);

			// Place
			PlacesListService placesListService;
			placesListService.setNumber(1);
			placesListService.setText(map.getDefault<string>(PARAMETER_PLACE_NAME));
			placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());
			placesListService.setClassFilter(PlacesListService::DATA_STOP);
			_stopArea = dynamic_pointer_cast<StopArea, Place>(
				placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				).value
			);

			// Row group
			try
			{
				_rowGroup = TimetableRowGroupTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_ROWGROUP_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<TimetableRowGroup>&)
			{
				throw ActionException("No such row group");
			}
		}
		
		
		
		void TimetableRowGroupItemAddAction::run(
			Request& request
		){
			TimetableRowGroupItem item;
			item.set<StopArea>(*_stopArea);
			item.set<Rank>(_rank);
			item.set<TimetableRowGroup>(*_rowGroup);

			DBTransaction transaction;
			item.beforeCreate(transaction);
			TimetableRowGroupItemTableSync::Save(&item, transaction);

			transaction.run();

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(item.getKey());
			}
		}
		
		
		
		bool TimetableRowGroupItemAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>();
		}
}	}

