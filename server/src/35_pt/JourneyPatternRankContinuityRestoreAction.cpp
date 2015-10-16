
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternRankContinuityRestoreAction class implementation.
/// @file JourneyPatternRankContinuityRestoreAction.cpp
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

#include "JourneyPatternRankContinuityRestoreAction.hpp"

#include "ActionException.h"
#include "Destination.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "UpdateQuery.hpp"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace graph;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::JourneyPatternRankContinuityRestoreAction>::FACTORY_KEY("JourneyPatternRankContinuityRestore");
	}

	namespace pt
	{
		const string JourneyPatternRankContinuityRestoreAction::PARAMETER_JOURNEY_PATTERN_ID = Action_PARAMETER_PREFIX + "journey_pattern_id";



		ParametersMap JourneyPatternRankContinuityRestoreAction::getParametersMap() const
		{
			ParametersMap map;
			if(_journeyPattern.get())
			{
				map.insert(PARAMETER_JOURNEY_PATTERN_ID, _journeyPattern->getKey());
			}
			return map;
		}



		void JourneyPatternRankContinuityRestoreAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Journey pattern
			try
			{
				_journeyPattern = JourneyPatternTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_JOURNEY_PATTERN_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such journey pattern");
			}
		}



		void JourneyPatternRankContinuityRestoreAction::run(
			Request& request
		){
			Env env;
			LineStopTableSync::SearchResult lineStops(
				LineStopTableSync::Search(
					env,
					_journeyPattern->getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true,
					true,
					FIELDS_ONLY_LOAD_LEVEL
			)	);
			size_t rank(0);
			DBTransaction transaction;
			BOOST_FOREACH(const boost::shared_ptr<LineStop>& lineStop, lineStops)
			{
				if(lineStop->get<RankInPath>() != rank)
				{
					UpdateQuery<LineStopTableSync> query;
					query.addUpdateField(RankInPath::FIELD.name, rank);
					query.addWhereField(TABLE_COL_ID, lineStop->getKey());
					query.execute(transaction);
				}
				++rank;
			}
			transaction.run();
		}



		bool JourneyPatternRankContinuityRestoreAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
