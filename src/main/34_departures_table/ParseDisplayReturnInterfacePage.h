
/** ParseDisplayReturnInterfacePage class header.
	@file ParseDisplayReturnInterfacePage.h
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

#ifndef SYNTHESE_ParseDisplayReturnInterfacePage_H__
#define SYNTHESE_ParseDisplayReturnInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace departurestable
	{
		/** Interface Page Class to parse a string returned by a managed display screen.

			Usage :
			@code {{parse_display_return <text>}} @endcode
			
			The parameters of the elements are :
				-# The text to parse

			The output is :
				The parsed text (use | to separate fields)

			The fields must be ordered like this :
				-# message OK
				-# cpu OK
				-# cpu error code
				-# peripheral ok
				-# peripheral error code
				-# driver ok
				-# driver error code
				-# light ok
				-# light error code

			Example :
			@code {{parse_display_return 0230310620202020202020202020202003}}@endcode
			outputs :
			@code 1|1|0|1|0|1|0000|1|0000 @endcode

			@ingroup m34Pages refPages
		*/
		class ParseDisplayReturnInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, ParseDisplayReturnInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(std::ostream& stream, const std::string& text, interfaces::VariablesMap& variables, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_ParseDisplayReturnInterfacePage_H__
