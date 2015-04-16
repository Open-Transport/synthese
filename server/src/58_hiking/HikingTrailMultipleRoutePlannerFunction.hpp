
//////////////////////////////////////////////////////////////////////////////////////////
/// HikingTrailMultipleRoutePlannerFunction class header.
///	@file HikingTrailMultipleRoutePlannerFunction.hpp
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

#ifndef SYNTHESE_HikingTrailMultipleRoutePlannerFunction_H__
#define SYNTHESE_HikingTrailMultipleRoutePlannerFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "RoadModule.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class HourPeriod;
		class PTServiceConfig;
	}

	namespace hiking
	{
		class HikingTrail;

		//////////////////////////////////////////////////////////////////////////
		///	58.15 Function : HikingTrailMultipleRoutePlannerFunction.
		///	@ingroup m58Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : HikingTrailMultipleRoutePlannerFunction
		///
		/// Parameters :
		///	<ul>
		///		<li>roid : id of the trail</li>
		///		<li>ci : name of the departure city</li>
		///		<li>na : name of the departure stop</li>
		///		<li>br : rank of the stop where the trail must begin</li>
		///		<li>er : rank of the stop(s) where the trail can end (multiple value are accepted)</li>
		///		<li>dp : id of the page to use to display the schedule sheets</li>
		///		<li>si : id of the website (for route planning parameters)</li>
		///		<li>bd : beginning journey day (SQL date format)</li>
		///		<li>bp : beginning journey period (related to website periods)</li>
		///		<li>ed : ending journey day (SQL date format)</li>
		///		<li>ep : ending journey period (related to website periods)</li>
		///	</ul>
		///
		/// The page will be displayed with the following parameters :
		///	<ul>
		///		<li>(object) : route planner result</li>
		///		<li>beginning : 1 if the sheet corresponds to the beginning approach</li>
		///		<li>ending_rank : rank of the ending journey</li>
		///	</ul>
		class HikingTrailMultipleRoutePlannerFunction:
			public util::FactorableTemplate<server::Function,HikingTrailMultipleRoutePlannerFunction>
		{
		public:
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_STOP;
			static const std::string PARAMETER_TRAIL_BEGINNING;
			static const std::string PARAMETER_TRAIL_ENDING;
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_BEGINNING_DAY;
			static const std::string PARAMETER_BEGINNING_PERIOD;
			static const std::string PARAMETER_ENDING_DAY;
			static const std::string PARAMETER_ENDING_PERIOD;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_SCHEDULES_ROW_PAGE;
			static const std::string PARAMETER_SCHEDULES_CELL_PAGE;
			static const std::string PARAMETER_LINES_ROW_PAGE;
			static const std::string PARAMETER_LINE_MARKER_PAGE;
			static const std::string PARAMETER_BOARD_PAGE;
			static const std::string PARAMETER_WARNING_PAGE;
			static const std::string PARAMETER_RESERVATION_PAGE;
			static const std::string PARAMETER_DURATION_PAGE;
			static const std::string PARAMETER_TEXT_DURATION_PAGE;
			static const std::string PARAMETER_MAP_PAGE;
			static const std::string PARAMETER_MAP_LINE_PAGE;
			static const std::string PARAMETER_DATE_TIME_PAGE;
			static const std::string PARAMETER_STOP_CELL_PAGE;
			static const std::string PARAMETER_SERVICE_CELL_PAGE;
			static const std::string PARAMETER_JUNCTION_CELL_PAGE;
			static const std::string PARAMETER_MAP_STOP_PAGE;
			static const std::string PARAMETER_MAP_SERVICE_PAGE;
			static const std::string PARAMETER_MAP_JUNCTION_PAGE;

		protected:
			//! \name Page parameters
			//@{
				road::RoadModule::ExtendedFetchPlaceResult	_place;
				boost::shared_ptr<const HikingTrail> _trail;
				std::size_t _beginningRank;
				std::vector<std::size_t> _endingRanks;
				boost::shared_ptr<const pt_website::PTServiceConfig> _site;
				boost::gregorian::date					_beginningDay;
				std::size_t								_beginningPeriodId;
				const pt_website::HourPeriod*		_beginningPeriod;
				boost::gregorian::date					_endingDay;
				std::size_t								_endingPeriodId;
				const pt_website::HourPeriod*		_endingPeriod;
			//@}

			//! @name Display pages
			//@{
				boost::shared_ptr<const cms::Webpage> _page;
				boost::shared_ptr<const cms::Webpage> _schedulesRowPage;
				boost::shared_ptr<const cms::Webpage> _schedulesCellPage;
				boost::shared_ptr<const cms::Webpage> _linesRowPage;
				boost::shared_ptr<const cms::Webpage> _lineMarkerPage;
				boost::shared_ptr<const cms::Webpage> _boardPage;
				boost::shared_ptr<const cms::Webpage> _warningPage;
				boost::shared_ptr<const cms::Webpage> _reservationPage;
				boost::shared_ptr<const cms::Webpage> _durationPage;
				boost::shared_ptr<const cms::Webpage> _textDurationPage;
				boost::shared_ptr<const cms::Webpage> _mapPage;
				boost::shared_ptr<const cms::Webpage> _mapLinePage;
				boost::shared_ptr<const cms::Webpage> _dateTimePage;
				boost::shared_ptr<const cms::Webpage> _stopCellPage;
				boost::shared_ptr<const cms::Webpage> _serviceCellPage;
				boost::shared_ptr<const cms::Webpage> _junctionPage;
				boost::shared_ptr<const cms::Webpage> _mapStopCellPage;
				boost::shared_ptr<const cms::Webpage> _mapServiceCellPage;
				boost::shared_ptr<const cms::Webpage> _mapJunctionPage;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
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
	}
}

#endif // SYNTHESE_HikingTrailMultipleRoutePlannerFunction_H__
