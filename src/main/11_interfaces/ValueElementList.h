
#ifndef SYNTHESE_ValueElementList_H__
#define SYNTHESE_ValueElementList_H__


#include "ValueInterfaceElement.h"
#include <deque>

namespace synthese
{
	namespace interfaces
	{
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


			void parse( const std::string& text );
			void copy(ValueElementList& vai);

			ValueInterfaceElement* front();


			/** Builds a parameters vector from the value element list.
			@param parameters Parameters to use when filling undefined parameter elements
			@return Totally defined parameters.
			*/
			const ParametersVector fillParameters( const ParametersVector& parameters ) const;
		};
	}
}

#endif // SYNTHESE_ValueElementList_H__
