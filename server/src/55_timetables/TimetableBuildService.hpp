
//////////////////////////////////////////////////////////////////////////////////////////
/// TimetableBuildService class header.
///	@file TimetableBuildService.h
///	@author Hugues
///	@date 2009
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

#ifndef SYNTHESE_TimetableBuildService_H__
#define SYNTHESE_TimetableBuildService_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "PlacesList.hpp"
#include "Timetable.h"
#include "TimetableResult.hpp"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class JourneyPattern;
	}

	namespace timetables
	{
		class TimetableGenerator;
		class TimetableWarning;
		class TimetableRowGroup;
		class TimetableRowGroupItem;

		//////////////////////////////////////////////////////////////////////////
		/// 55.15 Function : Display timetable generation result.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2009-2010
		///	@ingroup m55Functions refFunctions
		class TimetableBuildService:
			public util::FactorableTemplate<server::Function,TimetableBuildService>
		{
		public:
			static const std::string PARAMETER_CALENDAR_ID;
			static const std::string PARAMETER_STOP_PREFIX;
			static const std::string PARAMETER_CITY_PREFIX;
			static const std::string PARAMETER_DAY;
			static const std::string PARAMETER_WAYBACK_FILTER;
			static const std::string PARAMETER_IGNORE_PAST_DATES;
			static const std::string PARAMETER_AUTO_INTERMEDIATE_STOPS;
			static const std::string PARAMETER_ROWS_BEFORE;
			static const std::string PARAMETER_ROWS_AFTER;
			static const std::string PARAMETER_AUTO_SORT_ROWS_BEFORE;
			static const std::string PARAMETER_AUTO_SORT_ROWS_AFTER;
			static const std::string PARAMETER_MERGE_COLS_WITH_SAME_SCHEDULES;
			static const std::string PARAMETER_WITH_WARNINGS;

			static const std::string TAG_TIMETABLE;
			static const std::string ATTR_TIMETABLE_RANK;

			static const std::string TAG_NOTE;
			static const std::string TAG_LINE;

			static const std::string TAG_STOP_ROW;
			static const std::string ATTR_IS_BEFORE_TRANSFER;
			static const std::string ATTR_TRANSFER_DEPTH;
			static const std::string ATTR_IS_ARRIVAL;
			static const std::string ATTR_IS_DEPARTURE;
			static const std::string TAG_PLACE;
			static const std::string TAG_STOP_POINT;

			static const std::string TAG_COL;

			static const std::string TAG_CELL;
			static const std::string ATTR_TIME;
			static const std::string TAG_SERVICE;
			static const std::string ATTR_ID;

			static const std::string TAG_COMPRESSION_CELL;
			static const std::string ATTR_COMPRESSION_RANK;
			static const std::string ATTR_COMPRESSION_REPEATED;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Timetable>	_timetable;
				boost::shared_ptr<const calendar::CalendarTemplate>	_calendarTemplate;
				boost::shared_ptr<const pt::CommercialLine> _commercialLine;
				boost::shared_ptr<const pt::JourneyPattern> _line;
				boost::shared_ptr<TimetableResult::Warnings> _warnings;
				boost::optional<bool> _waybackFilter;
				boost::optional<bool> _ignorePastDates;
				std::vector<boost::shared_ptr<Timetable> > _containerContent;
				std::size_t _timetableRank;
				boost::optional<bool> _mergeColsWithSameSchedules;
				bool _withWarnings;
			//@}

			mutable std::vector<boost::shared_ptr<TimetableRowGroup> > _rowGroupsOwner;
			mutable std::vector<boost::shared_ptr<TimetableRowGroupItem> > _rowGroupItemsOwner;

			typedef algorithm::PlacesList<const pt::StopArea*, const pt::JourneyPattern*> PlacesListConfiguration;

			static void AddLineDirectionToTimetable(
				Timetable& timetable,
				const pt::CommercialLine& line,
				bool wayBack
			);

			void _addPlacesToRows(
				Timetable& timetable,
				const std::string& places,
				bool before,
				bool autoSort
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			TimetableBuildService();

			//! @name Setters
			//@{
				void setTimetable(boost::shared_ptr<const Timetable> value){ _timetable = value; }
				void setLine(boost::shared_ptr<const pt::JourneyPattern> value){ _line = value; }
				void setCalendarTemplate(boost::shared_ptr<const calendar::CalendarTemplate> value){ _calendarTemplate = value; }
				void setIgnorePastDates(bool value){ _ignorePastDates = value; }
				void setTimetableRank(std::size_t value){ _timetableRank = value; }
				void setWarnings(boost::shared_ptr<TimetableResult::Warnings> value){ _warnings = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Timetable_content_generator#XML-Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @author Hugues Romain
			/// @date 2009
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;


			void _outputResult(
				util::ParametersMap& pm,
				boost::shared_ptr<const Timetable> object,
				std::size_t rank
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			///	Output of a row containing schedules into a parameters map.
			///	@param place informations about the row
			///	@param times Time of each column (not_a_date_time = stop not served)
			void _outputStopRow(
				util::ParametersMap& pm,
				const TimetableRow& place,
				const TimetableResult::RowTimesVector& times,
				const TimetableResult::RowServicesVector& services,
				const TimetableResult::Columns& columns,
				bool isBeforeTransfer,
				size_t depth
			) const;



			boost::shared_ptr<util::ParametersMap> _outputCell(
				const TimetableResult::RowTimesVector& times,
				const TimetableResult::RowServicesVector& services,
				const TimetableResult::Columns& columns,
				size_t rank,
				bool extractCompression
			) const;
		};
}	}

#endif // SYNTHESE_TimetableGenerateFunction_H__
