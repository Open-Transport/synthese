
/** Path class header.
	@file Path.h

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

#ifndef SYNTHESE_ENV_PATH_H
#define SYNTHESE_ENV_PATH_H

#include <vector>
#include <set>

#include "01_util/UId.h"

#include "04_time/Schedule.h"

#include "15_env/BikeComplyer.h"
#include "15_env/HandicappedComplyer.h"
#include "15_env/PedestrianComplyer.h"
#include "15_env/ReservationRuleComplyer.h"
#include "15_env/Calendar.h"
#include "15_env/Service.h"

namespace synthese
{

	namespace time
	{
		class DateTime;
	}

	namespace messages
	{
		class Alarm;
	}

	namespace env
	{

		class Axis;
		class Edge;
		class Fare;
		class Point;

		/** Path abstract base class.

			A path is a sequence of edges.

			A path is associated with a set of services allowing
			to follow this path at certain dates and times.

			It is associated as well with a set of compliances, defining
			which types of entities are able to move along this path. For instance : 
				- a bus line is compliant with pedestrians, may be compliant with bikes
				and wheelchairs
				- a road may be compliant only with cars, or only for pedestrians and bikes,
				or for all
				- a ferry line is compliant with cars, bikes, pedestrian
				- ...

			@ingroup m15
		*/
		class Path : 
			public BikeComplyer,
			public HandicappedComplyer,
			public PedestrianComplyer,
			public ReservationRuleComplyer
		    
		{

		private:

			struct cmpService
			{
				bool operator() (const Service* s1, const Service* s2) const
				{
					return s1->getDepartureSchedule () < s2->getDepartureSchedule ();
				}
			};

		public:

//			typedef std::set<Service*, cmpService> ServiceSet;
			typedef std::set<Service*> ServiceSet;

		protected:

			std::vector<Edge*> _edges; 
			ServiceSet _services;

			Fare* _fare;
			const messages::Alarm* _alarm;

			Calendar _calendar; //!< Calendar indicating if there is at least one service running on each day.

			Path ();

		public:

			virtual ~Path ();


			//! @name Getters/Setters
			//@{
				virtual const uid& getId () const = 0;

				int getEdgesCount () const;
				const Edge* getEdge (int index) const;

				const ServiceSet& getServices () const;
				const Service* getService (int serviceIndex) const;

				const Fare* getFare () const;
				void setFare (Fare* fare);

				bool hasApplicableAlarm (const synthese::time::DateTime& start, 
							const synthese::time::DateTime& end) const;
				const messages::Alarm* getAlarm () const;
				void setAlarm (messages::Alarm* alarm);

				virtual const Axis* getAxis () const = 0;

				const std::vector<Edge*>& getEdges () const;
			//@}

			//! @name Query methods.
			//@{

				virtual bool isRoad () const = 0;
				virtual bool isLine () const = 0;

				/** Gets all the geographical points linked by the path
					between two of its edges. If no from/to edge
					index is provided, all the edges are considered.
					@param fromEdgeIndex 
					@param toEdgeIndex 

					This includes :
					- vertices (address/physical stops)
					- via points
				*/
				std::vector<const Point*> getPoints (int fromEdgeIndex = 0,
								int toEdgeIndex = -1) const;

				bool isInService (const synthese::time::Date& date) const;

				Edge*	getLastEdge()	const;

			//@}
		    
			//! @name Update methods.
			//@{

				/** Adds edge at the end of the path.
					@param edge The edge to add

					All the pointer links necessary to the graph exploration are created :
						- the links between edges (describing the path)
				*/
				void addEdge (Edge* edge);

				void addService (Service* service, 
						const std::vector<synthese::time::Schedule>& departureSchedules,
						const std::vector<synthese::time::Schedule>& arrivalSchedules);

				void removeService (Service* service);


				/** Updates path calendar.

				The generated calendar indicates whether or not a day contains at least one service.
				It takes into account services running after midnight : if at least one minute
				of a day is concerned by a service, then the whole day is selected.

				Thus, if a calculation request is done on a deselected calendar day, the path 
				can safely be filtered.
				*/
				void updateCalendar ();
			//@}
		    
		};
	}
}

#endif 	    
