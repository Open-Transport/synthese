
#ifndef SYNTHESE_IfThenElseInterfaceElement_H__
#define SYNTHESE_IfThenElseInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		/** Conditional value.
			@code if,<condition>,<to_return_if_condition_is_true>,<to_return_if_condition_is_false (default=0)> @endcode

			The conditional value can be used for differencing the output according to a condition :
			@code print if,{{param:2}},{the param 2 is ok},{the param 2 is ko} @endcode

			It can be used for branching purposes. In this case, do not forget to jump over the "else" bloc after the "then" one :
			@code goto if,{{param:2}},bloc1,bloc2
			
			line bloc1
			...
			goto end_of_bloc
			
			line bloc2
			...

			line end_of_bloc
			@endcode
		*/
		class IfThenElseInterfaceElement : public ValueInterfaceElement
		{
		private:
			ValueInterfaceElement* _criteria;
			ValueInterfaceElement* _to_return_if_true;
			ValueInterfaceElement* _to_return_if_false;

		public:
			~IfThenElseInterfaceElement();
			std::string getValue(const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			void storeParameters(ValueElementList& vel);
		};

	}
}

#endif // SYNTHESE_IfThenElseInterfaceElement_H__

