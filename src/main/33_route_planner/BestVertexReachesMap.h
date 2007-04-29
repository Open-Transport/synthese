
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
		class JourneyLeg;

		/** Best vertex reaches map class.
			@ingroup m33
		*/
		class BestVertexReachesMap
		{
		 private:

			typedef std::map<const synthese::env::Vertex*, JourneyLeg*> JourneyLegMap;
			typedef std::map<const synthese::env::Vertex*, synthese::time::DateTime> TimeMap;
		    
			const AccessDirection _accessDirection;

			JourneyLegMap _bestJourneyLegMap;
			TimeMap _bestTimeMap;

		 public:


			BestVertexReachesMap (const AccessDirection& accessDirection);
			~BestVertexReachesMap ();


			//! @name Getters/Setters
			//@{
			//@}


			//! @name Query methods
			//@{
			bool contains (const synthese::env::Vertex* vertex) const;

			const synthese::time::DateTime& 
			getBestTime (const synthese::env::Vertex* vertex, 
					 const synthese::time::DateTime& defaultValue) const;

			JourneyLeg* getBestJourneyLeg (const synthese::env::Vertex* vertex);

			//@}


			//! @name Update methods
			//@{
			void clear ();
			void insert (const synthese::env::Vertex* vertex, 
				 JourneyLeg* journeyLeg);
		    
			void insert (const synthese::env::Vertex* vertex, 
				 const synthese::time::DateTime& dateTime,
				 bool propagateInConnectionPlace = true);

			//@}


		 private:


		};
	}
}

#endif
