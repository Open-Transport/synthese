
#ifndef SYNTHESE_VinciBikeAdminInterfaceElement_H__
#define SYNTHESE_VinciBikeAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciBikeSearchAdminInterfaceElement Class.
			@ingroup m71
		*/
		class VinciBikeAdminInterfaceElement : public admin::AdminInterfaceElement
		{
			std::string _bikeNumber;

		public:
			VinciBikeAdminInterfaceElement ();
			std::string getTitle() const;
			void display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}
#endif // SYNTHESE_VinciBikeAdminInterfaceElement_H__

