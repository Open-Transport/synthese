
/** DisplayScreenContentFunction class header.
	@file DisplayScreenContentFunction.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_DisplayScreenContentRequest_H__
#define SYNTHESE_DisplayScreenContentRequest_H__

#include "FunctionWithSite.h"
#include "FactorableTemplate.h"
#include "UtilTypes.h"
#include "DeparturesTableTypes.h"
#include "DisplayScreen.h"

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace graph
	{
		class ServicePointer;
	}

	namespace pt
	{
		class StopPoint;
		class StopArea;
	}

	namespace pt_website
	{
		class RollingStockFilter;
	}

	namespace departure_boards
	{
		class AlarmTestOnDisplayScreenFunction;
		class DisplayType;

		//////////////////////////////////////////////////////////////////////////
		/// Arrival/departures table generator function.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m54Functions refFunctions
		/// @author Hugues Romain, Xavier Raffin
		/// @date 2002
		class DisplayScreenContentFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<false>,DisplayScreenContentFunction>
		{
		public:
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_WAY;
			static const std::string PARAMETER_TB;
			static const std::string PARAMETER_INTERFACE_ID;
			static const std::string PARAMETER_MAC_ADDRESS;
			static const std::string PARAMETER_OPERATOR_CODE;
			static const std::string PARAMETER_ROWS_NUMBER;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_STOP_NAME;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_ROLLING_STOCK_FILTER_ID;
			static const std::string PARAMETER_GENERATION_METHOD;
			static const std::string PARAMETER_USE_SAE_DIRECT_CONNECTION;
			static const std::string PARAMETER_USE_SCOM;

			static const std::string DATA_FIRST_DEPARTURE_TIME;
			static const std::string DATA_LAST_DEPARTURE_TIME;

			static const std::string PARAMETER_MAIN_PAGE_ID;
			static const std::string PARAMETER_ROW_PAGE_ID;
			static const std::string PARAMETER_DESTINATION_PAGE_ID;
			static const std::string PARAMETER_TRANSFER_DESTINATION_PAGE_ID;
			static const std::string PARAMETER_DATA_SOURCE_NAME_FILTER;

			static const std::string PARAMETER_STOPS_LIST;

			static const std::string DATA_STOP_ID;
			static const std::string DATA_OPERATOR_CODE;
			static const std::string DATA_NETWORK_ID;
			static const std::string DATA_NETWORK_NAME;

			static const std::string DATA_STOP_AREA_ID;
			static const std::string DATA_STOP_AREA_NAME;
			static const std::string DATA_STOP_AREA_CITY_NAME;
			static const std::string DATA_STOP_AREA_CITY_ID;

			static const std::string DATA_IS_REAL_TIME;

			static const std::string DATA_HANDICAPPED_ACCESS;
			
			static const std::string DATA_SERVICE_ARRIVAL_COMMENT;
			static const std::string DATA_SERVICE_DEPARTURE_COMMENT;

			//Direct connection SAE structures :
                        struct ServiceRealTime
                        {
                                std::string date;
                                std::string Realtime;
                                std::string oc;
                                std::string arret;
                                std::string nom_ligne;
                                std::string lineShortName;
                                std::string LineStyle;
                                std::string lineColor;
                                std::string lineXmlColor;
                                std::string depart;
                                std::string cityName_begin;
                                std::string arrivee;
                                std::string cityName_end;
                                std::string cityName_current;
                                util::RegistryKeyType cityId_current;
                                util::RegistryKeyType stopAreaId;
                                util::RegistryKeyType stop_id;
                                util::RegistryKeyType networkId;
                        };

		private:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreen> _screen;
				boost::shared_ptr<DisplayType> _type;
				boost::optional<boost::posix_time::ptime> _date;
				boost::optional<util::RegistryKeyType> _lineToDisplay;
				boost::shared_ptr<const pt_website::RollingStockFilter> _rollingStockFilter;
				bool _wayIsBackward;
				boost::optional<std::string> _dataSourceName;
				static std::vector<std::string> _SAELine;
				bool _useSAEDirectConnection;

				boost::shared_ptr<const cms::Webpage> _mainPage;
				boost::shared_ptr<const cms::Webpage> _rowPage;
				boost::shared_ptr<const cms::Webpage> _destinationPage;
				boost::shared_ptr<const cms::Webpage> _transferDestinationPage;
			//@}

			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Request
			//////////////////////////////////////////////////////////////////////////
			void _setFromParametersMap(const util::ParametersMap& map);



			/** Concat an search XML result to stream
			*/
			void concatXMLResult(
				std::ostream& stream,
				graph::ServicePointer& servicePointer,
				const pt::StopPoint* stop
			)const;

			void concatXMLResultRealTime(
				std::ostream& stream,
				ServiceRealTime& serviceReal
			)const;

			//////////////////////////////////////////////////////////////////////////
			/// Add journey information to the given parameters map.
			/// @author Sylvain Pasche
			void addJourneyToParametersMap(
				util::ParametersMap& pm,
				graph::ServicePointer& servicePointer,
				const pt::StopPoint* stop
			) const;
			
			void addJourneyToParametersMapRealTime(
				util::ParametersMap& pm,
				ServiceRealTime& serviceReal
			) const;

			void displayFullDate(
				const std::string & datafieldName,
				const boost::posix_time::ptime & time,
				util::ParametersMap & pm
			) const;

		public:
			DisplayScreenContentFunction():
				_wayIsBackward(false)
			{}



			//////////////////////////////////////////////////////////////////////////
			/// Launches the display.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream stream to write the output on
			/// @param request request which has launched the function
			/// @pre _screen and _screen->getType() must be not null
			/// @author Hugues Romain, Xavier Raffin
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;

			//! @name Setters
			//@{
				void setScreen(boost::shared_ptr<const DisplayScreen> value);
			//@}

		private:
			//! @name Static methods to set CMS page variables for departure board
			//@{
				static const std::string DATA_TITLE;
				static const std::string DATA_WIRING_CODE;
				static const std::string DATA_DISPLAY_SERVICE_NUMBER;
				static const std::string DATA_DISPLAY_TRACK_NUMBER;
				static const std::string DATA_DISPLAY_TEAM;
				static const std::string DATA_STOP_NAME;
				static const std::string DATA_INTERMEDIATE_STOPS_NUMBER;
				static const std::string DATA_DISPLAY_CLOCK;
				static const std::string DATA_ROWS;
				static const std::string DATA_DATE;
				static const std::string DATA_MAC;
				static const std::string DATA_SUBSCREEN_;

				static const std::string DATA_JOURNEYS;

		public:
				//////////////////////////////////////////////////////////////////////////
				/// Display of a departure board.
				/// https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Response-through-CMS-for-chronological-departure-board
				//////////////////////////////////////////////////////////////////////////
				/// @param stream stream to write the result on
				/// @param request request which has called the display
				/// @param page CMS template to use to display the departure board
				/// @param rowPage CMS template to use to display each service present on the board
				/// @param destinationPage CMS template to use to display each destination of each destination of each service
				/// @param transferPage CMS template to use to display each transfer destination reachable from each destination of each service
				/// @param date departure date
				/// @param rows result to display
				/// @param screen display screen source object
				/// @author Hugues Romain
				/// @since 3.2.0
				void _displayDepartureBoard(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> rowPage,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					boost::shared_ptr<const cms::Webpage> transferPage,
					const boost::posix_time::ptime&  date,
					const ArrivalDepartureList& rows,
					const DisplayScreen& screen
				) const;
			private:

				static const std::string DATA_ROW_RANK;
				static const std::string DATA_PAGE_NUMBER;
				static const std::string DATA_BLINKS;
				static const std::string DATA_TIME;
				static const std::string DATA_WAITING_TIME;
				static const std::string DATA_PLANNED_TIME;
				static const std::string DATA_DELAY;
				static const std::string DATA_SERVICE_ID;
				static const std::string DATA_SERVICE_NUMBER;
				static const std::string DATA_TRACK;
				static const std::string DATA_TEAM;
				static const std::string DATA_TRANSPORT_MODE;
				static const std::string DATA_RANK_IN_PATH;
				static const std::string DATA_DESTINATIONS;
				static const std::string DATA_DIRECTION;
                static const std::string DATA_WAYBACK;
				static const std::string DATA_IS_CANCELED;

				//////////////////////////////////////////////////////////////////////////
				/// Displays a row of a departure board.
				/// https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Response-through-CMS-for-chronological-departure-board
				//////////////////////////////////////////////////////////////////////////
				/// @param stream stream to write the result on
				/// @param request request which has called the display
				/// @param page CMS template to use to display the service
				/// @param destinationPage CMS template to use to display each destination of each destination of each service
				/// @param transferPage CMS template to use to display each transfer destination reachable from each destination of each service
				/// @param rowRank rank of the row
				/// @param pageNumber page of memory to display on (for protocoles with separated pages)
				/// @param row result to display
				/// @param screen display screen source object
				/// @author Hugues Romain
				/// @since 3.2.0
				void _displayDepartureBoardRow(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					boost::shared_ptr<const cms::Webpage> transferPage,
					const boost::posix_time::ptime& requestTime,
					size_t rowRank,
					size_t pageNumber,
					const ArrivalDepartureRow& row,
					const DisplayScreen& screen
				) const;



				static const std::string DATA_IS_SAME_CITY;
				static const std::string DATA_IS_END_STATION;
				static const std::string DATA_DESTINATION_RANK;
				static const std::string DATA_DESTINATION_GLOBAL_RANK;
				static const std::string DATA_TRANSFERS;
				static const std::string DATA_IS_CONTINUATION;
				static const std::string DATA_CONTINUATION_STARTS_AT_END;

				//////////////////////////////////////////////////////////////////////////
				/// Display of an intermediate or ending destination of a departure board.
				/// https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Response-through-CMS-for-chronological-departure-board
				//////////////////////////////////////////////////////////////////////////
				/// @param stream stream to write the result on
				/// @param request request which has called the display
				/// @param page CMS template to use to display each destination of each destination of each service
				/// @param transferPage CMS template to use to display each transfer destination reachable from each destination of each service
				/// @param object service pointer representing the link between the start of the service and the destination to display
				/// @param rank rank of the destination in the service
				/// @param globalRank rank of the destination in the whole board
				/// @param transferDestinations transfers reachable by the destination
				/// @param screen the source object
				/// @param isContinuation true if the destination is reached by a continuation transfer
				/// @param continuationStartsAtEnd true if the continuation has started where the main service ends
				/// @param screen display screen source object
				/// @author Hugues Romain
				/// @since 3.2.0
				void _displayDepartureBoardDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> transferPage,
					const graph::ServicePointer& object,
					bool lastDisplayedStopWasInTheSameCity,
					bool isTheEndStation,
					size_t rank,
					size_t globalRank,
					const IntermediateStop::TransferDestinations& transferDestinations,
					const DisplayScreen& screen,
					bool isContinuation,
					bool continuationStartsAtEnd
				) const;



				static const std::string DATA_DEPARTURE_TIME;
				static const std::string DATA_ARRIVAL_TIME;
				static const std::string DATA_TRANSFER_RANK;

				//////////////////////////////////////////////////////////////////////////
				/// Display of a transfer destination from an intermediate stop.
				/// https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Response-through-CMS-for-chronological-departure-board
				//////////////////////////////////////////////////////////////////////////
				void _displayDepartureBoardTrandferDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					const graph::ServicePointer& object,
					size_t localTransferRank,
					const DisplayScreen& screen
				) const;

			//@}

			//! @name Static methods to set CMS page variables for route planning board
			//@{
				static const std::string DATA_WITH_TRANSFER;
			public:
				//////////////////////////////////////////////////////////////////////////
				/// Display of a route planning board.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Journey-planner-based-departure-board-main-page
				void _displayRoutePlanningBoard(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> rowPage,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					const boost::posix_time::ptime& date,
					const RoutePlanningList& rows,
					const DisplayScreen& screen
				) const;
			private:


				static const std::string DATA_SECOND_;
				static const std::string DATA_SECOND_TRACK;
				static const std::string DATA_SECOND_SERVICE_NUMBER;
				static const std::string DATA_SECOND_TIME;
				static const std::string DATA_SECOND_TRANSPORT_MODE;
				static const std::string DATA_TRANSFER_STOP_NAME;

				//////////////////////////////////////////////////////////////////////////
				/// Displays a row of a route planning board.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Destination-of-journey-planner-based-departure-board
				//////////////////////////////////////////////////////////////////////////
				void _displayRoutePlanningBoardRow(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					size_t rowId,
					const RoutePlanningRow& row,
					const DisplayScreen& screen
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Variables sent to the cms :
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Destination-of-journey-planner-based-departure-board
				//////////////////////////////////////////////////////////////////////////
				void _displayRoutePlanningBoardDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					const pt::StopArea& place
				) const;
			//@}

			/// Use scom for this service
			/// False by default, set to true if "use_scom" is in the parameters
			bool _scom;

			server::FunctionAPI getAPI() const;

			friend class AlarmTestOnDisplayScreenFunction;
		};
}	}
#endif // SYNTHESE_DisplayScreenContentRequest_H__
