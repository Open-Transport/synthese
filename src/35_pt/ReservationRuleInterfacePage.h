
/** ReservationRuleInterfacePage class header.
	@file ReservationRuleInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_ReservationRuleInterfacePage_H__
#define SYNTHESE_ReservationRuleInterfacePage_H__

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace graph
	{
		class Journey;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.11 Interface : display of the reservation rule of a journey.
		///	@ingroup m35Pages refPages
		///
		///	Available data :
		///	 - is_optional : Reservation is optional 1|0
		///	 - is_compulsory : Reservation is compulsory 1|0
		///	 - delay : Reservation delay (0 if reservation impossible)
		///	 - deadline : Reservation deadline (-1/-1/-1 if reservation impossible)
		class ReservationRuleInterfacePage
		{
		public:
			static const std::string DATA_IS_OPTIONAL;
			static const std::string DATA_IS_COMPULSORY;
			static const std::string DATA_DELAY;
			static const std::string DATA_DEADLINE;

			//////////////////////////////////////////////////////////////////////////
			/// Displays a reservation rule.
			/// @param stream stream to display on
			/// @param page page to use for the display. If null, no display is done.
			/// @param dateTimePage page to use for the deadline display. If null, default output.
			/// @param request current request
			/// @param journey the journey to read
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				boost::shared_ptr<const cms::Webpage> dateTimePage,
				const server::Request& request,
				const util::ParametersMap& templateParametersMap,
				const graph::Journey& journey
			);
		};
	}
}

#endif // SYNTHESE_ReservationRuleInterfacePage_H__
