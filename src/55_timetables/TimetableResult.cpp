
/** TimetableResult class implementation.
	@file TimetableResult.cpp

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

#include "TimetableResult.hpp"
#include "Line.h"

namespace synthese
{
	namespace timetables
	{
		TimetableResult::RowTimesVector TimetableResult::getRowSchedules(
			std::size_t rank
		) const	{
			RowTimesVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
			{
				const TimetableColumn::Content& content(it->getContent());
				result.push_back((content.begin() + rank)->second);
			}
			return result;
		}



		TimetableResult::RowLinesVector TimetableResult::getRowLines() const
		{
			RowLinesVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getLine()->getCommercialLine());
			return result;
		}



		TimetableResult::RowNotesVector TimetableResult::getRowNotes() const
		{
			RowNotesVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getWarning());
			return result;
		}



		TimetableResult::RowRollingStockVector TimetableResult::getRowRollingStock() const
		{
			RowRollingStockVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getLine()->getRollingStock());
			return result;
		}



		TimetableResult::RowTypeODVector TimetableResult::getOriginTypes() const
		{
			RowTypeODVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getOriginType());
			return result;
		}



		TimetableResult::RowTypeODVector TimetableResult::getDestinationTypes() const
		{
			RowTypeODVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getDestinationType());
			return result;
		}
	}
}
