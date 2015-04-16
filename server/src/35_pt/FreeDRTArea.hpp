
/** FreeDRTArea class header.
	@file FreeDRTArea.hpp

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

#ifndef SYNTHESE_pt_FreeDRTArea_hpp__
#define SYNTHESE_pt_FreeDRTArea_hpp__

#include "Registrable.h"
#include "Registry.h"
#include "Journey.h"
#include "Exception.h"
#include "Path.h"
#include "Edge.h"
#include "Vertex.h"
#include "Hub.h"

#include <set>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace geography
	{
		class City;
		class Place;
	}

	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt
	{
		class CommercialLine;
		class FreeDRTTimeSlot;
		class StopArea;
		class TransportNetwork;

		//////////////////////////////////////////////////////////////////////////
		/// Free DRT area.
		/// Contracts :
		///  - only one time slot is available at a specified date/time
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35
		/// @author Hugues Romain
		/// @date 2011
		class FreeDRTArea:
			public graph::Path
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<FreeDRTArea>	Registry;

			typedef std::set<geography::City*> Cities;
			typedef std::set<StopArea*> StopAreas;

			typedef std::map<
				pt::StopArea*,
				graph::Journey
			> ReachableStopAreas;

			enum Direction
			{
				DEPARTURE_TO_ARRIVAL,
				ARRIVAL_TO_DEPARTURE
			};

			class MissingGeometryInPlaceException:
				public synthese::Exception
			{
			public:
				MissingGeometryInPlaceException(
					const FreeDRTArea& area,
					const geography::Place& place
				);
			};

		private:
			/// @name Data
			//@{
				Cities _cities;
				StopAreas _stopAreas;
				std::string _name;
			//@}

			/// @name Links
			//@{
			//@}

		public:
			FreeDRTArea(
				util::RegistryKeyType key = 0
			);

			//! @name Getters
			//@{
				virtual std::string getName() const { return _name; }
				const Cities& getCities() const { return _cities; }
				const StopAreas& getStopAreas() const { return _stopAreas; }
			//@}

			//! @name Setters
			//@{
				void setCities(const Cities& value){ _cities = value; }
				void setStopAreas(const StopAreas& value){ _stopAreas = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			/// @name Modifiers
			//@{
				void setRollingStock(vehicle::RollingStock* value);
				void setNetwork(TransportNetwork* value);
				void setLine(CommercialLine* value);
				void addTimeSlot(FreeDRTTimeSlot& value);
				void removeTimeSlot(FreeDRTTimeSlot& value);
			//@}

			/// @name Services
			//@{
				virtual std::string getRuleUserName() const;
				virtual bool isPedestrianMode() const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if at least a service runs at the specified date.
				/// Fake implementation : always return true
				virtual bool isActive(const boost::gregorian::date& date) const;

				virtual bool isDepartureAllowed() const;
				virtual bool isArrivalAllowed() const;

				virtual graph::GraphIdType getGraphType() const;


				CommercialLine* getLine() const;
				vehicle::RollingStock*	getRollingStock()			const;
				TransportNetwork* getNetwork() const;


				//////////////////////////////////////////////////////////////////////////
				/// Checks if the place belongs to the area.
				/// @param place the place to check
				/// @result true if the place belongs to the area
				bool includesPlace(const geography::Place& place) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets all stop areas that can be reached from/to the specified place.
				/// @param direction departure to arrival or arrival to departure
				/// @param time time at the place (min if departure to arrival, max else)
				/// @param place (departure if departure to arrival, arrival else)
				/// @pre place must belong to the area (use includesPlace to check)
				/// @return the list of all reachable stop areas with the best journey for
				/// each of them
				ReachableStopAreas getReachableStopAreas(
					Direction direction,
					const boost::posix_time::ptime time,
					const geography::Place& place
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the next journey between to places.
				/// @param direction departure to arrival or arrival to departure
				/// @param time time (at the departure if departure to arrival, at the
				/// arrival else)
				/// @param from departure place
				/// @param to arrival place
				/// @return the journey as continuous service (real time availability is
				/// ignored at this stage
				graph::Journey getJourney(
					Direction direction,
					const boost::posix_time::ptime time,
					const geography::Place& from,
					const geography::Place& to
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_FreeDRTArea_hpp__

