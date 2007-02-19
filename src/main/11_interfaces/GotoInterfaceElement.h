
#ifndef SYNTHESE_GotoInterfaceElement_H__
#define SYNTHESE_GotoInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"
#include <string>

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		/** Goto interface element.
		*/
		class GotoInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueInterfaceElement* _label;

		public:
			void storeParameters(ValueElementList& vel);
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			const std::string getLabel(const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
		};

	}
}

#endif // SYNTHESE_GotoInterfaceElement_H__

