
/** RoadChunk class header.
	@file RoadChunk.h

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

#ifndef SYNTHESE_ENV_ROADCHUNK_H
#define SYNTHESE_ENV_ROADCHUNK_H

#include "Edge.h"
#include "Road.h"

#include <utility>

namespace geos
{
	namespace geom
	{
		class Point;
}	}

namespace synthese
{
	namespace road
	{
		class Crossing;

		struct CarSpeedFactors {
			typedef std::map<Road::RoadType, double> SpeedFactors;
			SpeedFactors _factors;

			CarSpeedFactors() : _factors()
			{
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_UNKNOWN, 0.7));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_MOTORWAY, 0.9));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_MEDIANSTRIPPEDROAD, 0.85));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_PRINCIPLEAXIS, 0.8));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_SECONDARYAXIS, 0.6));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_BRIDGE, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_STREET, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_PEDESTRIANSTREET, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_ACCESSROAD, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_PRIVATEWAY, 0.2));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_PEDESTRIANPATH, 0.2));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_TUNNEL, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_HIGHWAY, 0.8));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_STEPS, 0.5));
				_factors.insert(SpeedFactors::value_type(Road::ROAD_TYPE_SERVICE, 0.2));
			}
		};

		//////////////////////////////////////////////////////////////////////////
		/// Association class between road and crossings.
		///	@ingroup m35
		/// @author Marc Jambert, Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		///	A road chunk is always delimited by two adresses, with no other
		///	address in between.
		///	These delimiting addresses can correspond to :
		///		- a crossing address
		///		- a physical stop address
		///		- a public place address
		class RoadChunk:
			public graph::Edge
		{
		public :
			static const CarSpeedFactors CAR_SPEED_FACTORS;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id identifier (default 0)
			/// @param fromCrossing crossing where the chunk begins (default NULL)
			/// @param rankInRoad rank of the chunk in the road path (default unknown)
			/// @param road road which the chunk belongs to (default NULL)
			/// @param metricOffset distance between the the chunk beginning and the road beginning (default unknown)
			RoadChunk(
				util::RegistryKeyType id = 0,
				Crossing* fromCrossing = NULL,
				int rankInRoad = UNKNOWN_VALUE,
				Road* road = NULL,
				double metricOffset = UNKNOWN_VALUE,
				double carSpeed = 50 / 3.6
			);

			virtual ~RoadChunk ();

			double _carSpeed;

		public:
			//! @name Getters
			//@{
				Crossing* getFromCrossing() const;
				double getCarSpeed(bool nominalSpeed = false) const;
			//@}



			//! @name Setters
			//@{
				void setFromCrossing(Crossing* value);
				void setRoad(Road* road);
				void setCarSpeed(double carSpeed){ _carSpeed = carSpeed; }
			//@}



			//! @name Query methods.
			//@{
				virtual bool isDepartureAllowed() const;
				virtual bool isArrivalAllowed() const;
				virtual bool isReversed() const = 0;

				Road* getRoad() const;

				//////////////////////////////////////////////////////////////////////////
				/// Extracts the point corresponding to an offset from the beginning of the chunk.
				/// If the offset is greater than the length of the chunk, then the method
				///	returns the last point of the chunk
				/// @param metricOffset
				/// @return the point corresponding to an offset from the beginning of the chunk.
				boost::shared_ptr<geos::geom::Point> getPointFromOffset(
					graph::MetricOffset metricOffset
				) const;


				void unlink();
			//@}
		};
}	}

#endif
