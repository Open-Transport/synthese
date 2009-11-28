
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
#include "UId.h"
#include "Calendar.h"
#include "Registry.h"
#include "TimetableGenerator.h"

#include <string>

namespace synthese
{
	namespace util
	{
		class Env;
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

		private:
			// Variables
			bool					_mustBeginAPage;
			Rows					_rows;
			calendar::Calendar			_baseCalendar;
			std::string				_title;
			uid						_bookId;
			int						_rank;
			uid						_templateCalendarId;
			bool					_isBook;

		public:
			// Constructeur
			Timetable(util::RegistryKeyType id = UNKNOWN_VALUE);



			//! @name Setters
			//@{
				void setTitle(const std::string& title);
				void setMustBeginAPage(bool newVal);
				void setBaseCalendar(const calendar::Calendar& calendar);
				void setBookId(uid id);
				void setTemplateCalendarId(uid id);
				void setRank(int value);
				void setIsBook(bool value);
			//@}

			//! @name Modifiers
			//@{
				void addRow(const TimetableRow& row);
			//@}

			//! @name Getters
			//@{
				bool					getMustBeginAPage()		const;
				const calendar::Calendar&	getBaseCalendar()		const;
				const std::string&		getTitle()				const;
				const Rows&				getRows()				const;
				uid						getBookId()				const;
				uid						getTemplateCalendarId()	const;
				int						getRank()				const;
				bool					getIsBook()				const;
			//@}

			//! @name Queries
			//@{
				std::auto_ptr<TimetableGenerator> getGenerator(
					const util::Env& env
				)	const;
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_Timetable_h__
