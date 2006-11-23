
#ifndef SYNTHESE_AdminPagesTreeInterfaceElement_H__
#define SYNTHESE_AdminPagesTreeInterfaceElement_H__

#include <string>
#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		/** Tree of admin pages links.
			@ingroup m32
		*/
		class AdminPagesTreeInterfaceElement : public interfaces::ValueInterfaceElement
		{
			// List of parameters to store
			//interfaces::ValueInterfaceElement* _parameter1;
			//...

			static std::string getSubPages(const std::string& page, const std::string& currentPage, const server::Request* request);

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);

			std::string getValue(const interfaces::ParametersVector&, const void* object = NULL, const server::Request* request = NULL) const;
			
			~AdminPagesTreeInterfaceElement();
		};
	}
}

#endif // SYNTHESE_AdminPagesTreeInterfaceElement_H__