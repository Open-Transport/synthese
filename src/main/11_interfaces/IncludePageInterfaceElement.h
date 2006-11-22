
#ifndef SYNTHESE_IncludePageInterfaceElement_H__
#define SYNTHESE_IncludePageInterfaceElement_H__


#include "11_interfaces/LibraryInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"

namespace synthese
{
	namespace interfaces
	{
		class IncludePageInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueInterfaceElement*	_page_code;
			ValueElementList		_parameters;

		public:
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}

#endif // SYNTHESE_IncludePageInterfaceElement_H__
