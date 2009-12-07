
/** TimetableGenerator class header.
	@file TimetableGenerator.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "TimetableColumn.h"
#include "TimetableWarning.h"
#include "TimetableRow.h"

namespace synthese
{
	namespace time
	{
		class Schedule;
	}
	
	namespace util
	{
		class Env;
	}

	namespace env
	{
		class Line;
		class CommercialLine;
	}

	namespace timetables
	{
		/** TimetableGenerator class.
			@ingroup m55
		*/
		class TimetableGenerator
		{
		public:
			typedef std::map<std::size_t, TimetableWarning>		Warnings;
			typedef std::vector<std::size_t>	ColumnWarnings;
			typedef std::vector<TimetableColumn>			Columns;
			typedef std::vector<TimetableRow>				Rows;
			typedef std::set<const env::CommercialLine*>	AuthorizedLines;

		private:

			//! @name Content definition
			//@{
				Rows				_rows;
				calendar::Calendar		_baseCalendar;
				AuthorizedLines				_authorizedLines;
				bool				_withContinuousServices;
				const util::Env&	_env;
			//@}

			//! @name Rendering parameters
			//@{
				int				_maxColumnsNumber;
			//@}

			//! @name Results
			//@{
				Columns			_columns;
				Warnings		_warnings;
			//@}
			

			//! @name Algorithms
			//@{
				bool	_isLineSelected(const env::Line& line)	const;
				void	_insert(const TimetableColumn& col);
				void	_buildWarnings();
			//@}

		public:
			TimetableGenerator(
				const util::Env& env
			);

			//! @name Getters
			//@{
				const calendar::Calendar& getBaseCalendar() const;
				const Warnings&	getWarnings()	const;
				const Rows&		getRows()		const;
				const Columns&	getColumns()	const;
			//@}

			//! @name Actions
			//@{
				void build();
			//@}

			//! @name Output by row
			//@{
				std::vector<time::Schedule>		getSchedulesByRow(Rows::const_iterator row)	const;
				std::vector<const env::Line*>	getLines()									const;
				std::vector<tTypeOD>			getOriginTypes()							const;
				std::vector<tTypeOD>			getDestinationTypes()						const;
				ColumnWarnings					getColumnsWarnings()	const;
			//@}

			//! @name Setters
			//@{
				void setRows(const Rows& rows);
				void setBaseCalendar(const calendar::Calendar& value);
				void setAuthorizedLines(const AuthorizedLines& value);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableGenerator_h__
