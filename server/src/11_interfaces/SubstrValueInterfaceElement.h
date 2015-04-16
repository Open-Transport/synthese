
/** SubstrValueInterfaceElement class header.
	@file SubstrValueInterfaceElement.h
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

#ifndef SYNTHESE_SubstrValueInterfaceElement_H__
#define SYNTHESE_SubstrValueInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueElementList;

		/** SubstrValueInterfaceElement value interface element class.
			@ingroup m11Library refLibrary

			Usage :
			@code {{substr <param> <param>}} @endcode

			The parameters of the elements are :
				-# The string to read
				-# The position of the first character to return
				-# The length of the string to return

			The output is :
				The truncated string corresponding to the parameters

			Example :
			@code {{substr text 2 1}}@endcode
			outputs :
			@code x @endcode
		*/
		class SubstrValueInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, SubstrValueInterfaceElement>
		{
		private:
			// Attributes
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _string;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _start;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _length;

		public:
			/** Evaluates the context and builds the output.
				@param parameters Current execution parameters
				@param variables Current interface variables
				@param object Current displayed object
				@param request Current request
				@return Generated string output
			*/
			std::string display(
				std::ostream&
				, const interfaces::ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;

			/** Parameters storage and quantity verification.
				@param vel Value Elements List : the parameters
				@throw InterfacePageException if the parameters number is incorrect
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_SubstrValueInterfaceElement_H__
