
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
		private:
			static const bool _registered;

		public:
			/** Display.
				@param stream Stream to write on
				@param site Display site
			*/

			void display(std::ostream& stream
				, const Site* site = NULL) const;
		};
	}
}