
#ifndef SYNTHESE_VinciCustomerAdminInterfaceElement_H__
#define SYNTHESE_VinciCustomerAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciCustomerAdminInterfaceElement Class.
		@ingroup m71
		*/
		class VinciCustomerAdminInterfaceElement : public admin::AdminInterfaceElement
		{
		public:
			VinciCustomerAdminInterfaceElement();

			std::string getTitle() const;

			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciCustomerAdminInterfaceElement_H__