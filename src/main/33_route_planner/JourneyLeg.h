
/** JourneyLeg class header.
	@file JourneyLeg.h

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

#ifndef SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H
#define SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H

#include "04_time/DateTime.h"

#include "15_env/SquareDistance.h"
#include "15_env/ServicePointer.h"


#include <string>


namespace synthese
{

	namespace env
	{
		class Axis;
		class Service;
		class Edge;
		class Path;
	}

	namespace routeplanner
	{

		/** Journey leg class.

			@ingroup m33
		*/
		class JourneyLeg
		{
		 private:
			const env::ServicePointer _servicePointer;			//!< Used service.

			const env::Edge* _origin;   //!< Origin
			const env::Edge* _destination;  //!< Destination

			time::DateTime _departureTime; //!< Departure moment (first if continuous service)
			time::DateTime _arrivalTime;    //!< Arrival moment (first if continuous service)
			
			int _continuousServiceRange;
		    
			env::SquareDistance _squareDistance;

		 public:

			 JourneyLeg (const env::ServicePointer& servicePointer);
			~JourneyLeg ();

			//! @name Setters
			//@{
				void setDepartureTime (const time::DateTime& departureTime);
				void setArrivalTime (const time::DateTime& arrivalTime);
				void setOrigin (const env::Edge* origin);
				void setDestination (const env::Edge* destination);
				void setContinuousServiceRange (int continuousServiceRange);
				void setSquareDistance (const env::SquareDistance& squareDistance);
			//@}

			//! @name Getters
			//@{
				const env::Path*			getPath ()					const;
				const env::ServicePointer&	getServiceInstance()		const;
				const time::DateTime&		getDepartureTime ()			const;
				const time::DateTime&		getArrivalTime ()			const;
				const env::Edge*			getOrigin ()				const;
				const env::Edge*			getDestination ()			const;
				int							getContinuousServiceRange()	const;
				const env::SquareDistance&	getSquareDistance ()		const;
				env::SquareDistance&		getSquareDistance ();
			//@}


			//! @name Query methods
			//@{
				const synthese::env::Axis* getAxis () const;

				/** Returns this journey leg duration in minutes.
				 */
				int getDuration () const;

				int getDistance () const;
			//@}


			//! @name Update methods
			//@{

			//@}


		};
	}
}

#endif
