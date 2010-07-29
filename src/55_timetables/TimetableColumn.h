
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

#include "Calendar.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;
		class StopPoint;
	}

	namespace pt
	{
		class JourneyPattern;
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
			typedef std::vector<std::pair<const pt::StopPoint*, boost::posix_time::time_duration> > Content;
			enum tTypeOD
			{
				Texte,
				Terminus,
				Indetermine
			};

		private:
			// Variables
			Content		_content;
			calendar::Calendar				_calendar;
			boost::shared_ptr<TimetableWarning>			_warning;
			const pt::JourneyPattern*				_line;
			tTypeOD							_originType;
			tTypeOD							_destinationType;

		public:
			// Constructeur
			TimetableColumn(
				const TimetableGenerator& generator,
				const pt::SchedulesBasedService& service
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
				void	setWarning(boost::shared_ptr<TimetableWarning> value) { _warning = value; }
			//@}

			//! @name Getters
			//@{
				const calendar::Calendar&				getCalendar()			const { return _calendar; }
				const Content&							getContent()			const { return _content; }
				const pt::JourneyPattern*						getLine()				const { return _line; }
				tTypeOD									getOriginType()			const { return _originType; }
				tTypeOD									getDestinationType()	const { return _destinationType; }
				boost::shared_ptr<TimetableWarning>					getWarning()			const { return _warning; }
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableColumn_h__
