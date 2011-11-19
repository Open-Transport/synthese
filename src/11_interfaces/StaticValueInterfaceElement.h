
/** StaticValueInterfaceElement class header.
	@file StaticValueInterfaceElement.h

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

#ifndef SYNTHESE_StaticValueInterfaceElement_H__
#define SYNTHESE_StaticValueInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		/** Special value interface element containing text (stops the recursion of value elements parsing).
			As this class is child of ValueInterfaceElement, it can not be join in the factory, because of its constructor which requires an argument to run.
			The parse method is implemented for technical reasons only, but does nothing.

			To create a static value interface element, just put the text if it does not contain any space, or put { } or [ ] around it.

			Examples :

			@code
element
this_is_also_an_element
{element with spaces}
[element with spaces]
{element with spaces containing a [ or a ] character}
[element with spaces containing a { or a } character]
			@endcode

			@note If a value element contains both { and [ elements, then separate it into smaller elements, and join it with the concatenation element :
			@code {{ {element with spaces and [ character} [ element with space and { character ] OK !}} @endcode
			will output
			@code element with spaces and [ character element with space and { character OK! @endcode

			@ingroup m11Library refLibrary
		*/
		class StaticValueInterfaceElement
			: public util::FactorableTemplate<LibraryInterfaceElement,StaticValueInterfaceElement>
		{
		private:
			const std::string _value;

		public:
			StaticValueInterfaceElement();
			StaticValueInterfaceElement(const std::string& value);
			std::string	display(
				std::ostream&
				, const ParametersVector&, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL ) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_StaticValueInterfaceElement_H__
