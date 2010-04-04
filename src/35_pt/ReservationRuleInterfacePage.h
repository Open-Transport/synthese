
/** ReservationRuleInterfacePage class header.
	@file ReservationRuleInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_ReservationRuleInterfacePage_H__
#define SYNTHESE_ReservationRuleInterfacePage_H__

#include "InterfacePage.h"

#include "FactorableTemplate.h"

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

	namespace pt
	{
		/** ReservationRuleInterfacePage Interface Page Class.
			@ingroup m35Pages refPages

			Available data :
			 - is_optional : Reservation is optional 1|0
			 - is_compulsory : Reservation is compulsory 1|0
			 - delay : Reservation delay (0 if reservation impossible)
			 - deadline : Reservation deadline (-1/-1/-1 if reservation impossible)
		*/
		class ReservationRuleInterfacePage:
			public util::FactorableTemplate<interfaces::InterfacePage,ReservationRuleInterfacePage>
		{
		public:
			static const std::string DATA_IS_OPTIONAL;
			static const std::string DATA_IS_COMPULSORY;
			static const std::string DATA_DELAY;
			static const std::string DATA_DEADLINE;

			ReservationRuleInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const graph::Journey& journey
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_ReservationRuleInterfacePage_H__
