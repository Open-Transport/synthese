
#ifndef SYNTHESE_JourneyBoardServiceCellInterfacePage_H__
#define SYNTHESE_JourneyBoardServiceCellInterfacePage_H__


#include "11_interfaces/InterfacePage.h"
#include "04_time/DateTime.h"
#include "15_env/ReservationRule.h"
#include "15_env/Alarm.h"


#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace env
	{
		class Path;
	}

	namespace interfaces
	{
		class Site;

		/** Journey board cell for use of a service.
		@code journey_board_service_cell @endcode
		*/
		class JourneyBoardServiceCellInterfacePage : public InterfacePage
		{
		public:
			/** Display.
				@param stream Stream to write on
				@param firstDepartureTime (0) First start time
				@param lastDepartureTime (1) Last start time (will be empty if not continuous service)
				@param firstArrivalTime (2) First start time
				@param lastArrivalTime (3) Last start time (will be empty if not continuous service)
				@param rollingStockId (4) ID of used rolling stock
				@param rollingStockName (5) Display name of used rolling stock
				@param rollingStockFullDescription (6) HTML description of used Rolling stock
				@param destinationName (7) Destination name of the vehicle
				@param handicappedFilterStatus (8)
				@param handicappedPlacesNumber (9)
				@param bikeFilterStatus (10)
				@param bikePlacesNumber (11)
				@param isReservationCompulsory (12)
				@param isReservationOptional (13)
				@param maxBookingDate (14)
				@param reservationRule (15/16/17) Reservation rule (15=tel number, 16=times of opening, 17=web URL)
				@param syntheseOnlineBookingURL (18) URL to run to book on the service. Empty if online booking unavailable
				@param alarm (19/20) Alert (19=message, 20=level)
				@param color (21) Odd or even color
				@param line (Path* object)
				@param site Displayed site
			*/
			void display( std::ostream& stream
				, const synthese::time::Hour& firstDepartureTime
				, const synthese::time::Hour& lastDepartureTime
				, const synthese::time::Hour& firstArrivalTime
				, const synthese::time::Hour& lastArrivalTime
				, int rollingStockId
				, const std::string& rollingStockName
				, const std::string& rollingStockFullDescription
				, const std::string& destinationName
				, boost::logic::tribool handicappedFilterStatus
				, int handicappedPlacesNumber
				, boost::logic::tribool bikeFilterStatus
				, int bikePlacesNumber
				, bool isReservationCompulsory
				, bool isReservationOptional
				, const synthese::time::DateTime maxBookingDate
				, const synthese::env::ReservationRule* reservationRule
				, const std::string& syntheseOnlineBookingURL
				, const synthese::env::Alarm* alarm
				, bool color
				, const synthese::env::Path* line
				, const server::Request* request = NULL ) const;

		};
	}
}
#endif // SYNTHESE_JourneyBoardServiceCellInterfacePage_H__
