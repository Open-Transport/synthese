
#ifndef SYNTHESE_RedirectInterfacePage_H__
#define SYNTHESE_RedirectInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace server
	{
		/** Redirection interface page.
			
			Parameters :
				-# HTTP url to redirect
		*/
		class RedirectInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object :

				@param stream Stream to write on
				@param request Source request

				The redirection is made for launching the specified request
			*/
			void display(std::ostream& stream, const server::Request* request = NULL) const;


			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object :

				@param stream Stream to write on
				@param url URL to redirect after the action
				@param request Source request
			*/
			void display(std::ostream& stream, const std::string& url, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_RedirectInterfacePage_H__
