
/** BestVertexReachesMap class header.
	@file BestVertexReachesMap.h

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

#ifndef SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H
#define SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H


#include "15_env/Types.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/ServiceUse.h"

#include "04_time/DateTime.h"

#include <map>

namespace synthese
{

	namespace env
	{
		class Vertex;
	}


	namespace routeplanner
	{
		/** Best vertex reaches map class.
			@ingroup m33
		*/
		class BestVertexReachesMap
		{
		 private:


			typedef std::map<const env::Vertex*, env::ServiceUse> JourneyLegMap;
			typedef std::map<const env::Vertex*, time::DateTime> TimeMap;
		    
			AccessDirection _accessDirection;

			JourneyLegMap _bestJourneyLegMap;
			TimeMap _bestTimeMap;

		 public:


			BestVertexReachesMap();
			~BestVertexReachesMap();


			//! @name Getters/Setters
			//@{
			//@}


			//! @name Query methods
			//@{
			bool contains (const env::Vertex* vertex) const;

			const time::DateTime& getBestTime (const env::Vertex* vertex, 
					 const time::DateTime& defaultValue) const;

			//@}


			//! @name Update methods
			//@{
			void clear (const AccessDirection& accessDirection);
			void insert (const env::ServiceUse& journeyLeg);
		    
			void insert (const env::Vertex* vertex, 
				 const time::DateTime& dateTime,
				 bool propagateInConnectionPlace = true);

			//@}


		 private:


		};
	}
}

#endif
