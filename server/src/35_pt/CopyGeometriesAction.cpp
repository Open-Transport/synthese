
//////////////////////////////////////////////////////////////////////////
/// CopyGeometriesAction class implementation.
/// @file CopyGeometriesAction.cpp
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

#include "CopyGeometriesAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTUseRule.h"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::CopyGeometriesAction>::FACTORY_KEY("CopyGeometriesAction");
	}

	namespace pt
	{
		const string CopyGeometriesAction::PARAMETER_STARTING_STOP = Action_PARAMETER_PREFIX + "ss";
		const string CopyGeometriesAction::PARAMETER_ENDING_STOP = Action_PARAMETER_PREFIX + "es";
		const string CopyGeometriesAction::PARAMETER_EDGE_ID = Action_PARAMETER_PREFIX + "ei";


		ParametersMap CopyGeometriesAction::getParametersMap() const
		{
			ParametersMap map;
			if(_startingStop.get())
			{
				map.insert(PARAMETER_STARTING_STOP, _startingStop->getKey());
			}
			if(_endingStop.get())
			{
				map.insert(PARAMETER_ENDING_STOP, _endingStop->getKey());
			}
			if(_edgeTemplate.get())
			{
				map.insert(PARAMETER_EDGE_ID, _edgeTemplate->getKey());
			}
			return map;
		}



		void CopyGeometriesAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_startingStop = StopPointTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STARTING_STOP), *_env, UP_LINKS_LOAD_LEVEL);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such starting stop");
			}

			try
			{
				_endingStop = StopPointTableSync::Get(map.get<RegistryKeyType>(PARAMETER_ENDING_STOP), *_env, UP_LINKS_LOAD_LEVEL);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such ending stop");
			}

			RegistryKeyType templateId(map.getDefault<RegistryKeyType>(PARAMETER_EDGE_ID,0));
			if(templateId) try
			{
				_edgeTemplate = LineStopTableSync::Get(
					templateId,
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
				LineStopTableSync::Search(
					*_env,
					_edgeTemplate->get<Line>()->getKey()
				);
				const LineStop* next(_edgeTemplate->get<Line>()->getLineStop(_edgeTemplate->get<RankInPath>() + 1));
				if(	!_edgeTemplate->get<LineNode>() ||
					dynamic_cast<const StopPoint*>(&*_edgeTemplate->get<LineNode>()) != _startingStop.get() ||
					!next ||
					!next->get<LineNode>() ||
					&*next->get<LineNode>() != _endingStop.get()
				){
					throw ActionException("The edge and the stops does not match");
				}

				if(!_edgeTemplate->get<LineStringGeometry>().get())
				{
					throw ActionException("The edge does not define any geometry");
				}
			}
			catch(ObjectNotFoundException<LineStop>&)
			{
				throw ActionException("No such edge");
			}
		}



		void CopyGeometriesAction::run(
			Request& request
		){
			DBTransaction transaction;

			LineStopTableSync::SearchResult edges(
				LineStopTableSync::SearchByStops(
					*_env,
					_startingStop->getKey(),
					_endingStop->getKey()
			)	);

			// Selection of the template geometry
			boost::shared_ptr<LineString> geom;

			if(_edgeTemplate.get())
			{
				geom = _edgeTemplate->get<LineStringGeometry>();
			}
			else
			{
				BOOST_FOREACH(const boost::shared_ptr<LineStop>& edge, edges)
				{
					// The most detailed geometry is selected as template
					if(edge->get<LineStringGeometry>().get() &&
						(!geom.get() || edge->get<LineStringGeometry>()->getCoordinatesRO()->getSize() > geom->getCoordinatesRO()->getSize())
					){
						geom = edge->get<LineStringGeometry>();
					}
				}
			}

			if(geom.get())
			{
				BOOST_FOREACH(const boost::shared_ptr<LineStop>& edge, edges)
				{
					edge->set<LineStringGeometry>(geom);
					LineStopTableSync::Save(edge.get(), transaction);
				}
			}

			transaction.run();
		}



		bool CopyGeometriesAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
