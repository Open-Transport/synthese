
/** DurationInterfacePage class header.
	@file DurationInterfacePage.h

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

#ifndef SYNTHESE_DurationInterfacePage_H__
#define SYNTHESE_DurationInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		/** Duration interface page.
			@ingroup m11Pages refPages

			The duration interface page takes the following arguments :
				-# Duration in minutes
				-# Contained hours
				-# Minutes completing the hours

			The display can be "#1 min" or "#2 h #3 min" etc.
		*/
		class DurationInterfacePage : public util::FactorableTemplate<InterfacePage,DurationInterfacePage>
		{
		public:
			DurationInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.

				@param duration Duration to display
			*/
			void display(std::ostream& stream, int duration, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_DurationInterfacePage_H__
