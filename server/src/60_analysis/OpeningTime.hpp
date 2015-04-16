
/** Opening time class header.
	@file OpeningTime.hpp

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

#ifndef SYNTHESE_analysis_OpeningTime_H
#define SYNTHESE_analysis_OpeningTime_H

#include "Object.hpp"

#include "CalendarTemplate.h"
#include "NumericField.hpp"
#include "PointerField.hpp"
#include "PublicPlace.h"
#include "Registry.h"
#include "SchemaMacros.hpp"
#include "TimeField.hpp"

namespace synthese
{
	namespace analysis
	{
		FIELD_TIME(Time)
		FIELD_BOOL(IsArrival)
		FIELD_POINTER(OpeningDays, calendar::CalendarTemplate)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(road::PublicPlace),
			FIELD(Time),
			FIELD(IsArrival),
			FIELD(OpeningDays)
		> OpeningTimeSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Public place.
		///	@ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Public place model :
		/// @image html uml_public_place.png
		///
		class OpeningTime:
			public Object<OpeningTime, OpeningTimeSchema>
		{
		public:
			OpeningTime(
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
