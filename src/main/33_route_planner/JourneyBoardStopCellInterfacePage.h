
#ifndef SYNTHESE_JourneyBoardStopCellInterfacePage_H__
#define SYNTHESE_JourneyBoardStopCellInterfacePage_H__


#include "11_interfaces/InterfacePage.h"
#include "15_env/Alarm.h"
#include "04_time/Hour.h"

namespace synthese
{
	namespace interfaces
	{
		/** Journey board cell for stop at a place.
			@code journey_board_stop_cell @endcode
		*/
		class JourneyBoardStopCellInterfacePage : public InterfacePage
		{
		public:
			/** Display.
				@param stream Stream to write on
				@param isItArrival (0) true if the stop is used as an arrival, false else
				@param alarm (1/2) Alert (1=message, 2=level)
				@param isItTerminus (3) true if the stop is the terminus of the used line, false else
				@param stopName (4) Stop name
				@param color (5) Odd or even color
				@param firstTime (6) Fist time
				@param lastTime (7) Last time (UNKNOWN if continuous service)
				@param site Displayed site
			*/
			void display( std::ostream& stream, bool isItArrival, const synthese::env::Alarm* alarm
				, bool isItTerminus, const std::string& stopName, bool color
				, const synthese::time::Hour& firstTime, const synthese::time::Hour& lastTime
				, const Site* site = NULL ) const;

		};
	}
}
#endif // SYNTHESE_JourneyBoardStopCellInterfacePage_H__
