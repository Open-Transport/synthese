
#ifndef SYNTHESE_VinciReportsAdminInterfaceElement_H__
#define SYNTHESE_VinciReportsAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciReportsAdminInterfaceElement Class.
			@ingroup m71
		*/
		class VinciReportsAdminInterfaceElement : public admin::AdminInterfaceElement
		{
		public:
			VinciReportsAdminInterfaceElement();
			std::string getTitle() const;
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciReportsAdminInterfaceElement_H__