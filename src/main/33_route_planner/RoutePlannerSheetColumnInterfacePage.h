
#include "11_interfaces/InterfacePage.h"
#include "04_time/Hour.h"

namespace synthese
{
	namespace interfaces
	{
		/** Schedule sheet cell.
			@code schedule_sheet_column @endcode
		*/
		class RoutePlannerSheetColumnInterfacePage : public InterfacePage
		{
		public:
			/** Display of schedule sheet cell.
				@param stream Stream to write on
				@param isItFirstLine (0) Is the cell the first departure or arrival ?
				@param isItLastLine (1) Is the cell the last departure or arrival ?
				@param columnNumber (2) Rank of the column from left to right
				@param isItFootLine (3) Is the cell on a pedestrian junction ?
				@param firstTime (4) Start of continuous service, Time else
				@param lastTime (5) End of continuous service, Time else
				@param isItContinuousService (6) Is the cell on a continuous service ?
				@param site Displayed site
			*/
			void display( std::ostream& stream, bool isItFirstLine, bool isItLastLine, size_t columnNumber, bool isItFootLine
				, const synthese::time::Hour& firstTime, const synthese::time::Hour& lastTime
				, bool isItContinuousService, const Site* site = NULL ) const;
		};
	}
}