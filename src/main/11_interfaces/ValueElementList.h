
#ifndef SYNTHESE_ValueElementList_H__
#define SYNTHESE_ValueElementList_H__

#include <deque>

#include "11_interfaces/DisplayableElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		class ValueElementList
		{
			typedef std::deque<ValueInterfaceElement*> ElementsList;

			ElementsList _elements;	//!< The list

		public:

			/** ValueElementList Constructor.
			@param text Text to parse
			@author Hugues Romain
			@date 2006

			Parses the text to create the list.	
			*/
			ValueElementList(const std::string text = "");

			/** Copy constructor.
				the element list to copy will be empty after the copy
			*/
			ValueElementList(ValueElementList& vai);

			/** Destructor.
			@author Hugues
			@date 2006

			Deletes all the elements linked by the vector.
			*/
			~ValueElementList();


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
			void parse( const std::string& text );
			
			ValueInterfaceElement* front();
			bool isEmpty() const;
			size_t size() const;


			/** Builds a parameters vector from the value element list.
				@param parameters Parameters to use when filling undefined parameter elements
				@return Totally defined parameters.
			*/
			const ParametersVector fillParameters( const ParametersVector& parameters ) const;
		};
	}
}

#endif // SYNTHESE_ValueElementList_H__
