
/** LoginInterfacePage class header.
	@file LoginInterfacePage.h
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

#ifndef SYNTHESE_LoginInterfacePage_H__
#define SYNTHESE_LoginInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace server
	{
		class Function;

		/** LoginInterfacePage Interface Page Class.
			@ingroup m15Pages refPages

			Parameters :
				- 0 : function key if login succeeds
				- 1 : function parameters (query string format)
				- 2 : title
		*/
		class LoginInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, LoginInterfacePage>
		{
		public:
			static const std::string DATA_FUNCTION_KEY_IF_SUCCESS;
			static const std::string DATA_FUNCTION_PARAMETER;
			static const std::string DATA_TITLE;

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, const Function* functionIfSucceed
				, const std::string& title
				, interfaces::VariablesMap& variables
				, const Request* request = NULL
			) const;

			LoginInterfacePage();
		};
	}
}

#endif // SYNTHESE_LoginInterfacePage_H__
