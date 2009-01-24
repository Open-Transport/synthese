
/** TimetableRow class implementation.
	@file TimetableRow.cpp

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

#include "TimetableRow.h"

namespace synthese
{
	namespace timetables
	{
		TimetableRow::TimetableRow()
			: _isArrival(true)
			, _isDeparture(true)
			, _place(NULL)
			, _compulsory(PassageFacultatif)
			, _rank(UNKNOWN_VALUE)
			, _timetableId(UNKNOWN_VALUE)
		{

		}




		void TimetableRow::setRank( int rank )
		{
			_rank = rank;
		}



		void TimetableRow::setPlace( const env::PublicTransportStopZoneConnectionPlace* place )
		{
			_place = place;
		}



		void TimetableRow::setCompulsory(tTypeGareIndicateur compulsory )
		{
			_compulsory = compulsory;
		}



		void TimetableRow::setTimetableId( uid id )
		{
			_timetableId = id;
		}



		const env::PublicTransportStopZoneConnectionPlace* TimetableRow::getPlace() const
		{
			return _place;
		}



		synthese::timetables::tTypeGareIndicateur TimetableRow::getCompulsory() const
		{
			return _compulsory;
		}



		int TimetableRow::getRank() const
		{
			return _rank;
		}



		uid TimetableRow::getTimetableId() const
		{
			return _timetableId;
		}



		bool TimetableRow::getIsArrival() const
		{
			return _isArrival;
		}



		bool TimetableRow::getIsDeparture() const
		{
			return _isDeparture;
		}



		void TimetableRow::setIsArrival( bool value )
		{
			_isArrival = value;
		}



		void TimetableRow::setIsDeparture( bool value )
		{
			_isDeparture = value;
		}
	}
}
