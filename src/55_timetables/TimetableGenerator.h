
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
		class Line;
		class CommercialLine;
	}

	namespace timetables
	{
		class TimetableResult;

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
			typedef std::set<const pt::PhysicalStop*>		AuthorizedPhysicalStops;

		private:

			//! @name Content definition
			//@{
				Rows						_rows;
				calendar::Calendar			_baseCalendar;
				AuthorizedLines				_authorizedLines;
				AuthorizedPhysicalStops		_authorizedPhysicalStops;
				bool						_withContinuousServices;
				const util::Env&			_env;
			//@}

			//! @name Rendering parameters
			//@{
				int				_maxColumnsNumber;
			//@}

			//! @name Algorithms
			//@{
				bool	_isLineSelected(const pt::Line& line)	const;
				void	_insert(TimetableResult& result, const TimetableColumn& col);
				void	_buildWarnings(TimetableResult& result);
				void	_scanServices(TimetableResult& result, const pt::Line& line);
			//@}

		public:
			TimetableGenerator(
				const util::Env& env
			);

			//! @name Getters
			//@{
				const calendar::Calendar& getBaseCalendar() const { return _baseCalendar; }
				const Rows&		getRows()		const { return _rows; }
				const AuthorizedPhysicalStops& getAuthorizedPhysicalStops() const { return _authorizedPhysicalStops; }
			//@}

			//! @name Actions
			//@{
				TimetableResult build();
			//@}

			//! @name Setters
			//@{
				void setRows(const Rows& rows) { _rows = rows; }
				void setBaseCalendar(const calendar::Calendar& value) { _baseCalendar = value; }
				void setAuthorizedLines(const AuthorizedLines& value) { _authorizedLines = value; }
				void setAuthorizedPhysicalStops(const AuthorizedPhysicalStops& value) { _authorizedPhysicalStops = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableGenerator_h__
