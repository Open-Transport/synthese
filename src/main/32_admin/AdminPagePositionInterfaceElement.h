
#ifndef SYNTHESE_AdminPagePositionInterfaceElement_H__
#define SYNTHESE_AdminPagePositionInterfaceElement_H__

#include <string>
#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		/** Position of current admin page in the tree.
			@ingroup m32
		*/
		class AdminPagePositionInterfaceElement : public interfaces::ValueInterfaceElement
		{
			// List of parameters to store
			//interfaces::ValueInterfaceElement* _parameter1;
			//...

			static std::string getUpPages(const AdminInterfaceElement* page);

		public:
			/** Controls and store the internals parameters.
			@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);

			std::string getValue(const interfaces::ParametersVector&, const void* object = NULL, const server::Request* request = NULL ) const;

			~AdminPagePositionInterfaceElement();
		};
	}
}

#endif // SYNTHESE_AdminPagePositionInterfaceElement_H__
