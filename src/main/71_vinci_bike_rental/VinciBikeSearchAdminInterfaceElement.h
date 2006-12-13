
#ifndef SYNTHESE_VinciBikeSearchAdminInterfaceElement_H__
#define SYNTHESE_VinciBikeSearchAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciBikeSearchAdminInterfaceElement Class.
			@ingroup m71
		*/
		class VinciBikeSearchAdminInterfaceElement : public admin::AdminInterfaceElement
		{
		public:
			VinciBikeSearchAdminInterfaceElement();
			std::string getTitle() const;
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciBikeSearchAdminInterfaceElement_H__
