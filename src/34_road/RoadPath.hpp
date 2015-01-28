
/** RoadPath class header.
	@file RoadPath.hpp

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

#ifndef SYNTHESE_road_RoadPath_hpp__
#define SYNTHESE_road_RoadPath_hpp__

#include "Path.h"

namespace synthese
{
	namespace road
	{
		class Road;
		class RoadPlace;

		/** RoadPath class.
			@ingroup m34
		*/
		class RoadPath:
			public graph::Path
		{
			friend class Road;

		private:
			Road* _road;

			void _setRoadPlace( RoadPlace& value );

/*			void _insertRoadChunk(
				RoadChunk& chunk,
				double length,
				std::size_t rankShift
			);
*/
		public:
			RoadPath(
				Road& road
			);
			virtual ~RoadPath();

			Road* getRoad() const { return _road; }

			virtual bool isPedestrianMode() const;
			virtual bool isRoad() const;
			virtual std::string getRuleUserName() const;
			virtual bool isActive(const boost::gregorian::date& date) const;
			RoadPlace* getRoadPlace() const;
		};
}	}

#endif // SYNTHESE_road_RoadPath_hpp__

