
/** IfThenElseInterfaceElement class header.
	@file IfThenElseInterfaceElement.h

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

#ifndef SYNTHESE_IfThenElseInterfaceElement_H__
#define SYNTHESE_IfThenElseInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		/** Conditional value.
			@code if,<condition>,<to_return_if_condition_is_true>,<to_return_if_condition_is_false (default=0)> @endcode

			The conditional value can be used for differencing the output according to a condition :
			@code print if,{{param:2}},{the param 2 is ok},{the param 2 is ko} @endcode

			It can be used for branching purposes. In this case, do not forget to jump over the "else" bloc after the "then" one :
			@code goto if,{{param:2}},bloc1,bloc2
			
			line bloc1
			...
			goto end_of_bloc
			
			line bloc2
			...

			line end_of_bloc
			@endcode
		*/
		class IfThenElseInterfaceElement : public ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _criteria;
			ValueInterfaceElement* _to_return_if_true;
			ValueInterfaceElement* _to_return_if_false;

		public:
			~IfThenElseInterfaceElement();
			std::string getValue(const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object = NULL, const server::Request* request = NULL) const;
			void storeParameters(ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_IfThenElseInterfaceElement_H__

