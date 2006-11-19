
#ifndef SYNTHESE_IfThenElseInterfaceElement_H__
#define SYNTHESE_IfThenElseInterfaceElement_H__

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;

		class IfThenElseInterfaceElement : public LibraryInterfaceElement
		{
		private:
			ValueInterfaceElement* _criteria;
			LibraryInterfaceElement* _to_do_if_true;
			LibraryInterfaceElement* _to_do_if_false;

		public:
			~IfThenElseInterfaceElement();
			void display(std::ostream& stream, const ParametersVector& parameters, const void* object = NULL, const server::Request* request = NULL) const;
			void parse( const std::string& text);
		};

	}
}

#endif // SYNTHESE_IfThenElseInterfaceElement_H__
