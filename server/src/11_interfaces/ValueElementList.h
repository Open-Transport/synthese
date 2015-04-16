
/** ValueElementList class header.
	@file ValueElementList.h

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

#ifndef SYNTHESE_ValueElementList_H__
#define SYNTHESE_ValueElementList_H__

#include <boost/shared_ptr.hpp>

#include "11_interfaces/Types.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace interfaces
	{
		class LibraryInterfaceElement;
		class InterfacePage;

		/** Value element list.
			@ingroup m11
		*/
		class ValueElementList
		{
		private:
			typedef std::deque<boost::shared_ptr<LibraryInterfaceElement> > ElementsList;

			ElementsList _elements;	//!< The list

			void parse(const std::string& text, const InterfacePage* page);

		public:

			/** Parsing a string for building the element list.
				@param text Text to parse. Must be on one line without the <tt>\n</tt> character.

				The string is made up of a suit of elements, separated by spaces :
				@code element1 element2 element3 ... elementn @endcode

				3 types of elements are available :
					-# single word elements : the word is directly saved as simple text (StaticValueInterfaceElement)
						@code text_to_save @endcode
						The word must not containt any space nor any : character. In this case, refer to the next type.
					-# multiple words elements : the text is saved as a StaticValueInterfaceElement object. The multiple word element is surrounded by braces.
						@code {text to save} @endcode
					-# recursive call of a value element to parse : The recursive call is surrounded with double braces. The first word is the ValueInterfaceElement to call when the display will be done, and the following ones are its parameters elements which are ValueInterfaceElement too. They are separated by spaces.
						@code {{valuetype param1 param2 {long text param 3} param4}} @endcode
						In this case, each parameter is reevaluated by a new recursion of this parse function.
						The value elements can be cascaded with double braces notation :
						@code {{valuetype param1 {valuetype2 param2.1 {{param 2.2}} param2.3} param3}} @endcode
						For calling a value interface element without parameter, surround it with double braces without having to put the : character
						@code {{valuetype}} @endcode

				Examples :
					-# Single word :
						- <tt>Hello</tt>
						- <tt>How_are_you_?_***</tt>
					-# Multiple words :
						- <tt>{Hello world}</tt>
						- <tt>{Title, blue}</tt>
						- <tt>{Name:}</tt>
					-# Recursive call :
						- <tt>{{username}}</tt>
						- <tt>{{param 12}}</tt>
						- <tt>{{stop 12 {{param:4}} text {long text}}}</tt>
						- <tt>{{field hidden name {{cityname {{param 6}} {{param 2}}}} {style="visibility:hidden"}}}</tt>
			*/
			ValueElementList(const std::string& text, const InterfacePage* page);

			/** Empty constructor.
			*/
			ValueElementList();


			boost::shared_ptr<interfaces::LibraryInterfaceElement> front();
			bool isEmpty() const;
			size_t size() const;

			/** Moves the elements from the provided Value Element List object to the current one.
				@param vel Object to read
				@param page Interface page of the elements of the current object
			*/
			void	takeFrom(ValueElementList& vel, const InterfacePage* page);

			/** Builds a parameters vector from the value element list.
				@param parameters Parameters to use when filling undefined parameter elements
				@return Totally defined parameters.
			*/
			const ParametersVector fillParameters(
				const ParametersVector& parameters
				, VariablesMap& vars
				, const void* object = NULL
				, const server::Request* request=NULL
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Adds an element to the list.
			/// @param element The element to add to the list.
			/// @author Hugues Romain
			/// @date 2010
			void push_back(boost::shared_ptr<LibraryInterfaceElement> element);
		};
	}
}

#endif // SYNTHESE_ValueElementList_H__
