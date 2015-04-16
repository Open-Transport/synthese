
/** TimetableSetLineAction class header.
	@file TimetableSetLineAction.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_TimetableSetLineAction_H__
#define SYNTHESE_TimetableSetLineAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace timetables
	{
		class Timetable;

		/** TimetableSetLineAction action class.
			@ingroup m55Actions refActions
		*/
		class TimetableSetLineAction:
			public util::FactorableTemplate<server::Action, TimetableSetLineAction>
		{
		public:
			static const std::string PARAMETER_TIMETABLE_ID;
			static const std::string PARAMETER_LINE_ID;

		private:
			boost::shared_ptr<Timetable> _timetable;
			boost::shared_ptr<const pt::CommercialLine> _line;

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

			virtual bool isAuthorized(const server::Session* session) const;

			void setTimetable(boost::shared_ptr<Timetable> value);
			void setLine(boost::shared_ptr<const pt::CommercialLine> value);
		};
	}
}

#endif // SYNTHESE_TimetableSetLineAction_H__
