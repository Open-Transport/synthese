
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
			@ingroup m11Library refLibrary
		*/
		class GotoInterfaceElement : public LibraryInterfaceElement
		{
		private:
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _label;

		public:
			void storeParameters(ValueElementList& vel);
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;
		};

	}
}

#endif // SYNTHESE_GotoInterfaceElement_H__

