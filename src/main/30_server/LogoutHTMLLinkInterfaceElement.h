
#ifndef SYNTHESE_LogoutHTMLLinkInterfaceElement_H__
#define SYNTHESE_LogoutHTMLLinkInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class LogoutHTMLLinkInterfaceElement : public interfaces::ValueInterfaceElement
		{
			interfaces::ValueInterfaceElement* _redirectionURL;
			interfaces::ValueInterfaceElement* _page_key;
			interfaces::ValueInterfaceElement* _content;

		public:
			/** Parameters parser.
				Parameters order :
					-# Redirection URL after logout (empty if the following parameter is filled)
					-# Page key to redirect after logout (empty if URL is filled)
					-# Text to put in the link
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_LogoutHTMLLinkInterfaceElement_H__
