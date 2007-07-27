
/** PlainCharFilterValueInterfaceElement class header.
	@file PlainCharFilterValueInterfaceElement.h
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

#ifndef SYNTHESE_PlainCharFilterValueInterfaceElement_H__
#define SYNTHESE_PlainCharFilterValueInterfaceElement_H__

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
		/** PlainCharFilterValueInterfaceElement value interface element class.
			@ingroup m11Values refValues

			Usage :
			@code {{plain_char <param> <param>}} @endcode
			
			The parameters of the elements are :
				-# First parameter
				-# Second parameter

			The output is :
			(description)

			Example :
			@code {{plain_char P1 P2}}@endcode
			outputs :
			@code @endcode
		*/
		class PlainCharFilterValueInterfaceElement : public interfaces::ValueInterfaceElement
		{
		private:
			// Attributes
			boost::shared_ptr<interfaces::ValueInterfaceElement> _text;
			
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

#endif // SYNTHESE_PlainCharFilterValueInterfaceElement_H__
