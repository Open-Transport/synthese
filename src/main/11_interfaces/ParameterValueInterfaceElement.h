
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
			int _rank;

		public:
			const std::string& getValue( const ParametersVector&, const void* object = NULL, const server::Request* request = NULL ) const;
			void parse( const std::string& text);
		};
	}
}
#endif // SYNTHESE_ParameterValueInterfaceElement_H__
