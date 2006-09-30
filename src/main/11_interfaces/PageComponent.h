#ifndef SYNTHESE_INTERFACES_PAGE_COMPONENT_H
#define SYNTHESE_INTERFACES_PAGE_COMPONENT_H

#include "DisplayableElement.h"


namespace synthese
{
    
namespace interfaces
{
	/** element which can produce a single display.
		@ingroup m11
	*/
	class PageComponent : public DisplayableElement
	{
	protected:

		/** Contructor.
			@param components Components of the page.
		*/
		PageComponent()
			: DisplayableElement() 
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
