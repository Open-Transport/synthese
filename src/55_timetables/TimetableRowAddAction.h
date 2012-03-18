
/** TimetableRowAddAction class header.
	@file TimetableRowAddAction.h
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

#ifndef SYNTHESE_TimetableRowAddAction_H__
#define SYNTHESE_TimetableRowAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace timetables
	{
		class Timetable;

		/** TimetableRowAddAction action class.
			@ingroup m55Actions refActions
		*/
		class TimetableRowAddAction
			: public util::FactorableTemplate<server::Action, TimetableRowAddAction>
		{
		public:
			static const std::string PARAMETER_TIMETABLE_ID;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;
			static const std::string PARAMETER_IS_DEPARTURE;
			static const std::string PARAMETER_IS_ARRIVAL;
			static const std::string PARAMETER_IS_COMPULSORY;
			static const std::string PARAMETER_IS_SUFFICIENT;
			static const std::string PARAMETER_IS_DISPLAYED;

		private:
			boost::shared_ptr<const Timetable>		_timetable;
			size_t									_rank;
			const pt::StopArea*	_place;
			bool												_isDeparture;
			bool												_isArrival;
			bool												_isCompulsory;
			bool												_isSufficient;
			bool												_isDisplayed;


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

			TimetableRowAddAction();

			void setTimetable(boost::shared_ptr<const Timetable> timetable);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_TimetableRowAddAction_H__
