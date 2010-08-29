
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternRemoveAction class implementation.
/// @file JourneyPatternRemoveAction.cpp
/// @author Hugues Romain
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
#include "JourneyPatternRemoveAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "Fetcher.h"
#include "Service.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace graph;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::JourneyPatternRemoveAction>::FACTORY_KEY("JourneyPatternRemoveAction");
	}

	namespace pt
	{
		const string JourneyPatternRemoveAction::PARAMETER_JOURNEY_PATTERN_ID = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap JourneyPatternRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_journeyPattern.get())
			{
				map.insert(PARAMETER_JOURNEY_PATTERN_ID, _journeyPattern->getKey());
			}
			return map;
		}
		
		
		
		void JourneyPatternRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_journeyPattern = Env::GetOfficialEnv().get<JourneyPattern>(map.get<RegistryKeyType>(PARAMETER_JOURNEY_PATTERN_ID));
			}
			catch(ObjectNotFoundException<JourneyPattern>&)
			{
				throw ActionException("No such journey pattern");
			}
		}
		
		
		
		void JourneyPatternRemoveAction::run(
			Request& request
		){
			// Services deletion
			SQLiteTransaction serviceTransaction;
			BOOST_FOREACH(Service* service, _journeyPattern->getServices())
			{
				Fetcher<Service>::FetchRemove(*service, serviceTransaction);
			}
			serviceTransaction.run();

			// LineStops deletion
			SQLiteTransaction lineStopsTransaction;
			BOOST_FOREACH(Edge* edge, _journeyPattern->getEdges())
			{
				LineStopTableSync::Remove(edge->getKey(), lineStopsTransaction);
			}
			lineStopsTransaction.run();

			// Object deletion
			JourneyPatternTableSync::Remove(_journeyPattern->getKey());
		//	::AddDeleteEntry(*_object, request.getUser().get());

		}
		
		
		
		bool JourneyPatternRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}
	}
}
