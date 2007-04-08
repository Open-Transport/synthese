
/** ConcatenateValueInterfaceElement class header.
	@file ConcatenateValueInterfaceElement.h

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

#ifndef SYNTHESE_ConcatenateValueInterfaceElement_H__
#define SYNTHESE_ConcatenateValueInterfaceElement_H__

#include <boost/shared_ptr.hpp>

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;
	}
	namespace interfaces
	{
		/** ConcatenateValueInterfaceElement value interface element class.
			@ingroup m11Values refValues

			Usage :
			@code {{$ <param1> <param2> ... <paramn>}} @endcode
			
			The output is the concatenation of each param.
			@warning no space will be inserted between the parameters.

			Example :
			@code {{$ This { is } an [ interesting way of coding] ! {{param 1}}}} @endcode
			with the first execution parameter containing the ? character, it outputs :
			@code This is an interesting way of coding!? @endcode
		*/
		class ConcatenateValueInterfaceElement : public interfaces::ValueInterfaceElement
		{
		private:
			std::vector<boost::shared_ptr<interfaces::ValueInterfaceElement> > _parameters; // Parameters storage
			
		public:
			/** Evaluates the context and builds the output.
				@param parameters Current execution parameters
				@param variables Current interface variables
				@param object Current displayed object
				@param request Current request
				@return Generated string output
			*/
			std::string getValue(const interfaces::ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;
			
			/** Parameters storage and quantity verification.
				@param vel Value Elements List : the parameters
				@throw InterfacePageException if the parameters number is incorrect
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_ConcatenateValueInterfaceElement_H__
