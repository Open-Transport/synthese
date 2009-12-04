
/** TimetableColumn class header.
	@file TimetableColumn.h

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

#ifndef SYNTHESE_timetables_TimetableColumn_h__
#define SYNTHESE_timetables_TimetableColumn_h__

#include <vector>

#include "Schedule.h"

#include "55_timetables/types.h"
#include "Calendar.h"

namespace synthese
{
	namespace env
	{
		class Line;
		class NonPermanentService;
		class PhysicalStop;
	}

	namespace timetables
	{
		class TimetableGenerator;
		class TimetableWarning;

		/** TimetableColumn class.
			Colonne de tableau d'indicateur papier

			 caractérisée par:
			   - des pointeurs vers les cArrets décrits
			   - le code Postscript de la colonne
			   - le nombre de lignes de la colonne
			@author Hugues Romain
			@date 2001
			@ingroup m55
		*/
		class TimetableColumn
		{
		public:
			typedef std::vector<std::pair<const env::PhysicalStop*, time::Schedule> > Content;

		private:
			// Variables
			Content		_content;
			calendar::Calendar				_calendar;
			const TimetableWarning*			_warning;
			const env::Line*				_line;
			tTypeOD							_originType;
			tTypeOD							_destinationType;

		public:
			// Constructeur
			TimetableColumn(
				const TimetableGenerator& generator,
				const env::NonPermanentService& service
			);

			//! @name Queries
			//@{
				int		operator <= (const TimetableColumn& op) const;
				bool	operator == (const TimetableColumn& op) const;
			//@}

			//! @name Modifiers
			//@{
				void	merge(const TimetableColumn& col);
			//@}

			//! @name Setters
			//@{
				void	setWarning(const TimetableWarning* it);
			//@}

			//! @name Getters
			//@{
				const calendar::Calendar&				getCalendar()			const;
				const Content&							getContent()			const;
				const env::Line*						getLine()				const;
				tTypeOD									getOriginType()			const;
				tTypeOD									getDestinationType()	const;
				const TimetableWarning*					getWarning()			const;
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableColumn_h__
