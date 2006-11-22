
#ifndef SYNTHESE_StaticValueInterfaceElement_H__
#define SYNTHESE_StaticValueInterfaceElement_H__


#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		/** Special value interface element containing text (stops the recursion of value elements parsing).
			As this class is child of ValueInterfaceElement, it can not be join in the factory, because of its constructor which requires an argument to run.
			The parse method is implemented for technical reasons only, but does nothing.
		*/
		class StaticValueInterfaceElement : public ValueInterfaceElement
		{
		private:
			const std::string _value;

		public:
			StaticValueInterfaceElement(const std::string& value);
			std::string	getValue( const ParametersVector&, const void* object = NULL, const server::Request* request = NULL ) const;
			void storeParameters(ValueElementList& vel);
		};
	}
}
#endif // SYNTHESE_StaticValueInterfaceElement_H__
