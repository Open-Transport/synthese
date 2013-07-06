
/** OpeningTime class implementation.
	@file OpeningTime.cpp

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

#include "OpeningTime.hpp"

namespace synthese
{
	using namespace analysis;

	CLASS_DEFINITION(OpeningTime, "t111_opening_times", 111)
	FIELD_DEFINITION_OF_OBJECT(OpeningTime, "opening_time_id", "opening_time_ids")

	FIELD_DEFINITION_OF_TYPE(Time, "time", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(IsArrival, "is_arrival", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(OpeningDays, "opening_days", SQL_TEXT)

	namespace analysis
	{
		OpeningTime::OpeningTime(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<OpeningTime, OpeningTimeSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(road::PublicPlace),
					FIELD_DEFAULT_CONSTRUCTOR(Time),
					FIELD_VALUE_CONSTRUCTOR(IsArrival, false),
					FIELD_DEFAULT_CONSTRUCTOR(OpeningDays)
			)	)
		{}



		void OpeningTime::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			
		}



		void OpeningTime::unlink()
		{

		}
}	}
