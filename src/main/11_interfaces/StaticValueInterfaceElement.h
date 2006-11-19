
#ifndef SYNTHESE_StaticValueInterfaceElement_H__
#define SYNTHESE_StaticValueInterfaceElement_H__


#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class StaticValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			std::string _value;

		public:
			const std::string& getValue( const ParametersVector&, const void* object = NULL, const server::Request* request = NULL ) const;
			void parse( const std::string& text);

		};
	}
}
#endif // SYNTHESE_StaticValueInterfaceElement_H__
