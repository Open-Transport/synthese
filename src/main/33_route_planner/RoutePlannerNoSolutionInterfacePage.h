
#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		/** No solution message.
			@code routeplanner_no_solution @endcode
		*/
		class RoutePlannerNoSolutionInterfacePage : public InterfacePage
		{
		public:
			/** Display.
				@param stream Stream to write on
				@param site Display site
			*/

			void display(std::ostream& stream
				, const server::Request* request = NULL) const;
		};
	}
}
