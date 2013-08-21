
/** StopPoint class header.
	@file StopPoint.hpp

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

#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H

#include "UtilConstants.h"
#include "Vertex.h"
#include "ImportableTemplate.hpp"
#include "Address.h"
#include "ReachableFromCrossing.hpp"

#include <string>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace pt
	{
		class LineStop;
		class StopArea;
		class CommercialLine;
		class JourneyPattern;

		//////////////////////////////////////////////////////////////////////////
		/// Physical stop (bus stop, etc.).
		///	A physical stop is an entry point to the transport network.
		///	RoutePoint
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class StopPoint:
			public graph::Vertex,
			public impex::ImportableTemplate<StopPoint>,
			public road::ReachableFromCrossing
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<StopPoint>	Registry;

		private:
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_X;
			static const std::string DATA_Y;
			static const std::string DATA_OPERATOR_CODE;

		public:
			static const std::string TAG_STOP_AREA;

		private:
			road::Address _projectedPoint;
			std::string _name;

		public:

			StopPoint(
				util::RegistryKeyType id = 0,
				std::string name = std::string(),
				const pt::StopArea* place = NULL,
				boost::shared_ptr<geos::geom::Point> geometry = boost::shared_ptr<geos::geom::Point>(),
				bool withIndexation = true
			);

			~StopPoint();


			//! @name Getters
			//@{
				const road::Address& getProjectedPoint() const { return _projectedPoint; }
				virtual std::string getName() const { return _name; }
			//@}

			//! @name Setters
			//@{
				void setProjectedPoint(const road::Address& value){ _projectedPoint = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			//! @name Services
			//@{
				const pt::StopArea* getConnectionPlace() const;
				virtual graph::GraphIdType getGraphType() const;
				virtual graph::VertexAccess getVertexAccess(const road::Crossing& crossing) const;
				virtual std::string getRuleUserName() const;



				typedef std::set<const CommercialLine*> LinesSet;

				//////////////////////////////////////////////////////////////////////////
				/// List of lines calling at the stop.
				/// @param withDepartures returns lines with a departure from the stop
				/// @param withArrivals returns lines with an arrival from the stop
				/// @return the lines calling at the stop
				/// @author Hugues Romain
				LinesSet getCommercialLines(
					bool withDepartures = true,
					bool withArrivals = true
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Mapped type : arrival / departure
				typedef std::map<JourneyPattern*, std::pair<bool, bool> > JourneyPatternsMap;



				//////////////////////////////////////////////////////////////////////////
				/// List of journey patterns calling at the stop
				/// @param withDepartures returns journey patterns with a departure from the stop
				/// @param withArrivals returns journey patterns with an arrival from the stop
				/// @return the journey patterns calling at the stop
				/// @author Hugues Romain
				JourneyPatternsMap getJourneyPatterns(
					bool withDepartures = true,
					bool withArrivals = true
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Parameters map output.
				/// @author Hugues Romain
				void toParametersMap(
					util::ParametersMap& pm,
					bool withStopAreaData,
					const CoordinatesSystem& coordinatesSystem,
					std::string prefix = std::string()
				) const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);
			//@}
		};

	}
}

#endif
