#ifndef SYNTHESE_INTERFACES_DISPLAYABLE_ELEMENT_H
#define SYNTHESE_INTERFACES_DISPLAYABLE_ELEMENT_H

#include <vector>

#include "ValueElement.h"

namespace synthese
{
    
namespace interfaces
{

	typedef vector<ValueElement> ParametersVector;


	/** Abstract class specifying virtual methods for all types of output element, making up each interface page.
		@ingroup m11
	*/
	class DisplayableElement
	{
	protected:
		/** Constructor (do nothing).
		*/
		DisplayElement() { }

	public:

		/** Generic display method.
			@param stream Stream to write the output in
			@param parameters Parameters vector
			@return Name of the next line to display (empty = next line)
		*/
		std::string display(std::ostream& stream, const ParametersVector& parameters) = 0;
	}
}
}

#endif