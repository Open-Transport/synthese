////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class header.
///	@file LinesListFunction.h
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

#ifndef SYNTHESE_LinesListFunction_H__
#define SYNTHESE_LinesListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "Calendar.h"
#include "SecurityTypes.hpp"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	class CoordinatesSystem;

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace geography
	{
		class City;
	}

	namespace tree
	{
		class TreeFolder;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class RollingStockFilter;
	}

	namespace messages
	{
		class CustomBroadcastPoint;
	}

	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt
	{
		class CommercialLine;
		class TransportNetwork;
		class ReservationContact;

		////////////////////////////////////////////////////////////////////
		/// 35.15 Function : Lines list public Function class.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Lines_list
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2008 2011
		///	@ingroup m35Functions refFunctions
		class LinesListFunction:
			public util::FactorableTemplate<server::Function,LinesListFunction>
		{
		 private:
			static const std::string PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES;
			static const std::string PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES;
			static const std::string PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES;
			static const std::string PARAMETER_IGNORE_LINE_SHORT_NAME;
			static const std::string PARAMETER_LETTERS_BEFORE_NUMBERS;
			static const std::string PARAMETER_NETWORK_ID;
			static const std::string PARAMETER_OUTPUT_GEOMETRY;
			static const std::string PARAMETER_OUTPUT_STOPS;
			static const std::string PARAMETER_OUTPUT_TERMINUSES;
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			static const std::string PARAMETER_SORT_BY_TRANSPORT_MODE;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_RIGHT_CLASS;
			static const std::string PARAMETER_RIGHT_LEVEL;
			static const std::string PARAMETER_CONTACT_CENTER_ID;
			static const std::string PARAMETER_CITY_FILTER;
			static const std::string PARAMETER_STOP_AREA_TERMINUS_PAGE_ID;
			static const std::string PARAMETER_DATE_FILTER;
			static const std::string PARAMETER_CALENDAR_FILTER;
			static const std::string PARAMETER_RUNS_SOON_FILTER;
			static const std::string PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE_FILTER;
			static const std::string PARAMETER_BROADCAST_POINT_ID;
			static const std::string PARAMETER_WITH_DIRECTIONS;

			static const std::string FORMAT_WKT;

			static const std::string TAG_FORWARD_DIRECTION;
			static const std::string TAG_BACKWARD_DIRECTION;
			static const std::string ATTR_DIRECTION;
			static const std::string TAG_LINE;
			static const std::string DATA_LINES;
			static const std::string DATA_STOP_AREAS;
			static const std::string DATA_STOP_AREA;
			static const std::string DATA_TRANSPORT_MODE;
			static const std::string DATA_GEOMETRY;
			static const std::string DATA_WKT;
			static const std::string DATA_EDGE;
			static const std::string DATA_POINT;
			static const std::string DATA_X;
			static const std::string DATA_Y;
			static const std::string DATA_RANK;
			static const std::string DATA_TERMINUS;
			static const std::string DATA_STOP_AREA_TERMINUS;

		protected:
			//! \name Page parameters
			//@{
				std::set<const pt::TransportNetwork*> _networks;
				std::set<const tree::TreeFolder*> _folders;
				std::set<const geography::City*> _cities;
				boost::shared_ptr<const pt::CommercialLine> _line;
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _stopAreaTerminusPage;
				const CoordinatesSystem* _coordinatesSystem;
				bool _outputStops;
				bool _outputTerminuses;
				bool _ignoreTimetableExcludedLines;
				bool _ignoreJourneyPlannerExcludedLines;
				bool _ignoreDeparturesBoardExcludedLines;
				bool _ignoreLineShortName;
				bool _withDirections;
				bool _lettersBeforeNumbers;
				std::string _outputGeometry;
				boost::shared_ptr<const pt_website::RollingStockFilter>	_rollingStockFilter;
				typedef std::vector<boost::shared_ptr<const vehicle::RollingStock> > TransportModeSorting;
				TransportModeSorting _sortByTransportMode;
				std::string _rightClass;
				boost::optional<security::RightLevel> _rightLevel;
				boost::optional<boost::shared_ptr<const ReservationContact> > _contactCenterFilter;
				boost::optional<boost::gregorian::date> _dateFilter;
				boost::optional<boost::shared_ptr<const calendar::CalendarTemplate> > _calendarFilter;
				calendar::Calendar _calendarDaysFilter;
				boost::optional<boost::posix_time::time_duration> _runsSoonFilter;
				bool _displayDurationBeforeFirstDepartureFilter;
				const messages::CustomBroadcastPoint* _broadcastPoint;
			//@}

			mutable boost::optional<const security::RightsOfSameClassMap&> _rights;


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// https://extranet.rcsmobility.com/projects/synthese/wiki/Lines_list#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// https://extranet.rcsmobility.com/projects/synthese/wiki/Lines_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to read
			void _setFromParametersMap(const util::ParametersMap& map);



			//////////////////////////////////////////////////////////////////////////
			/// Checks if the line must be selected according to the service parameters.
			/// All filters must be passed :
			///  - the line must be the child of a parent folder or network
			///  - the line must call at a city of the filter list
			///  - the user rights must allow the user to access to the line in read mode
			///  - the line must not be ignored according to the use rules
			///  - the line must use the transport mode specified in the filter
			///  - the line must use the contact center specified in the filter
			/// @param line the line to check
			/// @param request the current request
			/// @warning update the _rights cache before using _lineIsSelected
			bool _lineIsSelected(
				const CommercialLine& line,
				const server::Request& request
			) const;

		public:
			LinesListFunction();

			//////////////////////////////////////////////////////////////////////////
			/// Output.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Lines_list#Response
			//////////////////////////////////////////////////////////////////////////
			util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;


			//! @name Setters
			//@{
				void setNetwork(const TransportNetwork* value);
			//@}

			virtual bool isAuthorized(const server::Session* session) const;


			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_LinesListFunction_H__
