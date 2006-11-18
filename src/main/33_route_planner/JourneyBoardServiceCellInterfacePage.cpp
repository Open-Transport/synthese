
#include "JourneyBoardServiceCellInterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		void JourneyBoardServiceCellInterfacePage::display( std::ostream& stream
			, const synthese::time::Hour& firstDepartureTime , const synthese::time::Hour& lastDepartureTime
			, const synthese::time::Hour& firstArrivalTime , const synthese::time::Hour& lastArrivalTime
			, int rollingStockId, const std::string& rollingStockName, const std::string& rollingStockFullDescription
			, const std::string& destinationName , boost::logic::tribool handicappedFilterStatus
			, int handicappedPlacesNumber, boost::logic::tribool bikeFilterStatus, int bikePlacesNumber
			, bool isReservationCompulsory, bool isReservationOptional, const synthese::time::DateTime maxBookingDate
			, const synthese::env::ReservationRule* reservationRule, const std::string& syntheseOnlineBookingURL
			, const synthese::env::Alarm* alarm , bool color, const synthese::env::Path* line
			, const Site* site /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( firstDepartureTime.toInternalString() );
			pv.push_back( lastDepartureTime.toInternalString() );
			pv.push_back( firstArrivalTime.toInternalString() );
			pv.push_back( lastArrivalTime.toInternalString() );
			pv.push_back( synthese::util::Conversion::ToString( rollingStockId ) );
			pv.push_back( synthese::util::Conversion::ToString( handicappedFilterStatus ) );
			pv.push_back( synthese::util::Conversion::ToString( handicappedPlacesNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( bikeFilterStatus ) );
			pv.push_back( synthese::util::Conversion::ToString( bikePlacesNumber ) );
			pv.push_back( synthese::util::Conversion::ToString( isReservationCompulsory ) );
			pv.push_back( synthese::util::Conversion::ToString( isReservationOptional ) );
			pv.push_back( maxBookingDate.toInternalString() );
			pv.push_back( reservationRule->getPhoneExchangeNumber() );
			pv.push_back( reservationRule->getPhoneExchangeOpeningHours() );
			pv.push_back( reservationRule->getWebSiteUrl() );
			pv.push_back( syntheseOnlineBookingURL );
			pv.push_back( alarm != NULL ? alarm->getMessage() : "" );
			pv.push_back( alarm != NULL ? synthese::util::Conversion::ToString( alarm->getLevel() ) : "" );
			pv.push_back( synthese::util::Conversion::ToString( color ) );

			InterfacePage::display( stream, pv, (const void*) line, site );
		}
	}
}