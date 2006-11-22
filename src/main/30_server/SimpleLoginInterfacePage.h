
#ifndef SYNTHESE_SimpleLoginInterfacePage_H__
#define SYNTHESE_SimpleLoginInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;

		class SimpleLoginInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(std::ostream& stream, const void* object = NULL, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_SimpleLoginInterfacePage_H__
 