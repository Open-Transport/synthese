#ifndef SYNTHESE_INTERFACES_DISPLAYABLE_ELEMENT_H
#define SYNTHESE_INTERFACES_DISPLAYABLE_ELEMENT_H

#include <deque>

namespace synthese
{
	    
	namespace interfaces
	{

		class Site;
		typedef std::deque<std::string> ParametersVector;


		/** Abstract class specifying virtual methods for all types of output element, making up each interface page.
			@ingroup m11
		*/
		class DisplayableElement
		{
		protected:
			/** Constructor (do nothing).
			*/
			DisplayableElement() { }

		public:

			/** Generic display method.
				@param stream Stream to write the output in
				@param parameters Parameters vector
			*/
			virtual void display(std::ostream& stream, const ParametersVector& parameters, const void* rootObject = NULL, const Site* site = NULL) const = 0;
		};
	}
}

#endif