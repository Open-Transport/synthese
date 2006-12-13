
#ifndef SYNTHESE_CommentInterfaceElement_H__
#define SYNTHESE_CommentInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		class CommentInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param rootObject Object to read at the display
				@param request Source request
			*/
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
			~CommentInterfaceElement();
		};
	}
}

#endif // SYNTHESE_CommentInterfaceElement_H__
