////////////////////////////////////////////////////////////////////////////////
/// MultimodalJourneyPlannerService class header.
///	@file MultimodalJourneyPlannerService.hpp
///	@author Camille Hue
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

#ifndef SYNTHESE_MultimodalJourneyPlannerService_H__
#define SYNTHESE_MultimodalJourneyPlannerService_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"
#include "RoadModule.h"

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace server
	{
		class Request;
	}

	namespace multimodal_journey_planner
	{
		////////////////////////////////////////////////////////////////////
		/// 64.15 Function : multimodal journey planner.
		///	@ingroup m64Functions refFunctions
		/// @author Camille Hue
		///
		/// Usage : https://extranet.rcsmobility.com/projects/synthese/wiki/Multimodal_journey_planner
		///
		class MultimodalJourneyPlannerService:
			public util::FactorableTemplate<cms::FunctionWithSite<false>, MultimodalJourneyPlannerService>
		{
		public:
			static const std::string PARAMETER_DEPARTURE_CITY_TEXT;
			static const std::string PARAMETER_ARRIVAL_CITY_TEXT;
			static const std::string PARAMETER_DEPARTURE_PLACE_TEXT;
			static const std::string PARAMETER_ARRIVAL_PLACE_TEXT;
			static const std::string PARAMETER_DEPARTURE_DAY;
			static const std::string PARAMETER_DEPARTURE_TIME;
			static const std::string PARAMETER_USE_WALK;
			static const std::string PARAMETER_USE_PT;

			static const std::string PARAMETER_ASTAR_FOR_WALK; //TODO : remove when algorithm is chosen

			static const std::string DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT;
			static const std::string DATA_ERROR_MESSAGE;

		private:
			// TEMP
			bool _aStarForWalk;
			//! \name Parameters
			//@{
				std::string									_originCityText;
				std::string									_destinationCityText;
				std::string									_originPlaceText;
				std::string									_destinationPlaceText;
				boost::gregorian::date						_departureDay;
				boost::posix_time::time_duration			_departureTime;
				boost::optional<size_t>						_maxTransportConnectionCount;

				bool										_useWalk;
				bool										_usePt;

				bool										_useWalk;
				bool										_usePt;

				road::RoadModule::ExtendedFetchPlaceResult	_departure_place;
				road::RoadModule::ExtendedFetchPlaceResult	_arrival_place;

				boost::filesystem::path  					_loggerPath;
			//@}

			//! @name Pages
			//@{
			//@}

			/// @name Result
			//@{
				//mutable boost::shared_ptr<PTRoutePlannerResult> _result;
			//@}

		public:
			//! @name Getters
			//@{
				const std::string& getOutputFormat() const { return _outputFormat; }
			//@}

			//! @name Setters
			//@{
				void setOutputFormat(const std::string& value){ _outputFormat = value; }
				void setOriginCityText(const std::string& value){ _originCityText = value; }
				void setOriginPlaceText(const std::string& value){ _originPlaceText = value; }
				void setDestinationCityText(const std::string& value){ _destinationCityText = value; }
				void setDestinationPlaceText(const std::string& value){ _destinationPlaceText = value; }
			//@}

			/// @name Modifiers
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			virtual util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			virtual void _setFromParametersMap(const util::ParametersMap& map);



			MultimodalJourneyPlannerService();



			/** Action to run, defined by each subclass.
			*/
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_MultimodalJourneyPlannerService_H__
