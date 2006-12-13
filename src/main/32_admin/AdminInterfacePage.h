
#include "11_interfaces/InterfacePage.h"

#include "30_server/Site.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		/** Admin template page.
		@code admin @endcode
		*/
		class AdminInterfacePage : public interfaces::InterfacePage
		{
		public:

			/** Display of the admin page.
			@param stream Stream to write on
			@param pageKey Key of the AdminInterfaceElement to display
			@param parameters Parameters to transmit to the AdminInterfaceElement to display
			@param site Displayed site
			*/
			void display( std::ostream& stream
				, const AdminInterfaceElement* page
				, const uid objectId
				, const server::Request* request = NULL ) const;

		};
	}
}

