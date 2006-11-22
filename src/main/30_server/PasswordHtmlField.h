
#ifndef SYNTHESE_PasswordHtmlField_H__
#define SYNTHESE_PasswordHtmlField_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class PasswordHtmlField : public interfaces::ValueInterfaceElement
		{
		public:
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
			~PasswordHtmlField();
		};
	}
}

#endif // SYNTHESE_PasswordHtmlField_H__