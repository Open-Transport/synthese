
#include "11_interfaces/ValueElementList.h"

#include "CommentInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void CommentInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		void CommentInterfaceElement::display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
		}

		CommentInterfaceElement::~CommentInterfaceElement()
		{
		}
	}
}

