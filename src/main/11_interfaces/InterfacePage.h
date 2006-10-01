#ifndef SYNTHESE_INTERFACES_PAGE_H
#define SYNTHESE_INTERFACES_PAGE_H

#include <vector>
#include <map>

#include "DisplayableElement.h"


namespace synthese
{
    
namespace interfaces
{

	class PageComponent;

	/** Definition of a page, coming from the database. Page are elements of an interface.
		@ingroup m11
	*/
	class InterfacePage : public DisplayableElement
	{
	public:
		typedef vector<PageComponent*> PageComponentsVector;
		typedef map<string, int> LineLabelsMap;

	private:
		const PageComponentsVector _components;
	
	public:

		/** Contructor.
			@param components Components of the page.
		*/
		DisplayableElement(const PageComponentsVector& components)
			: _components(components)
			, DisplayableElement() 
		{ }

		/** Display method.
			@param stream Stream to write the output in
			@param parameters Parameters vector
			@return Name of the next line to display (empty = next line)
		*/
		std::string display(std::ostream& stream, const ParametersVector& parameters);


	}
}
}

#endif
