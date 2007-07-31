
/** JourneysResult class implementation.
	@file JourneysResult.cpp

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

#include "JourneysResult.h"

#include "33_route_planner/BestVertexReachesMap.h"

#include "15_env/Journey.h"
#include "15_env/Edge.h"
#include "15_env/JourneyComparator.h"
#include "15_env/Vertex.h"
#include "15_env/ConnectionPlace.h"

#include "01_util/Log.h"

#include <vector>
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace util;

	namespace routeplanner
	{
		JourneysResult::JourneysResult()
		{
		}

		void JourneysResult::add(Journey* journey )
		{
			remove(journey);
			const Vertex* vertex(journey->getEndEdge()->getFromVertex());
			_index.insert(make_pair(vertex, _result.insert(journey).first));
		}

		void JourneysResult::remove( const Journey* journey )
		{
			const Vertex* vertex(journey->getEndEdge()->getFromVertex());
			IndexMap::iterator it(_index.find(vertex));
			if (it != _index.end())
			{
				ResultSet::iterator its(it->second);
				const Journey* ptr = *its;
				_result.erase(its);
				_index.erase(it);
				delete ptr;
			}
		}

		const Journey* JourneysResult::front()
		{
			assert(!empty());

			const Journey* ptr(*_result.begin());
			_index.erase(ptr->getEndEdge()->getFromVertex());
			_result.erase(_result.begin());
			return ptr;
		}

		bool JourneysResult::empty() const
		{
			return _result.empty();
		}

		const Journey* const JourneysResult::get( const Vertex* vertex ) const
		{
			IndexMap::const_iterator it(_index.find(vertex));
			if (it != _index.end())
			{
				ResultSet::const_iterator its(it->second);
				return *its;
			}
			else
				return NULL;
		}

		const JourneysResult::ResultSet& JourneysResult::getJourneys() const
		{
			return _result;
		}

		void JourneysResult::cleanup(
			bool updateMinSpeed
			, const time::DateTime& newMaxTime
			, const BestVertexReachesMap& bvrm
		){
			vector<Journey*> journeysToAdd;
			vector<Journey*> journeysToRemove;
			for (IndexMap::iterator it(_index.begin()); it != _index.end();)
			{
				Journey* journey(*it->second);
				IndexMap::iterator next(it);
				++next;
				if (bvrm.mustBeCleared(it->first, journey->getEndTime(), newMaxTime))
					journeysToRemove.push_back(journey);
				else if (updateMinSpeed)
				{
					_result.erase(it->second);
					_index.erase(it);
					journey->setMinSpeedToEnd(newMaxTime);
					journeysToAdd.push_back(journey);
				}
				it = next;
			}
			for (vector<Journey*>::const_iterator it(journeysToRemove.begin()); it != journeysToRemove.end(); ++it)
				remove(*it);
			for (vector<Journey*>::const_iterator it(journeysToAdd.begin()); it != journeysToAdd.end(); ++it)
				add(*it);
		}

		void JourneysResult::log() const
		{
			stringstream s;
			s << "Exploration queue : (size=" << _result.size() << ")";
			for (JourneysResult::ResultSet::const_iterator it(_result.begin()); it != _result.end(); ++it)
			{
				const Journey& journey(**it);
				s	<< "\n -> " << journey.getEndEdge()->getFromVertex()->getConnectionPlace()->getFullName()
					<< " at " << journey.getEndTime().toString()
					<< "(dst = " << journey.getSquareDistanceToEnd().getDistance()
					<< " - min speed = " << journey.getMinSpeedToEnd()
					<< ")";
			}
			s << "\n";
			Log::GetInstance().trace(s.str());
		}
	}
}
