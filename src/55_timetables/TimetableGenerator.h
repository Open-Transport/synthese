
/** TimetableGenerator class header.
	@file TimetableGenerator.h

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

#ifndef SYNTHESE_timetables_TimetableGenerator_h__
#define SYNTHESE_timetables_TimetableGenerator_h__

#include <vector>
#include <set>

#include "Calendar.h"
#include "TimetableRow.h"
#include "TimetableResult.hpp"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace pt
	{
		class JourneyPattern;
		class CommercialLine;
	}

	namespace timetables
	{
		class TimetableResult;
		class TimetableRowGroup;

		//////////////////////////////////////////////////////////////////////////
		/// Timetable generator.
		///	@ingroup m55
		///	@author Hugues Romain
		/// @date 2002
		class TimetableGenerator
		{
		public:
			typedef std::vector<TimetableRow>				Rows;
			typedef std::set<const pt::CommercialLine*>	AuthorizedLines;
			typedef std::set<const pt::StopPoint*>		AuthorizedPhysicalStops;

			struct RowGroupsSort
			{
				bool operator()(
					TimetableRowGroup* g1,
					TimetableRowGroup* g2
				) const;
			};
			typedef std::set<TimetableRowGroup*, RowGroupsSort> RowGroups;

		private:

			//! @name Content definition
			//@{
				std::auto_ptr<TimetableGenerator>	_transferTimetableBefore;
				std::auto_ptr<TimetableGenerator>	_transferTimetableAfter;
				RowGroups					_rowGroups;
				calendar::Calendar			_baseCalendar;
				AuthorizedLines				_authorizedLines;
				AuthorizedPhysicalStops		_authorizedPhysicalStops;
				bool						_withContinuousServices;
				boost::optional<bool>		_wayBackFilter;
				const util::Env&			_env;
				boost::optional<std::size_t> _autoIntermediateStops;
				bool _mergeColsWithSameTimetables;
				bool _compression;
			//@}

			/// @name Result
			//@{
				mutable Rows						_rows;
			//@}

			//! @name Rendering parameters
			//@{
			//@}

			//! @name Algorithms
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if a journey pattern should be present on the timetable according
				/// to its definition.
				/// @param journeyPattern the journey pattern to test
				bool _isJourneyPatternSelected(const pt::JourneyPattern& journeyPattern) const;



				void	_insert(TimetableResult& result, const TimetableColumn& col) const;
				void	_buildWarnings(TimetableResult& result) const;
				void	_scanServices(TimetableResult& result, const pt::JourneyPattern& line) const;
			//@}

		public:
			TimetableGenerator(
				const util::Env& env
			);

			//! @name Getters
			//@{
				const calendar::Calendar& getBaseCalendar() const { return _baseCalendar; }
				const Rows&		getRows()		const { return _rows; }
				const RowGroups& getRowGroups() const { return _rowGroups; }
				const AuthorizedPhysicalStops& getAuthorizedPhysicalStops() const { return _authorizedPhysicalStops; }
				boost::optional<std::size_t> getAutoIntermediateStops() const { return _autoIntermediateStops; }
			//@}

			//! @name Services
			//@{
				const TimetableGenerator& getBeforeTransferTimetable(std::size_t depth) const;
				const TimetableGenerator& getAfterTransferTimetable(std::size_t depth) const;
			//@}

			//! @name Actions
			//@{
				TimetableResult build(
					bool withWarnings,
					boost::shared_ptr<TimetableResult::Warnings> warnings
				) const;
			//@}

			//! @name Setters
			//@{
				void setRows(const Rows& rows) { _rows = rows; }
				void setRowGroups(const RowGroups& value) { _rowGroups = value; }
				void setBaseCalendar(const calendar::Calendar& value) { _baseCalendar = value; }
				void setAuthorizedLines(const AuthorizedLines& value) { _authorizedLines = value; }
				void setAuthorizedPhysicalStops(const AuthorizedPhysicalStops& value) { _authorizedPhysicalStops = value; }
				void setBeforeTransferTimetable(std::auto_ptr<TimetableGenerator> value){ _transferTimetableBefore = value; }
				void setAfterTransferTimetable(std::auto_ptr<TimetableGenerator> value){ _transferTimetableAfter = value; }
				void setWaybackFilter(boost::optional<bool> value){ _wayBackFilter = value; }
				void setAutoIntermediateStops(boost::optional<std::size_t> value){ _autoIntermediateStops = value; }
				void setMergeColsWithSameTimetables(bool value){ _mergeColsWithSameTimetables = value; }
				void setCompression(bool value){ _compression = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableGenerator_h__
