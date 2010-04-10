
/** Timetable class header.
	@file Timetable.h

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

#ifndef SYNTHESE_timetables_Timetable_h__
#define SYNTHESE_timetables_Timetable_h__

#include "Registrable.h"
#include "Registry.h"
#include "TimetableGenerator.h"
#include "Exception.h"

#include <string>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace interfaces
	{
		class Interface;
	}
	
	namespace timetables
	{
		class TimetableRow;
		
		/** Timetable class.
			Tableau d'indicateur papier, caractérisée par:
				- des gares (cGareIndicateurs)
				- des colonnes (cColonneIndicateurs)
				- un masque de circulation de base (JC)
				- Un titre
			@author Hugues Romain
			@date 2001
			@ingroup m55
		*/
		class Timetable
		:	public virtual util::Registrable
		{
		public:
			typedef std::vector<TimetableRow>	Rows;
						
			/// Chosen registry class.
			typedef util::Registry<Timetable>	Registry;

			class ImpossibleGenerationException:
				public util::Exception
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

		private:
			//! @name Position
			//@{
				util::RegistryKeyType	_bookId;
				int						_rank;
			//@}

			//! @name Content
			//@{
				ContentType					_contentType;
				TimetableGenerator::AuthorizedLines			_authorizedLines;
				TimetableGenerator::AuthorizedPhysicalStops	_authorizedPhysicalStops;
				Rows					_rows;
				const calendar::CalendarTemplate*			_baseCalendar;
				std::string				_title;
			//@}

			//! @name Appearance
			//@{
				const interfaces::Interface*	_interface;
			//@}

		public:
			// Constructeur
			Timetable(util::RegistryKeyType id = 0);



			//! @name Setters
			//@{
				void addAuthorizedLine(const pt::CommercialLine* line);
				void removeAuthorizedLine(const pt::CommercialLine* line);
				void clearAuthorizedLines();
				void addAuthorizedPhysicalStop(const pt::PhysicalStop* stop);
				void removeAuthorizedPhysicalStop(const pt::PhysicalStop* stop);
				void clearAuthorizedPhysicalStops();
				void setBookId(util::RegistryKeyType value) { _bookId = value; }
				void setTitle(const std::string& title);
				void setBaseCalendar(const calendar::CalendarTemplate* calendar);
				void setRank(int value);
				void setContentType(ContentType value);
				void setInterface(const interfaces::Interface* value);
			//@}

			//! @name Modifiers
			//@{
				void addRow(const TimetableRow& row);
			//@}

			//! @name Getters
			//@{
				const TimetableGenerator::AuthorizedLines&	getAuthorizedLines() const;
				const TimetableGenerator::AuthorizedPhysicalStops& getAuthorizedPhysicalStops() const;
				const calendar::CalendarTemplate*	getBaseCalendar()		const;
				const std::string&		getTitle()				const;
				const Rows&				getRows()				const;
				util::RegistryKeyType	getBookId()				const { return _bookId; }
				int						getRank()				const;
				ContentType				getContentType()		const;
				const interfaces::Interface* getInterface()		const;
			//@}

			//! @name Queries
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
					const util::Env& env
				)	const;

				void generate(
					std::ostream& stream
				);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_Timetable_h__
