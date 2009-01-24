
/** Timetable class implementation.
	@file Timetable.cpp

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

#include "Timetable.h"
#include "TimetableRow.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> const std::string Registry<timetables::Timetable>::KEY("Timetable");
	}

	namespace timetables
	{
		Timetable::Timetable(RegistryKeyType id)
		:	Registrable(id),
			_mustBeginAPage(false),
			_bookId(0),
			_templateCalendarId(UNKNOWN_VALUE)
		{

		}



		void Timetable::setTitle( const std::string& title )
		{
			_title = title;
		}



		void Timetable::setMustBeginAPage( bool newVal )
		{
			_mustBeginAPage = newVal;
		}



		void Timetable::setBaseCalendar( const Calendar& calendar )
		{
			_baseCalendar = calendar;
		}



		void Timetable::addRow( const TimetableRow& row )
		{
			_rows.insert(_rows.begin() + row.getRank(), row);
		}



		bool Timetable::getMustBeginAPage() const
		{
			return _mustBeginAPage;
		}



		const Calendar& Timetable::getBaseCalendar() const
		{
			return _baseCalendar;
		}



		const std::string& Timetable::getTitle() const
		{
			return _title;
		}



		const Timetable::Rows& Timetable::getRows() const
		{
			return _rows;
		}



		void Timetable::setBookId( uid id )
		{
			_bookId = id;
		}



		void Timetable::setTemplateCalendarId( uid id )
		{
			_templateCalendarId = id;
		}



		uid Timetable::getTemplateCalendarId() const
		{
			return _templateCalendarId;
		}



		uid Timetable::getBookId() const
		{
			return _bookId;
		}



		void Timetable::setRank( int value )
		{
			_rank = value;
		}



		int Timetable::getRank() const
		{
			return _rank;
		}



		auto_ptr<TimetableGenerator> Timetable::getGenerator(
			const Env& env
		) const {
			auto_ptr<TimetableGenerator> g(new TimetableGenerator(env));
			g->setRows(_rows);
			return g;
		}



		void Timetable::setIsBook( bool value )
		{
			_isBook = value;
		}



		bool Timetable::getIsBook() const
		{
			return _isBook;
		}
	}
}
