
/** Timetable class header.
	@file Timetable.h

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

#ifndef SYNTHESE_timetables_Timetable_h__
#define SYNTHESE_timetables_Timetable_h__

#include "Object.hpp"

#include "Registry.h"
#include "TimetableGenerator.h"
#include "ImportableTemplate.hpp"
#include "Exception.h"
#include "StringField.hpp"
#include "CalendarTemplate.h"

#include <string>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}



	namespace timetables
	{
		class Timetable;

		FIELD_POINTER(Book, Timetable)
		FIELD_POINTER(BaseCalendar, calendar::CalendarTemplate)
		FIELD_INT(Format)
		FIELD_STRING(AuthorizedLines)
		FIELD_STRING(AuthorizedPhysicalStops)
		FIELD_POINTER(TransferTimetableBefore, timetables::Timetable)
		FIELD_POINTER(TransferTimetableAfter, timetables::Timetable)
		FIELD_BOOL(IgnoreEmptyRows)
		FIELD_BOOL(Compression)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Book),
			FIELD(Rank),
			FIELD(Title),
			FIELD(BaseCalendar),
			FIELD(Format),
			FIELD(AuthorizedLines),
			FIELD(AuthorizedPhysicalStops),
			FIELD(TransferTimetableBefore),
			FIELD(TransferTimetableAfter),
			FIELD(IgnoreEmptyRows),
			FIELD(Compression)
		> TimetableSchema;
	}



	namespace timetables
	{
		class TimetableRow;
		class TimetableRowGroup;

		/** Timetable class.
			Tableau d'indicateur papier, caractérisée par:
				- des gares (cGareIndicateurs)
				- des colonnes (cColonneIndicateurs)
				- un masque de circulation de base (JC)
				- Un titre
			@author Hugues Romain
			@date 2001-2010
			@ingroup m55
		*/
		class Timetable:
			public virtual Object<Timetable, TimetableSchema>,
			public virtual util::Registrable,
			public impex::ImportableTemplate<Timetable>
		{
		public:



			/// Chosen registry class.
			typedef util::Registry<Timetable>	Registry;

			typedef TimetableGenerator::Rows Rows;
			typedef TimetableGenerator::RowGroups RowGroups;

			class ImpossibleGenerationException:
				public synthese::Exception
			{
			public:
				ImpossibleGenerationException();
			};

			enum ContentType
			{
				CONTAINER = 0,
				TABLE_SERVICES_IN_COLS = 1,
				TABLE_SERVICES_IN_ROWS = 2,
				TIMES_IN_COLS = 3,
				TIMES_IN_ROWS = 4,
				LINE_SCHEMA = 6,
				CALENDAR = 7
			};

			static std::string GetFormatName(ContentType value);
			static std::string GetIcon(ContentType value);

			typedef std::vector<std::pair<boost::optional<ContentType>, std::string> > ContentTypesList;

			static ContentTypesList GetFormatsList();

			typedef boost::optional<Timetable&> Type;

			static const std::string DATA_GENERATOR_TYPE;
			static const std::string DATA_TITLE;
			static const std::string DATA_CALENDAR_NAME;

		private:
			//! @name Content
			//@{
				ContentType					_contentType;
				TimetableGenerator::AuthorizedLines			_authorizedLines;
				TimetableGenerator::AuthorizedPhysicalStops	_authorizedPhysicalStops;
				TimetableGenerator::Rows					_rows;
				TimetableGenerator::RowGroups				_rowGroups;
				boost::optional<bool>		_wayBackFilter;
				boost::optional<std::size_t> _autoIntermediateStops;
				bool _mergeColsWithSameTimetables;
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Converts timetable type into text code.
			/// @param value timetable type
			/// @return text code (empty if the content type is not valid)
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			static std::string GetTimetableTypeCode(Timetable::ContentType value);

		public:
			// Constructeur
			Timetable(util::RegistryKeyType id = 0);



			//! @name Setters
			//@{
				void setContentType(ContentType value){ _contentType = value; set<Format>(static_cast<size_t>(value)); }
				void setWaybackFilter(boost::optional<bool> value){ _wayBackFilter = value; }
				void setAutoIntermediateStops(boost::optional<std::size_t> value){ _autoIntermediateStops = value; }
				void setAuthorizedLines(const TimetableGenerator::AuthorizedLines& value){ _authorizedLines = value; }
				void setAuthorizedPhysicalStops(const TimetableGenerator::AuthorizedPhysicalStops& value){ _authorizedPhysicalStops = value; }
				void setRows(const Rows& value){ _rows = value; }
				void setRows(const RowGroups& value){ _rowGroups = value; }
				void setMergeColsWithSameTimetables(bool value){ _mergeColsWithSameTimetables = value; }
				void computeStrAndSetAuthorizedLines();
				void computeStrAndSetAuthorizedPhysicalStops();
			//@}

			//! @name Modifiers
			//@{
				void addAuthorizedLine(const pt::CommercialLine* line);
				void addAuthorizedPhysicalStop(const pt::StopPoint* stop);
				void addRow(const TimetableRow& row);
				void removeAuthorizedLine(const pt::CommercialLine* line);
				void removeAuthorizedPhysicalStop(const pt::StopPoint* stop);
				void clearRows();
				void clearAuthorizedLines();
				void clearAuthorizedPhysicalStops();
				void addRowGroup(TimetableRowGroup& rowGroup);
				void removeRowGroup(TimetableRowGroup& rowGroup);
			//@}

			//! @name Getters
			//@{
				const TimetableGenerator::AuthorizedLines&	getAuthorizedLines() const;
				const TimetableGenerator::AuthorizedPhysicalStops& getAuthorizedPhysicalStops() const;
				const Rows&				getRows()				const { return _rows; }
				const RowGroups&		getRowGroups()			const { return _rowGroups; }
				ContentType				getContentType()		const { return static_cast<ContentType>(get<Format>()); }
				boost::optional<bool>	getWaybackFilter() const { return _wayBackFilter; }
				bool getMergeColsWithSameTimetables() const { return _mergeColsWithSameTimetables; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Tests if the generator can be built.
				/// The test consists in :
				///		- check if the base calendar is defined
				///		- check if the base calendar is limited
				/// @return true if the generator can be built
				bool isGenerable() const;

				//////////////////////////////////////////////////////////////////////////
				/// Builds a time table generator according to the parameters of the
				/// object.
				/// @throws ImpossibleGenerationException if the base calendar template
				///		is not limited
				/// @param env Environment of the source data
				/// @return the generator
				std::auto_ptr<TimetableGenerator> getGenerator(
					const util::Env& env,
					boost::optional<calendar::Calendar> mask = boost::optional<calendar::Calendar>()
				)	const;



				Timetable* getTransferTimetableBefore(std::size_t depth);
				Timetable* getTransferTimetableAfter(std::size_t depth);
				const Timetable* getTransferTimetableBefore(std::size_t depth) const;
				const Timetable* getTransferTimetableAfter(std::size_t depth) const;
				std::size_t getBeforeTransferTimetablesNumber() const;
				std::size_t getAfterTransferTimetablesNumber() const;


				virtual void addAdditionalParameters(util::ParametersMap& pm, std::string prefix) const;
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_Timetable_h__
