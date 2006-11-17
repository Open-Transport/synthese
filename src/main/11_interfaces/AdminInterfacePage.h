
#include "InterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		class AdminInterfaceElement;

		/** Admin template page.
		@code admin @endcode
		*/
		class AdminInterfacePage : public InterfacePage
		{
		private:
			static const std::string _factory_key;

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
				, const Site* site = NULL ) const;

		};
	}
}
