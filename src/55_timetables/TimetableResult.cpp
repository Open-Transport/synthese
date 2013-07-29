
/** TimetableResult class implementation.
	@file TimetableResult.cpp

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

#include "TimetableResult.hpp"

#include "JourneyPattern.hpp"

#include <boost/foreach.hpp>

using namespace boost;

namespace synthese
{
	namespace timetables
	{
		TimetableResult::RowTimesVector TimetableResult::getRowSchedules(
			size_t rank
		) const	{
			RowTimesVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
			{
				const TimetableColumn::Content& content(it->getContent());
				result.push_back(*(content.begin() + rank));
			}
			return result;
		}



		bool TimetableResult::hasSchedules( size_t rank ) const
		{
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
			{
				const TimetableColumn::Content& content(it->getContent());
				if(!(content.begin() + rank)->second.is_not_a_date_time())
				{
					return true;
				}
			}
			return false;
		}



		TimetableResult::RowLinesVector TimetableResult::getRowLines() const
		{
			RowLinesVector result;
			for (Columns::const_iterator it(_columns.begin()); it != _columns.end(); ++it)
				result.push_back(it->getLine() ? it->getLine()->getCommercialLine() : NULL);
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



		const TimetableResult& TimetableResult::getBeforeTransferTimetable( std::size_t depth ) const
		{
			if(depth == 0)
			{
				return *this;
			}
			else
			{
				return _beforeTransfers->getBeforeTransferTimetable(depth - 1);
			}
		}

		TimetableResult& TimetableResult::getBeforeTransferTimetable( std::size_t depth )
		{
			if(depth == 0)
			{
				return *this;
			}
			else
			{
				return _beforeTransfers->getBeforeTransferTimetable(depth - 1);
			}
		}


		const TimetableResult& TimetableResult::getAfterTransferTimetable( std::size_t depth ) const
		{
			if(depth == 0)
			{
				return *this;
			}
			else
			{
				return _afterTransfers->getAfterTransferTimetable(depth - 1);
			}
		}

		TimetableResult& TimetableResult::getAfterTransferTimetable( std::size_t depth )
		{
			if(depth == 0)
			{
				return *this;
			}
			else
			{
				return _afterTransfers->getAfterTransferTimetable(depth - 1);
			}
		}



		void TimetableResult::createBeforeTransfer()
		{
			_beforeTransfers.reset(new TimetableResult(_warnings));
		}



		void TimetableResult::createAfterTransfer()
		{
			_afterTransfers.reset(new TimetableResult(_warnings));
		}



		TimetableResult::TimetableResult(
			boost::shared_ptr<Warnings> warnings
		):	_warnings(warnings.get() ? warnings : boost::shared_ptr<Warnings>(new Warnings))
		{}



		TimetableResult TimetableResult::copy() const
		{
			return TimetableResult(_warnings);
		}



		TimetableResult::RowServicesVector TimetableResult::getRowServices() const
		{
			RowServicesVector result;
			BOOST_FOREACH(const Columns::value_type& col, _columns)
			{
				result.push_back(col.getServices());
			}
			return result;
		}
}	}
