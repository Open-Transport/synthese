
/** TrafficForecast class header.
	@file TrafficForecast.hpp

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

#ifndef SYNTHESE_analysis_TrafficForecast_H
#define SYNTHESE_analysis_TrafficForecast_H

#include "Object.hpp"

#include "NumericField.hpp"
#include "OpeningTime.hpp"
#include "PointerField.hpp"
#include "PublicPlace.h"
#include "Registry.h"
#include "SchemaMacros.hpp"
#include "TimeField.hpp"

namespace synthese
{
	namespace analysis
	{
		FIELD_POINTER(Departure, util::Registrable)
		FIELD_POINTER(Arrival, util::Registrable)
		FIELD_POINTER(Days, calendar::CalendarTemplate)
		FIELD_INT(Traffic)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Departure),
			FIELD(Arrival),
			FIELD(OpeningTime),
			FIELD(Traffic)
		> TrafficForecastSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Traffic forecast.
		///	@ingroup m60
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		class TrafficForecast:
			public Object<TrafficForecast, TrafficForecastSchema>
		{
		public:
			TrafficForecast(
				util::RegistryKeyType id = 0
			);

			/// @name Getters
			//@{
			//@}

			/// @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			/// @name Services
			//@{
			//@}
		};
}	}

#endif
