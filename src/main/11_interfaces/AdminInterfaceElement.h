#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include "PageComponent.h"


namespace synthese
{
    
namespace interfaces
{
	/** element which can produce a single display.
		@ingroup m11
		
		@todo integrate the mechanism of factorable class
		@todo integrate static tree of admin classes
		
	*/
	class AdminInterfaceElement : public LibraryInterfaceElement
	{
	protected:

		/** Contructor.
		*/
		LibraryInterfaceElement()
			: PageComponent() 
		{ }

	public:

		/** Display method.
			@param stream Stream to write the output in
			@param parameters Parameters vector
			@return Name of the next line to display (empty = next line)
		*/
		std::string display(std::ostream& stream, const ParametersVector& parameters) = 0;


	}
}
}

#endif