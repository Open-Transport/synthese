
//////////////////////////////////////////////////////////////////////////////////////////
/// PTRouteDetailFunction class header.
///	@file PTRouteDetailFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_PTRouteDetailFunction_H__
#define SYNTHESE_PTRouteDetailFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace graph
	{
		class Edge;
	}

	namespace pt
	{
		class JourneyPattern;
		class SchedulesBasedService;

		//////////////////////////////////////////////////////////////////////////
		///	35.15 Function : PTRouteDetailFunction.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Route_detail
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		class PTRouteDetailFunction:
			public util::FactorableTemplate<server::Function,PTRouteDetailFunction>
		{
		public:
			static const std::string PARAMETER_MAIN_PAGE_ID;
			static const std::string PARAMETER_STOP_PAGE_ID;
			static const std::string PARAMETER_DISPLAY_DEPARTURE_STOPS;
			static const std::string PARAMETER_DISPLAY_ARRIVAL_STOPS;
			static const std::string PARAMETER_DISPLAY_SAME_STOP_AREA_ONCE;

		private:
			static const std::string TAG_ROUTE;
			static const std::string TAG_STOP;
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_LINE_ID;
			static const std::string DATA_LINE_NAME;
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_LONG_NAME;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_LINE_COLOR;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_DIRECTION;
            static const std::string DATA_WAYBACK;
			static const std::string DATA_STOPS;
			static const std::string DATA_OPERATOR_CODE;
			static const std::string DATA_RANK;
			static const std::string DATA_DEPARTURE_IS_ALLOWED;
			static const std::string DATA_ARRIVAL_IS_ALLOWED;
			static const std::string DATA_STOP_AREA_ID;
			static const std::string DATA_STOP_AREA_NAME;
			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_DIRECTION_ALIAS;
			static const std::string DATA_DEPARTURE_TIME;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_IS_MAIN;
			static const std::string DATA_WITH_SCHEDULES;
			static const std::string DATA_RESERVATION_NEEDED;

			//! \name Page parameters
			//@{
				boost::shared_ptr<const JourneyPattern> _journeyPattern;
				boost::shared_ptr<const SchedulesBasedService> _service;
				bool _displayDepartureStops;
				bool _displayArrivalStops;
				boost::shared_ptr<const cms::Webpage> _mainPage;
				boost::shared_ptr<const cms::Webpage> _stopPage;
				bool _displaySameStopAreaOnce;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Route_detail
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Route_detail
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



			void _displayStop(
				std::ostream& stream,
				const graph::Edge& edge
			) const;


		public:
			PTRouteDetailFunction();

			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Route_detail
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_PTRouteDetailFunction_H__
