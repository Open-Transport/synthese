
/** TimetableAddAction class header.
	@file TimetableAddAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TimetableAddAction_H__
#define SYNTHESE_TimetableAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace timetables
	{
		class Timetable;

		//////////////////////////////////////////////////////////////////////////
		/// Timetable creation action.
		/// @author Hugues Romain
		///	@ingroup m55Actions refActions
		/// From scratch creation :
		///	 - bi : id of the book which the new object must belong to
		///  - rk : rank of the new object
		///  - ti : name of the new object
		///  - ib : the new object is a book (true) or a timetable (false)
		///
		/// Creation from a template :
		///  - te : id of the object to copy
		///  - re : reverse the content when copying
		class TimetableAddAction
			: public util::FactorableTemplate<server::Action, TimetableAddAction>
		{
		public:
			static const std::string PARAMETER_BOOK_ID;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_IS_BOOK;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_REVERSE;

		private:
			boost::shared_ptr<const Timetable>	_book;
			size_t								_rank;
			std::string							_title;
			bool								_isBook;
			boost::shared_ptr<const Timetable>	_template;
			bool								_reverse;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			//! @name Setters
			//@{
				void setBook(boost::shared_ptr<const Timetable> value);
				void setTemplate(boost::shared_ptr<const Timetable> value){ _template = value; }
				void setReverse(bool value){ _reverse = value; }
			//@}

			TimetableAddAction();

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_TimetableAddAction_H__
