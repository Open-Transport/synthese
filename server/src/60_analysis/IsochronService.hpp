////////////////////////////////////////////////////////////////////////////////
/// IsochronService class header.
///	@file IsochronService.hpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_IsochronService_H__
#define SYNTHESE_IsochronService_H__

#include "AdminInterfaceElementTemplate.h"
#include "FactorableTemplate.h"
#include "Function.h"
#include "JourneysResult.h"
#include "StopArea.hpp"
#include "AccessParameters.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

namespace synthese
{
	class CoordinatesSystem;
	
	namespace algorithm
	{
		class AlgorithmLogger;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class StopArea;
	}		

	namespace analysis
	{
		class IsochronService:
			public util::FactorableTemplate<server::Function, IsochronService>
		{
		public:
			static const std::string PARAMETER_COORDINATES_XY;
			static const std::string PARAMETER_START_PLACE_NAME;
			static const std::string PARAMETER_MAX_DISTANCE;

			static const std::string PARAMETER_NETWORK_LIST;
			static const std::string PARAMETER_ROLLING_STOCK_LIST;

			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_BEGIN_TIME_SLOT;
			static const std::string PARAMETER_END_TIME_SLOT;
			static const std::string PARAMETER_MAX_CONNECTIONS;

			static const std::string PARAMETER_CURVES_STEP;
			static const std::string PARAMETER_MAX_DURATION;
			static const std::string PARAMETER_DURATION_TYPE;
			static const std::string PARAMETER_FREQUENCY_TYPE;
			static const std::string PARAMETER_SPEED;
			static const std::string PARAMETER_ONLY_WKT;

			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_BOARD_PAGE;
			static const std::string PARAMETER_STOP_PAGE;
			static const std::string PARAMETER_TIME_PAGE;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_DEPARTURE_CLASS_FILTER;

		private:
			static const std::string DATA_SOMETHING;
			graph::AccessParameters _accessParameters;
		private:
			typedef enum {
				DURATION_TYPE_FIXED_DATETIME = 0,
				DURATION_TYPE_BEST = 1,
				DURATION_TYPE_AVERAGE = 2,
				DURATION_TYPE_MEDIAN = 3,
				DURATION_TYPE_WORST = 4
			} DurationType;

			typedef enum {
				FREQUENCY_TYPE_NO = 0,
				FREQUENCY_TYPE_HALF_FREQUENCY_AVERAGE = 1,
				FREQUENCY_TYPE_HALF_FREQUENCY_MEDIAN = 2,
				FREQUENCY_TYPE_HALF_FREQUENCY_WORST = 3,
				FREQUENCY_TYPE_AVERAGE = 4,
				FREQUENCY_TYPE_MEDIAN = 5,
				FREQUENCY_TYPE_WORST = 6
			} FrequencyType;

			typedef struct {
				const pt::StopArea* stop;
				int nbSolutions;
				int duration;
				int distance;
				boost::posix_time::ptime lastDepartureTime;
				std::list<boost::posix_time::ptime> timeDepartureList;
			} StopStruct;

			// Best result for each StopArea and on one IntegralSearcher iteration
			typedef std::map<util::RegistryKeyType, algorithm::JourneysResult::ResultSet::const_iterator> BestResultsMap;

			typedef std::pair<int, StopStruct> stopPair;
			class SortableStop
			{
				public:
				bool operator()(const stopPair& stop1, const stopPair &stop2) const
				{
					return stop1.second.duration < stop2.second.duration;
				}
			};

			// Results by duration
			typedef std::multimap<int, StopStruct> ResultsMap;

			// ResultsMap access by StopArea
			typedef std::map<util::RegistryKeyType, ResultsMap::iterator> ResultsMapAccess;

			int _maxDistance;

			boost::gregorian::date _date;
			int _beginTimeSlot;
			int _endTimeSlot;
			int _maxConnections;
			int _curvesStep;
			int _maxDuration;
			int _speed;
			int _durationType;
			int _frequencyType;
			bool _onlyWKT;

			std::string	_startPlaceNameText;
			const CoordinatesSystem* _coordinatesSystem;
			boost::shared_ptr<geography::Place> _startPlace;

			boost::shared_ptr<const cms::Webpage> _page;
			boost::shared_ptr<const cms::Webpage> _boardPage;
			boost::shared_ptr<const cms::Webpage> _stopPage;
			boost::shared_ptr<const cms::Webpage> _timePage;

		public:
			IsochronService();



			virtual util::ParametersMap _getParametersMap() const;



			virtual void _setFromParametersMap(const util::ParametersMap& map);



			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			graph::VertexAccessMap _extendToPhysicalStops(
				const graph::VertexAccessMap& vam,
				const graph::VertexAccessMap& destinationVam,
				algorithm::PlanningPhase direction,
				const boost::posix_time::ptime& lowestDepartureTime,
				const boost::posix_time::ptime& lowestArrivalTime,
				const boost::posix_time::ptime& highestDepartureTime,
				const boost::posix_time::ptime& highestArrivalTime,
				const graph::AccessParameters& ap,
				const algorithm::AlgorithmLogger& logger
			) const;



			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_IsochronService_H__
