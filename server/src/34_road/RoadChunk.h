
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

#include "Registrable.h"
#include "WithGeometry.hpp"

#include "GraphTypes.h"
#include "RoadTypes.hpp"

#include <utility>

namespace geos
{
	namespace geom
	{
		class LineString;
		class Point;
}	}

namespace synthese
{
	namespace road
	{
		class Crossing;
		class House;
		class Road;
		class RoadChunkEdge;

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
			public util::Registrable,
			public WithGeometry<geos::geom::LineString>
		{
		public :
			typedef util::Registry<RoadChunk> Registry;

			static const CarSpeedFactors CAR_SPEED_FACTORS;

			typedef std::map<HouseNumber, House*> Houses;

		private:
			HouseNumberBounds _leftHouseNumberBounds;
			HouseNumberBounds _rightHouseNumberBounds;
			HouseNumberingPolicy _leftHouseNumberingPolicy;
			HouseNumberingPolicy _rightHouseNumberingPolicy;

			Houses _houses;
			int _carOneWay;
			double _carSpeed;

			Road* _road;
			Crossing* _crossing;
			size_t _rankInPath;
			graph::MetricOffset _metricOffset;
			bool _nonWalkable;
			bool _nonDrivable;
			bool _nonBikable;

			boost::shared_ptr<RoadChunkEdge> _forwardEdge;
			boost::shared_ptr<RoadChunkEdge> _reverseEdge;

			void _updateEdges() const;

			static bool _testIfHouseNumberBelongsToBounds(
				const HouseNumberBounds& bounds,
				const HouseNumberingPolicy& policy,
				HouseNumber houseNumber
			);

			//////////////////////////////////////////////////////////////////////////
			/// @pre bounds are defined
			/// @pre startOffset <= endOffset
			static graph::MetricOffset _computeMetricOffset(
				const HouseNumberBounds& bounds,
				HouseNumber houseNumber,
				graph::MetricOffset startOffset,
				graph::MetricOffset endOffset
			); 

		public:
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
				size_t rankInRoad = 0,
				Road* road = NULL,
				double metricOffset = UNKNOWN_VALUE,
				double carSpeed = 50 / 3.6
			);

			virtual ~RoadChunk ();

			//! @name Setters
			//@{
				void setLeftHouseNumberBounds(const HouseNumberBounds& value){ _leftHouseNumberBounds = value; }
				void setRightHouseNumberBounds(const HouseNumberBounds& value){ _rightHouseNumberBounds = value; }
				void setLeftHouseNumberingPolicy(const HouseNumberingPolicy& value){ _leftHouseNumberingPolicy = value; }
				void setRightHouseNumberingPolicy(const HouseNumberingPolicy& value){ _rightHouseNumberingPolicy = value; }
				void addHouse(House& house);
				void removeHouse(House& house);
				void setCarOneWay(int value){ _carOneWay = value; }
				void setFromCrossing(Crossing* value){ _crossing = value; }
				void setRoad(Road* road){ _road = road; }
				void setCarSpeed(double carSpeed){ _carSpeed = carSpeed; }
				void setRankInPath(size_t value){ _rankInPath = value; }
				void setMetricOffset(graph::MetricOffset value){ _metricOffset = value; }
				void setNonDrivable(bool value){ _nonDrivable = value; }
				void setNonWalkable(bool value){ _nonWalkable = value; }
				void setNonBikable(bool value){ _nonBikable = value; }
			//@}

			//! @name Getters
			//@{
				const HouseNumberBounds& getLeftHouseNumberBounds() const { return _leftHouseNumberBounds; }
				const HouseNumberBounds& getRightHouseNumberBounds() const { return _rightHouseNumberBounds; }
				const HouseNumberingPolicy& getLeftHouseNumberingPolicy() const { return _leftHouseNumberingPolicy; }
				const HouseNumberingPolicy& getRightHouseNumberingPolicy() const { return _rightHouseNumberingPolicy; }
				const Houses& getHouses() const { return _houses; }
				int getCarOneWay() const { return _carOneWay; }
				Crossing* getFromCrossing() const { return _crossing; }
				double getCarSpeed(bool nominalSpeed = false) const;
				Road* getRoad() const { return _road; }
				size_t getRankInPath() const { return _rankInPath; }
				graph::MetricOffset getMetricOffset() const { return _metricOffset; }
				bool getNonDrivable() const { return _nonDrivable; }
				bool getNonWalkable() const { return _nonWalkable; }
				bool getNonBikable() const { return _nonBikable; }
				RoadChunkEdge& getForwardEdge() const { return *_forwardEdge; }
				RoadChunkEdge& getReverseEdge() const { return *_reverseEdge; }
			//@}

				

			//! @name Services.
			//@{
				boost::shared_ptr<geos::geom::LineString> getRealGeometry() const;

				//////////////////////////////////////////////////////////////////////////
				/// Extracts the point corresponding to an offset from the beginning of the chunk.
				/// If the offset is greater than the length of the chunk, then the method
				///	returns the last point of the chunk
				/// @param metricOffset
				/// @return the point corresponding to an offset from the beginning of the chunk.
				boost::shared_ptr<geos::geom::Point> getPointFromOffset(
					graph::MetricOffset metricOffset
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Tests if a numeric house number belongs to the chunk
				/// @param houseNumber the house number to test
				/// @return true if the house number belongs to the chunk
				/// @author Hugues Romain
				/// @since 3.2.0
				bool testIfHouseNumberBelongsToChunk(
					HouseNumber houseNumber
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Computes metric offset of a valid house number
				/// @param houseNumber the house number
				/// @return the metric offset of the house in the path
				/// @pre houseNumber must be valid, the house number bounds of the chunk must be
				/// defined. Use testIfHouseNumberBelongsToChunk to check this pre-condition.
				/// @author Hugues Romain
				/// @since 3.2.0
				graph::MetricOffset getHouseNumberMetricOffset(
					HouseNumber houseNumber
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Computes house number for a metric offset
				/// @param metricOffset the metric offset
				/// @return the house number closest to this metric offset
				HouseNumber getHouseNumberFromOffset(
					double metricOffset
				) const;

				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual SubObjects getSubObjects() const;

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif
