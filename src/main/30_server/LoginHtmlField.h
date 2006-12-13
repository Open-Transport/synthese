
#ifndef SYNTHESE_LoginHtmlField_H__
#define SYNTHESE_LoginHtmlField_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class LoginHtmlField : public interfaces::ValueInterfaceElement
		{

		public:
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
			~LoginHtmlField();
		};
	}
}

#endif // SYNTHESE_LoginHtmlField_H__
