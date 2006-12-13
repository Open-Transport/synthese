
#ifndef SYNTHESE_ParameterValueInterfaceElement_H__
#define SYNTHESE_ParameterValueInterfaceElement_H__


#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ParameterValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _rank;

		public:
			std::string getValue( const ParametersVector&, const void* object = NULL, const server::Request* request = NULL ) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_ParameterValueInterfaceElement_H__

