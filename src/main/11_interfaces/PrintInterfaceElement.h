
#ifndef SYNTHESE_PrintInterfaceElement_H__
#define SYNTHESE_PrintInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		class PrintInterfaceElement : public interfaces::LibraryInterfaceElement
		{
			ValueInterfaceElement* _toBePrinted;

		public:
			/** Parameters parser.
				The parser copies the ValueElementList as is.
			*/
			void storeParameters(ValueElementList& vel);
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
			~PrintInterfaceElement();
		};
	}
}

#endif // SYNTHESE_PrintInterfaceElement_H__