
#ifndef SYNTHESE_HtmlFormInterfaceElement_H__
#define SYNTHESE_HtmlFormInterfaceElement_H__

#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace server
	{
		class HtmlFormInterfaceElement : public interfaces::ValueInterfaceElement
		{
			// List of parameters to store
			interfaces::ValueInterfaceElement* _name;
			interfaces::ValueInterfaceElement* _function_key;
			interfaces::ValueInterfaceElement* _function_parameters;
			interfaces::ValueInterfaceElement* _action_key;
			interfaces::ValueInterfaceElement* _action_parameters;
			bool								_with_action;

		public:
			/** Parameters parser.
			Parameters order :
				-# function name
				-# function parameters (query string format)
				-# action name (blank if no action to do)
				-# action parameters (query string format)
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			std::string getValue(const interfaces::ParametersVector& parameters, const void* rootObject = NULL, const server::Request* request = NULL) const;
			~HtmlFormInterfaceElement();
		};
	}
}

#endif // SYNTHESE_HtmlFormInterfaceElement_H__
