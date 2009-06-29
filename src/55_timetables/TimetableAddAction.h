
/** TimetableAddAction class header.
	@file TimetableAddAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TimetableAddAction_H__
#define SYNTHESE_TimetableAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace timetables
	{
		class Timetable;

		/** Timetable creation action class.
			@ingroup m55Actions refActions
		*/
		class TimetableAddAction
			: public util::FactorableTemplate<server::Action, TimetableAddAction>
		{
		public:
			static const std::string PARAMETER_BOOK_ID;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_IS_BOOK;

		private:
			boost::shared_ptr<const Timetable>	_book;
			int									_rank;
			std::string							_title;
			bool								_isBook;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();

			void setBook(boost::shared_ptr<const Timetable> book);
			
			TimetableAddAction();
			
			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_TimetableAddAction_H__
