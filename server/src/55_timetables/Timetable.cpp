
/** Timetable class implementation.
	@file Timetable.cpp

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

#include "Timetable.h"

#include "CalendarTemplate.h"
#include "CommercialLine.h"
#include "Conversion.h"
#include "Request.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "TimetableRowTableSync.h"
#include "TimetableRowGroupTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace calendar;
	using namespace pt;
	using namespace server;
	using namespace timetables;

	CLASS_DEFINITION(Timetable, "t052_timetables", 52)
	FIELD_DEFINITION_OF_OBJECT(Timetable, "timetable_id", "timetable_ids")

	FIELD_DEFINITION_OF_TYPE(Book, "book_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(BaseCalendar, "calendar_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Format, "format", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AuthorizedLines, "authorized_lines", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(AuthorizedPhysicalStops, "authorized_physical_stops", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TransferTimetableBefore, "transfer_timetable_before", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TransferTimetableAfter, "transfer_timetable_after", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(IgnoreEmptyRows, "ignore_empty_rows", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Compression, "compression", SQL_BOOLEAN)



	namespace timetables
	{
		const std::string Timetable::DATA_GENERATOR_TYPE("generator_type");
		const std::string Timetable::DATA_TITLE("title");
		const std::string Timetable::DATA_CALENDAR_NAME("calendar_name");



		Timetable::Timetable(
			RegistryKeyType id
		):	Registrable(id),
			Object<Timetable, TimetableSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Book),
					FIELD_VALUE_CONSTRUCTOR(Rank, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Title),
					FIELD_DEFAULT_CONSTRUCTOR(BaseCalendar),
					FIELD_DEFAULT_CONSTRUCTOR(Format),
					FIELD_DEFAULT_CONSTRUCTOR(AuthorizedLines),
					FIELD_DEFAULT_CONSTRUCTOR(AuthorizedPhysicalStops),
					FIELD_DEFAULT_CONSTRUCTOR(TransferTimetableBefore),
					FIELD_DEFAULT_CONSTRUCTOR(TransferTimetableAfter),
					FIELD_VALUE_CONSTRUCTOR(IgnoreEmptyRows, false),
					FIELD_VALUE_CONSTRUCTOR(Compression, false)
			)	),
            _contentType(CONTAINER),
			_mergeColsWithSameTimetables(true)
		{}



		void Timetable::computeStrAndSetAuthorizedLines()
		{
			string authorized_lines_str("");
			int rank(0);
			BOOST_FOREACH(const TimetableGenerator::AuthorizedLines::value_type& line, _authorizedLines)
			{
				authorized_lines_str += (rank > 0 ? ("," + lexical_cast<string>(line->getKey())) : lexical_cast<string>(line->getKey()));
				rank++;
			}
			set<AuthorizedLines>(authorized_lines_str);
		}



		void Timetable::computeStrAndSetAuthorizedPhysicalStops()
		{
			string authorized_ps_str("");
			int rank(0);
			BOOST_FOREACH(const TimetableGenerator::AuthorizedPhysicalStops::value_type& stop, _authorizedPhysicalStops)
			{
				authorized_ps_str += (rank > 0 ? ("," + lexical_cast<string>(stop->getKey())) : lexical_cast<string>(stop->getKey()));
				rank++;
			}
			set<AuthorizedPhysicalStops>(authorized_ps_str);
		}



		void Timetable::addRow( const TimetableRow& row )
		{
			if(row.getRank() > _rows.size()) throw synthese::Exception("Invalid rank in timetable");
			_rows.insert(_rows.begin() + row.getRank(), row);
		}



		void Timetable::clearRows()
		{
			_rows.clear();
			_rowGroups.clear();
		}



		bool Timetable::isGenerable() const
		{
			return
				_contentType == CONTAINER ||
				(get<BaseCalendar>().get_ptr() != NULL &&
				 get<BaseCalendar>()->isLimited())
			;
		}



		auto_ptr<TimetableGenerator> Timetable::getGenerator(
			const Env& env,
			optional<Calendar> mask
		) const {
			if(	!isGenerable() && !mask
			){
				throw ImpossibleGenerationException();
			}

			auto_ptr<TimetableGenerator> g(new TimetableGenerator(env));
			if(_contentType != CONTAINER)
			{
				// New rows definition
				if(!_rowGroups.empty())
				{
					g->setRowGroups(_rowGroups);
				}
				else // Old rows definition
				{
					g->setRows(_rows);
				}
				g->setBaseCalendar(mask ? *mask : get<BaseCalendar>()->getResult());
				g->setAuthorizedLines(_authorizedLines);
				g->setAuthorizedPhysicalStops(_authorizedPhysicalStops);
				g->setWaybackFilter(_wayBackFilter);
				g->setAutoIntermediateStops(_autoIntermediateStops);
				g->setCompression(get<Compression>());
				if(get<TransferTimetableBefore>())
				{
					g->setBeforeTransferTimetable(get<TransferTimetableBefore>()->getGenerator(env, mask));
				}
				if(get<TransferTimetableAfter>())
				{
					g->setAfterTransferTimetable(get<TransferTimetableAfter>()->getGenerator(env, mask));
				}
				g->setMergeColsWithSameTimetables(_mergeColsWithSameTimetables);
			}
			return g;
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
			computeStrAndSetAuthorizedLines();
		}



		void Timetable::removeAuthorizedLine( const pt::CommercialLine* line )
		{
			_authorizedLines.erase(line);
			computeStrAndSetAuthorizedLines();
		}



		void Timetable::addAuthorizedPhysicalStop( const pt::StopPoint* stop )
		{
			_authorizedPhysicalStops.insert(stop);
			computeStrAndSetAuthorizedPhysicalStops();
		}



		void Timetable::removeAuthorizedPhysicalStop( const pt::StopPoint* stop )
		{
			_authorizedPhysicalStops.erase(stop);
			computeStrAndSetAuthorizedPhysicalStops();
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
			if(get<TransferTimetableBefore>().get_ptr() == NULL)
			{
				return NULL;
			}
			return get<TransferTimetableBefore>()->getTransferTimetableBefore(depth - 1);
		}



		Timetable* Timetable::getTransferTimetableAfter( std::size_t depth /*= 0*/ )
		{
			if(depth == 0)
			{
				return this;
			}
			if(get<TransferTimetableAfter>().get_ptr() == NULL)
			{
				return NULL;
			}
			return get<TransferTimetableAfter>()->getTransferTimetableAfter(depth - 1);
		}


		const Timetable* Timetable::getTransferTimetableBefore( std::size_t depth /*= 0*/ ) const
		{
			if(depth == 0)
			{
				return this;
			}
			if(get<TransferTimetableBefore>().get_ptr() == NULL)
			{
				return NULL;
			}
			return get<TransferTimetableBefore>()->getTransferTimetableBefore(depth - 1);
		}



		const Timetable* Timetable::getTransferTimetableAfter( std::size_t depth /*= 0*/ ) const
		{
			if(depth == 0)
			{
				return this;
			}
			if(get<TransferTimetableAfter>().get_ptr() == NULL)
			{
				return NULL;
			}
			return get<TransferTimetableAfter>()->getTransferTimetableAfter(depth - 1);
		}


		std::size_t Timetable::getBeforeTransferTimetablesNumber() const
		{
			return get<TransferTimetableBefore>().get_ptr() == NULL ? 0 : (get<TransferTimetableBefore>()->getBeforeTransferTimetablesNumber() + 1);
		}



		std::size_t Timetable::getAfterTransferTimetablesNumber() const
		{
			return get<TransferTimetableAfter>().get_ptr() == NULL ? 0 : (get<TransferTimetableAfter>()->getAfterTransferTimetablesNumber() + 1);
		}



		void Timetable::removeRowGroup( TimetableRowGroup& rowGroup )
		{
			_rowGroups.erase(&rowGroup);
		}



		Timetable::ImpossibleGenerationException::ImpossibleGenerationException():
		synthese::Exception("Timetable generation is impossible.")
		{

		}



		void Timetable::addRowGroup( TimetableRowGroup& rowGroup )
		{
			_rowGroups.insert(&rowGroup);
		}



		void Timetable::addAdditionalParameters(
			util::ParametersMap& pm,
			std::string prefix
		) const	{
			// Common parameters
			pm.insert(DATA_GENERATOR_TYPE, GetTimetableTypeCode(getContentType()));
			pm.insert(DATA_TITLE, get<Title>());
			pm.insert(Request::PARAMETER_OBJECT_ID, getKey());
			
			// Base calendar
			if(get<BaseCalendar>())
			{
				pm.insert(DATA_CALENDAR_NAME, get<BaseCalendar>()->getName());
			}
		}



		std::string Timetable::GetTimetableTypeCode(
			Timetable::ContentType value
		){
			switch(value)
			{
			case Timetable::CONTAINER: return "container";
			case Timetable::CALENDAR: return "calendar";
			case Timetable::LINE_SCHEMA: return "line_schema";
			case Timetable::TABLE_SERVICES_IN_COLS: return "services_in_cols";
			case Timetable::TABLE_SERVICES_IN_ROWS: return "services_in_rows";
			case Timetable::TIMES_IN_COLS: return "times_in_cols";
			case Timetable::TIMES_IN_ROWS: return "times_in_rows";
			}
			assert(false);
			return string();
		}


		void Timetable::link(
			util::Env& env,
			bool withAlgorithmOptimizations /* false */
		){
			setContentType(static_cast<ContentType>(get<Format>()));
			vector<string> lines = Conversion::ToStringVector(get<AuthorizedLines>());
			clearAuthorizedLines();
			BOOST_FOREACH(const string& line, lines)
			{
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(line));
					addAuthorizedLine(Env::GetOfficialEnv().get<CommercialLine>(id).get());
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					Log::GetInstance().warn("Data corrupted in " + Timetable::TABLE_NAME + "/" + AuthorizedLines::FIELD.name);
				}
			}


			vector<string> pstops = Conversion::ToStringVector(get<AuthorizedPhysicalStops>());
			clearAuthorizedPhysicalStops();
			BOOST_FOREACH(const string& pstop, pstops)
			{
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(pstop));
					addAuthorizedPhysicalStop(Env::GetOfficialEnv().get<StopPoint>(id).get());
				}
				catch (ObjectNotFoundException<StopPoint>&)
				{
					Log::GetInstance().warn("Data corrupted in " + Timetable::TABLE_NAME + "/" + AuthorizedPhysicalStops::FIELD.name);
				}
			}

			clearRows();
			TimetableRowTableSync::SearchResult rows(
						TimetableRowTableSync::Search(env, getKey())
						);
			BOOST_FOREACH(const boost::shared_ptr<TimetableRow>& row, rows)
			{
				addRow(*row);
			}
			TimetableRowGroupTableSync::SearchResult rowGroups(
						TimetableRowGroupTableSync::Search(Env::GetOfficialEnv(), getKey())
						);
			BOOST_FOREACH(const boost::shared_ptr<TimetableRowGroup>& rowGroup, rowGroups)
			{
				addRowGroup(*rowGroup);
			}
		}


		void Timetable::unlink()
		{
		}
}	}
