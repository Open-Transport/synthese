

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace routeplanner
	{
		class Journey;
	}

	namespace interfaces
	{
		/** Schedule sheet lines list cell.
		@code schedule_sheet_lines_cell @endcode
		*/
		class RoutePlannerSheetLinesCellInterfacePage : public InterfacePage
		{
		public:
			/** Display of schedule sheet lines list cell.
				@param stream Stream to write on
				@param columnNumber (0) Column rank from left to right
				@param journey () The journey to describe
				@param site Displayed site
			*/
			void display( std::ostream& stream, size_t columnNumber, const synthese::routeplanner::Journey* object, const server::Request* request= NULL ) const;
		};
	}
}


