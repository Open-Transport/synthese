////////////////////////////////////////////////////////////////////////////////
/// RoadJourneyPlannerService class header.
///	@file RoadJourneyPlannerService.hpp
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

#ifndef SYNTHESE_RoadJourneyPlannerService_H__
#define SYNTHESE_RoadJourneyPlannerService_H__

#include "AccessParameters.h"
#include "FactorableTemplate.h"
#include "Function.h"
#include "RoadModule.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace road_journey_planner
	{
		class RoadJourneyPlannerService:
			public util::FactorableTemplate<server::Function, RoadJourneyPlannerService>
		{
		public:
			static const std::string PARAMETER_DEPARTURE_PLACE_TEXT;
			static const std::string PARAMETER_ARRIVAL_PLACE_TEXT;
			static const std::string PARAMETER_ACCESSIBILITY;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_DEPARTURE_PLACE_XY;
			static const std::string PARAMETER_ARRIVAL_PLACE_XY;
			static const std::string PARAMETER_INVERT_XY;

			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_BOARD_PAGE;
			static const std::string PARAMETER_STEP_PAGE;
			static const std::string PARAMETER_ERROR_PAGE;

		private:
			static const std::string DATA_WKT;

			static const std::string DATA_BOARD;
			static const std::string DATA_DEPARTURE_NAME;
			static const std::string DATA_ARRIVAL_NAME;
			static const std::string DATA_ORIGIN_CITY_TEXT;
			static const std::string DATA_ORIGIN_PLACE_TEXT;
			static const std::string DATA_DESTINATION_CITY_TEXT;
			static const std::string DATA_DESTINATION_PLACE_TEXT;
			static const std::string DATA_DEPARTURE_TIME;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_DURATION;
			static const std::string DATA_DISTANCE;

			static const std::string DATA_STEPS;
			static const std::string DATA_RANK;
			static const std::string DATA_ROAD_NAME;
			static const std::string DATA_STEP_DISTANCE;
			static const std::string DATA_TOTAL_DISTANCE;
			static const std::string DATA_DEPARTURE_STEP_TIME;
			static const std::string DATA_ARRIVAL_STEP_TIME;
			static const std::string DATA_STEP_DURATION;

			static const std::string DATA_ERROR_MESSAGE;

			static const std::string DATA_BOARD_MAP;
			static const std::string DATA_STEP_MAP;

		private:
			road::RoadModule::ExtendedFetchPlaceResult _departure_place;
			road::RoadModule::ExtendedFetchPlaceResult _arrival_place;
			const CoordinatesSystem* _coordinatesSystem;
			graph::AccessParameters _accessParameters;
			std::string _originPlaceText;
			std::string _destinationPlaceText;
			bool _carTrip;

			boost::shared_ptr<const cms::Webpage> _page;
			boost::shared_ptr<const cms::Webpage> _boardPage;
			boost::shared_ptr<const cms::Webpage> _stepPage;
			boost::shared_ptr<const cms::Webpage> _errorPage;

		public:
			virtual util::ParametersMap _getParametersMap() const;



			virtual void _setFromParametersMap(const util::ParametersMap& map);



			RoadJourneyPlannerService();



			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_RoadJourneyPlannerService_H__
