
#ifndef SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__
#define SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciCustomerSearchAdminInterfaceElement Class.
			@ingroup m71
		*/
		class VinciCustomerSearchAdminInterfaceElement : public admin::AdminInterfaceElement
		{
			static const std::string PARAM_SEARCH_NAME;
			static const std::string PARAM_SEARCH_SURNAME;

		public:
			VinciCustomerSearchAdminInterfaceElement();
			std::string getTitle() const;
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__
