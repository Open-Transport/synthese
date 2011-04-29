
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
#include "CalendarTemplate.h"
#include "Env.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace calendar;
	using namespace pt;

	namespace util
	{
		template<> const std::string Registry<timetables::Timetable>::KEY("Timetable");
	}

	namespace timetables
	{
		Timetable::Timetable(RegistryKeyType id)
		:	Registrable(id),
			_bookId(0),
			_baseCalendar(NULL),
			_transferTimetableBefore(NULL),
			_transferTimetableAfter(NULL)
		{
		}



		void Timetable::addRow( const TimetableRow& row )
		{
			if(row.getRank() > _rows.size()) throw synthese::Exception("Invalid rank in timetable");
			_rows.insert(_rows.begin() + row.getRank(), row);
		}



		const Timetable::Rows& Timetable::getRows() const
		{
			return _rows;
		}



		auto_ptr<TimetableGenerator> Timetable::getGenerator(
			const Env& env
		) const {
			if(	!isGenerable()
			){
				throw ImpossibleGenerationException();
			}

			auto_ptr<TimetableGenerator> g(new TimetableGenerator(env));
			if(_contentType != CONTAINER)
			{
				g->setRows(_rows);
				g->setBaseCalendar(_baseCalendar->getResult());
				g->setAuthorizedLines(_authorizedLines);
				g->setAuthorizedPhysicalStops(_authorizedPhysicalStops);
				if(_transferTimetableBefore)
				{
					g->setBeforeTransferTimetable(_transferTimetableBefore->getGenerator(env));
				}
				if(_transferTimetableAfter)
				{
					g->setAfterTransferTimetable(_transferTimetableAfter->getGenerator(env));
				}
			}
			return g;
		}



		bool Timetable::isGenerable() const
		{
			return
				_contentType == CONTAINER ||
				getBaseCalendar() != NULL &&
				getBaseCalendar()->isLimited()
			;
		}



		std::string Timetable::GetFormatName(
			ContentType value
		){
			switch(value)
			{
			case TABLE_SERVICES_IN_COLS: return "Services en colonnes";
			case TABLE_SERVICES_IN_ROWS: return "Services en lignes";
			case TIMES_IN_COLS: return "Heures en colonnes";
			case TIMES_IN_ROWS: return "Heures en lignes";
			case CONTAINER: return "Container";
			case CALENDAR: return "Calendrier";
			case LINE_SCHEMA: return "Schéma de ligne";
			}
			return "Inconnu";
		}



		Timetable::ContentTypesList Timetable::GetFormatsList()
		{
			ContentTypesList result;
			result.push_back(make_pair(TABLE_SERVICES_IN_COLS, GetFormatName(TABLE_SERVICES_IN_COLS)));
			result.push_back(make_pair(TABLE_SERVICES_IN_ROWS, GetFormatName(TABLE_SERVICES_IN_ROWS)));
			result.push_back(make_pair(TIMES_IN_COLS, GetFormatName(TIMES_IN_COLS)));
			result.push_back(make_pair(TIMES_IN_ROWS, GetFormatName(TIMES_IN_ROWS)));
			result.push_back(make_pair(CONTAINER, GetFormatName(CONTAINER)));
			return result;
		}



		std::string Timetable::GetIcon( ContentType value )
		{
			switch(value)
			{
			case TABLE_SERVICES_IN_COLS: return "table_col.png";
			case TABLE_SERVICES_IN_ROWS: return "table_row.png";
			case TIMES_IN_COLS: return "shape_align_top.png";
			case TIMES_IN_ROWS: return "shape_align_left.png";
			case CONTAINER: return "table_multiple.png";
			case CALENDAR: return "calendar.png";
			case LINE_SCHEMA: return "chart_line.png";
			}
			return "Inconnu";
		}



		const TimetableGenerator::AuthorizedLines& Timetable::getAuthorizedLines() const
		{
			return _authorizedLines;
		}



		void Timetable::clearAuthorizedLines()
		{
			_authorizedLines.clear();
		}



		void Timetable::addAuthorizedLine( const CommercialLine* line )
		{
			_authorizedLines.insert(line);
		}



		void Timetable::removeAuthorizedLine( const pt::CommercialLine* line )
		{
			_authorizedLines.erase(line);
		}



		void Timetable::addAuthorizedPhysicalStop( const pt::StopPoint* stop )
		{
			_authorizedPhysicalStops.insert(stop);
		}



		void Timetable::removeAuthorizedPhysicalStop( const pt::StopPoint* stop )
		{
			_authorizedPhysicalStops.erase(stop);
		}



		void Timetable::clearAuthorizedPhysicalStops()
		{
			_authorizedPhysicalStops.clear();
		}



		const TimetableGenerator::AuthorizedPhysicalStops& Timetable::getAuthorizedPhysicalStops() const
		{
			return _authorizedPhysicalStops;
		}



		Timetable* Timetable::getTransferTimetableBefore( std::size_t depth /*= 0*/ )
		{
			if(depth == 0)
			{
				return this;
			}
			if(_transferTimetableBefore == NULL)
			{
				return NULL;
			}
			return _transferTimetableBefore->getTransferTimetableBefore(depth - 1);
		}



		Timetable* Timetable::getTransferTimetableAfter( std::size_t depth /*= 0*/ )
		{
			if(depth == 0)
			{
				return this;
			}
			if(_transferTimetableAfter == NULL)
			{
				return NULL;
			}
			return _transferTimetableAfter->getTransferTimetableAfter(depth - 1);
		}


		const Timetable* Timetable::getTransferTimetableBefore( std::size_t depth /*= 0*/ ) const
		{
			if(depth == 0)
			{
				return this;
			}
			if(_transferTimetableBefore == NULL)
			{
				return NULL;
			}
			return _transferTimetableBefore->getTransferTimetableBefore(depth - 1);
		}



		const Timetable* Timetable::getTransferTimetableAfter( std::size_t depth /*= 0*/ ) const
		{
			if(depth == 0)
			{
				return this;
			}
			if(_transferTimetableAfter == NULL)
			{
				return NULL;
			}
			return _transferTimetableAfter->getTransferTimetableAfter(depth - 1);
		}


		std::size_t Timetable::getBeforeTransferTimetablesNumber() const
		{
			return _transferTimetableBefore == NULL ? 0 : (_transferTimetableBefore->getBeforeTransferTimetablesNumber() + 1);
		}



		std::size_t Timetable::getAfterTransferTimetablesNumber() const
		{
			return _transferTimetableAfter == NULL ? 0 : (_transferTimetableAfter->getAfterTransferTimetablesNumber() + 1);
		}



		Timetable::ImpossibleGenerationException::ImpossibleGenerationException():
		synthese::Exception("Timetable generation is impossible.")
		{

		}
	}
}
