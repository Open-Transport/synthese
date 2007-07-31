
/** JourneysResult class header.
	@file JourneysResult.h

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

#ifndef SYNTHESE_routeplanner_JourneysResult_h__
#define SYNTHESE_routeplanner_JourneysResult_h__

#include <map>
#include <set>

#include "15_env/JourneyComparator.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace env
	{
		class Vertex;
		class Journey;
	}

	namespace routeplanner
	{
		class BestVertexReachesMap;

		/** JourneysResult class.
			@ingroup m33
		*/
		class JourneysResult
		{			
		public:
			typedef std::set<env::Journey*, env::JourneyComparator> ResultSet;

		private:
			typedef std::map<const env::Vertex*, ResultSet::iterator> IndexMap;
			
			ResultSet	_result;
			IndexMap	_index;

		public:
			JourneysResult();

			//! @name Getters
			//@{
				const ResultSet& getJourneys() const;
			//@}

			//! @name Update methods
			//@{
				void remove(const env::Journey* journey);
				void add(env::Journey* journey);
				/** Gets the first journey of the result set and remove it.
					@return Pointer to the first journey
					@warning The returned pointer must be deleted after use
				*/
				const env::Journey* front();

				void cleanup(
					bool updateMinSpeed
					, const time::DateTime& newMaxTime
					, const BestVertexReachesMap& bvrm
				);
			//@}
			
			//! @name Queries
			//@{
				const env::Journey* const get(const env::Vertex* vertex) const;
				bool empty() const;
				void log() const;
			//@}
		};
	}
}

#endif // SYNTHESE_routeplanner_JourneysResult_h__
