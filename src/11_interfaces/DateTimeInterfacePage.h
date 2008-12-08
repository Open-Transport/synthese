
/** DateTimeInterfacePage class header.
	@file DateTimeInterfacePage.h
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

#ifndef SYNTHESE_DateTimeInterfacePage_H__
#define SYNTHESE_DateTimeInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace time
	{
		class DateTime;
		class Date;
	}

	namespace interfaces
	{
		/** DateTimeInterfacePage Interface Page Class.
			@ingroup m11Pages refPages

			Params :
			 - year
			 - month
			 - day
			 - hours (TIME_UNKNOWN = do not display hour)
			 - minutes
			 - day of week  : 0 = sunday, 1 = monday, ..., 6 = saturday 

			Object :
			 - DateTime
		*/
		class DateTimeInterfacePage : public util::FactorableTemplate<InterfacePage,DateTimeInterfacePage>
		{
		public:
			DateTimeInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const time::DateTime& dateTime
				, const server::Request* request = NULL
			) const;

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const time::Date& date
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_DateTimeInterfacePage_H__
