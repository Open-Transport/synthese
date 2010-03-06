
/** RealTimeUpdateScreenServiceInterfacePage class header.
	@file RealTimeUpdateScreenServiceInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_RealTimeUpdateScreenServiceInterfacePage_H__
#define SYNTHESE_RealTimeUpdateScreenServiceInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace env
	{
		class LineStop;
	}

	namespace pt
	{
		class ScheduledService;

		/** RealTimeUpdateScreenServiceInterfacePage Interface Page Class.
			@ingroup m35Pages refPages
			@author Hugues
			@date 2009

			@code real_time_service @endcode

			Parameters :
				- 0 : Place full name
				- 1 : Line class
				- 2 : Line short name
				- 3 : Line picture
				- 4 : Service number
				- 5 : Destination long name
				- 6 : Departure physical stop
				- 7 : Rolling stock ID
				- 8 : Theoretical schedule
				- 9 : Real time schedule
				- 10 : Delay
				- 11 : Delay update action URL
				- 12 : Quai update action URL

			Object : ScheduledService
		*/
		class RealTimeUpdateScreenServiceInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, RealTimeUpdateScreenServiceInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const ScheduledService& service,
				const env::LineStop& lineStop,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			RealTimeUpdateScreenServiceInterfacePage();
		};
	}
}

#endif // SYNTHESE_RealTimeUpdateScreenServiceInterfacePage_H__
