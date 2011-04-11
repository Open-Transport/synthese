
/** DisplayScreenContentFunction class header.
	@file DisplayScreenContentFunction.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

	namespace departure_boards
	{
		class DisplayType;

		/** Arrival/departures table generator function.
			@ingroup m54Functions refFunctions
			@author Hugues Romain, Xavier Raffin
			@date 2002

			<h2>Usage</h2>
			
			The content can be specified in 3 different ways :
			
			<h3>Usage 1 : Loading pre-configured display screen</h3>
	
			Remark : this is the only way to configure several parameters like the generation algorithm.
			
			The display screen can be specified by two ways :

			<h4>Usage 1.1 by id
			Parameters :
			<ul>
				<li>roid / tb : id of the pre-configured display screen</li>
				<li>date (optional) : date of search (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
			</ul>

			<h4>Usage 1.2 by mac address
			Parameters :
			<ul>
				<li>m : MAC address of the display screen</li>
				<li>date (optional) : date of search (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
			</ul>
			
			<h3>Usage 2 : Generating from a connection place</h3>
			
			The connection place can be specified by two ways :
			
			<h4>Usage 2.1 by id</h4>
			
			Parameters :
			<ul>
				<li>roid : id of the connection place</li>
				<li>i : id of display interface. If not specified, the standard XML output is used</li>
				<li>rn : table rows number</li>
				<li>date (optional) : date of search (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
			</ul>
			
			<h4>Usage 2.2 : by name</h4>
			
			Parameters :
			<ul>
				<li>cn : name of the city</li>
				<li>sn : name of the stop</li>
				<li>i : id of display interface. If not specified, the standard XML output is used</li>
				<li>rn : table rows number</li>
				<li>date (optional) : date of search (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
			</ul>
			
			<h3>Usage 3 : Generating from a physical stop</h3>
						
			The physical stop can be specified by two ways :
			
			<h4>Usage 3.1 by id</h4>
			
			Parameters :
			<ul>
				<li>roid : id of the physical stop</li>
				<li>i : id of display interface. If not specified, the standard XML output is used</li>
				<li>rn : table rows number</li>
			</ul>
			
			<h4>Usage 3.2 : by operator code</h4>
			
			Parameters :
			<ul>
				<li>roid : id of the connection place which belongs the stop</li>
				<li>oc : operator code of the physical stop</li>
				<li>i : id of display interface. If not specified, the standard XML output is used</li>
				<li>rn : table rows number</li>
			</ul>


			<h3>Usage 4 : Standard XML output</h2>

			<h4>Usage 4.1 : by operator code</h4>
			<ul>
				<li>oc : operator code of the physical stop</li>
				<li>rn : table rows number</li>
				<li>date (optional) : reference date (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
				<li>way (optional) : "backward" or "forward" Default value : "forward"</li>
				<li>lineid (optional) : Commercial line ID : if given then results are only for this line</li>
			</ul>
			If 'way' is "forward" the answer will be the 'rn' next departures after 'date'.
			If 'way' is "backward" the answer will be the 'rn' previous departures just before 'date'.
			<u>WARNING :</u> rn is the number of departures which have different start time.
			Consequently, if two service starts at the same minutes it will count for 1 start !

			<h4>Usage 4.2 : by physical stop ID</h4>
			<ul>
				<li>roid : id of the physical stop</li>
				<li>rn : table rows number</li>
				<li>date (optional) : reference date (iso format : YYYY-MM-DD HH:II). Default value : the time of the request</li>
				<li>way (optional) : "backward" or "forward" Default value : "forward"</li>
				<li>lineid (optional) : Commercial line ID : if given then results are only for this line</li>
			</ul>
			If 'way' is "forward" the answer will be the 'rn' next departures after 'date'.
			If 'way' is "backward" the answer will be the 'rn' previous departures just before 'date'.
			<u>WARNING :</u> rn is the number of departures which have different start time.
			Consequently, if two service starts at the same minutes it will count for 1 start !

			@image html DisplayScreenContentFunction.png

			<h3>Download</h3>
			<ul>
				<li><a href="include/54_departures_table/DisplayScreenContentFunction.xsd">XML output schema</a></li>
			</ul>
		*/
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

		private:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreen>	  _screen;
				boost::shared_ptr<DisplayType>			  _type;
				boost::optional<boost::posix_time::ptime> _date;
				boost::optional<util::RegistryKeyType>    _lineToDisplay;
				bool	_wayIsBackward;
			//@}

			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			/** Concat an search XML result to stream
			*/
			void concatXMLResult(
				std::ostream& stream,
				graph::ServicePointer& servicePointer,
				const pt::StopPoint* stop
				)const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Launches the display.
			///	@param stream stream to write the output on
			///	@param request request which has launched the function
			/// @pre _screen and _screen->getType() must be not null
			/// @author Hugues Romain, Xavier Raffin
			void run(std::ostream& stream, const server::Request& request) const;

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
				static const std::string DATA_MESSAGE_LEVEL;
				static const std::string DATA_MESSAGE_CONTENT;
				static const std::string DATA_DATE;
				static const std::string DATA_MAC;
				static const std::string DATA_SUBSCREEN_;

		public:
				//////////////////////////////////////////////////////////////////////////
				/// Display of a departure board.
				///	Variables sent to the cms :
				/// <dl>
				/// <dt>roid</dt><dd>id of the screen</dd>
				///	<dt>title</dt><dd>title of the screen</dd>
				/// <dt>date</dt><dd>date of the display</dd>
				///	<dt>wiring_code</dt><dd>wiring code of the screen</dd>
				///	<dt>display_service_number</dt><dd>service number must be displayed (1|0)</dd>
				///	<dt>display_track_number</dt><dd>track number must be displayed (1|0)</dd>
				///	<dt>intermediate_stops_number</dt><dd>number of intermediates stops to display</dd>
				///	<dt>display_team</dt><dd>team identifier must be displayed (1|0)</dd>
				///	<dt>stop_name</dt><dd>name of the departure stop area</dd>
				///	<dt>display_clock</dt><dd>clock must be displayed (1|0)</dd>
				///	<dt>rows</dt><dd>rows of the departure board</dd>
				///	<dt>message_level</dt><dd>level of the message linked with the screen (undefined if no message)</dd>
				/// <dt>message_content</dt><dd>content of the message linked with the screen (undefined if no message)</dd>
				/// <dt>subscreen_x</dt><dd>id of the xth sub screen (x replaced by numeric value 0, 1, 2, ... The cms must lauch itself the display for each subscreen.</dd>
				/// </dl>
				///
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
				static void DisplayDepartureBoard(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> rowPage,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					boost::shared_ptr<const cms::Webpage> transferPage,
					const boost::posix_time::ptime&  date,
					const ArrivalDepartureListWithAlarm& rows,
					const DisplayScreen& screen
				);
			private:

				static const std::string DATA_ROW_RANK;
				static const std::string DATA_PAGE_NUMBER;
				static const std::string DATA_BLINKS;
				static const std::string DATA_TIME;
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
				
				//////////////////////////////////////////////////////////////////////////
				/// Displays a row of a departure board.
				///	Variables sent to the CMS template :
				/// <dl>
				///	<dt>row_rank</dt><dd>Rank of the row in the departure table</dd>
				///	<dt>page_number</dt><dd>Number of the page to display for multiple page protocol (eg Lumiplan)</dd>
				///	<dt>display_service_number</dt><dd>service number must be displayed (1|0)</dd>
				///	<dt>display_track_number</dt><dd>track number must be displayed (1|0)</dd>
				///	<dt>intermediate_stops_number</dt><dd>number of intermediates stops to display</dd>
				///	<dt>display_team</dt><dd>team identifier must be displayed (1|0)</dd>
				///	<dt>blinks</dt><dd>Departure blinks ? (1|0)</dd>
				///	<dt>time</dt><dd>Departure date time</dd>
				/// <dt>planned_time</dt><dd>Planned departure date time</dd>
				/// <dt>service_id</dt><dd>ID of the service</dd>
				///	<dt>service_number</dt><dd>Service number</dd>
				///	<dt>track</dt><dd>Track name</dd>
				///	<dt>team</dt><dd>Team number</dd>
				///	<dt>transport_mode</dt><dd>ID of the transport mode</dd>
				///	<dt>delay</dt><dd>delay duration</dd>
				/// <dt>rank_in_path</dt><dd>Rank of the departure stop in the path of the service</dd>
				/// <dt>destinations</dt><dd>Destinations (content generated by the cms if a destinationPage is defined)</dd>
				/// <dt>subscreen_x</dt><dd>id of the xth sub screen (x replaced by numeric value 0, 1, 2, ... The cms must lauch itself the display for each subscreen.</dd>
				/// <dt>direction</dt><dd>Direction shown in front of the vehicle</dd>
				/// <dt>line_id</dt><dd>line id</dd>
				///	<dt>line_short_name</dt><dd>line short name (in most of cases the line number)</dd>
				///	<dt>line_long_name</dt><dd>line long name</dd>
				///	<dt>line_color</dt><dd>line RGB color</dd>
				///	<dt>line_style</dt><dd>line CSS class</dd>
				/// <dt>line_image</dt><dd>line image url</dd>
				/// <dt>stop_id</dt><dd>departure stop area id</dd>
				///	<dt>stop_name</dt><dd>departure stop area name in the city</dd>
				///	<dt>city_id</dt><dd>id of the departure city where the stop area is located</dd>
				///	<dt>city name</dt><dd>name of the departure city where the stop area is located</dd>
				///	<dt>stop_name_13</dt><dd>departure stop area short alias (13 characters max) designed for departure boards</dd>
				///	<dt>stop_name_26</dt><dd>departure stop area middle sized alias (26 characters max) designed for departure boards</dd>
				///	<dt>stop_name_for_timetables</dt><dd>departure stop area alias designed for timetables</dd>
				/// </dl>
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
				static void DisplayDepartureBoardRow(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					boost::shared_ptr<const cms::Webpage> transferPage,
					std::size_t rowRank,
					int pageNumber,
					const ArrivalDepartureRow& row,
					const DisplayScreen& screen
				);



				static const std::string DATA_IS_SAME_CITY;
				static const std::string DATA_IS_END_STATION;
				static const std::string DATA_DESTINATION_RANK;
				static const std::string DATA_DESTINATION_GLOBAL_RANK;
				static const std::string DATA_TRANSFERS;
				static const std::string DATA_IS_CONTINUATION;
				static const std::string DATA_CONTINUATION_STARTS_AT_END;
				
				//////////////////////////////////////////////////////////////////////////
				/// Display of an intermediate or ending destination of a departure board.
				///
				/// Variables sent to the cms :
				/// <dl>
				///	<dt>stop_id</dt><dd>stop ID</dd>
				///	<dt>city_name</dt><dd>stop city name</dd>
				///	<dt>stop_name</dt><dd>stop name</dd>
				///	<dt>stop_name_26</dt><dd>stop 26 characters alias</dd>
				///	<dt>stop_name_13</dt><dd>stop 13 characters alias</dd>
				///	<dt>is_same_city</dt><dd>last displayed stop was in the same city ? (1|0)</dd>
				///	<dt>time</dt><dd>arrival date time</dd>
				///	<dt>is_end_station</dt><dd>is the end station</dd>
				///	<dt>destination_rank</dt><dd>rank of the destination in the service (not rank in path)</dd>
				/// <dt>destination_global_rank</dt><dd>rank of the destination in the board (includes continuation transfer reached destinations)</dd>
				/// <dt>transfers</dt><dd>Transfers (content generated by the cms if a transferPage is defined)</dd>
				/// <dt>subscreen_x</dt><dd>id of the xth sub screen (x replaced by numeric value 0, 1, 2, ... The cms must lauch itself the display for each subscreen.</dd>
				/// <dt>direction</dt><dd>Direction shown in front of the vehicle</dd>
				/// <dt>is_continuation</dt><dd>1|0 : 1 if the destination is reached by a continuation transfer</dd>
				/// <dt>continuation_starts_at_end</dt><dd>1|0 : 1 if the continuation has started where the main service ends</dd>
				/// </dl>
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
				static void DisplayDepartureBoardDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> transferPage,
					const graph::ServicePointer& object,
					bool lastDisplayedStopWasInTheSameCity,
					bool isTheEndStation,
					std::size_t rank,
					std::size_t globalRank,
					const IntermediateStop::TransferDestinations& transferDestinations,
					const DisplayScreen& screen,
					bool isContinuation,
					bool continuationStartsAtEnd
				);



				static const std::string DATA_DEPARTURE_TIME;
				static const std::string DATA_ARRIVAL_TIME;
				static const std::string DATA_TRANSFER_RANK;

				//////////////////////////////////////////////////////////////////////////
				/// Display of a transfer destination from an intermediate stop.
				/// <dl>
				///	<dt>transport_mode</dt><dd>transport mode ID</dd>
				///	<dt>line_id</dt><dd>line ID</dd>
				///	<dt>line_short_name</dt><dd>line number (short name)</dd>
				///	<dt>line_long_name</dt><dd>line long name</dd>
				///	<dt>line_style</dt><dd>line CSS class</dd>
				///	<dt>line_color</dt><dd>line RGB Color</dd>
				///	<dt>line_image</dt><dd>line img path</dd>
				///	<dt>stop_id</dt><dd>destination ID</dd>
				///	<dt>city_name</dt><dd>destination city name</dd>
				///	<dt>stop_name</dt><dd>destination name</dd>
				///	<dt>stop_name_26</dt><dd>destination 26 characters alias</dd>
				///	<dt>stop_name_13</dt><dd>destination 13 characters alias</dd>
				///	<dt>departure_time</dt><dd>departure time</dd>
				///	<dt>arrival_time</dt><dd>arrival time</dd>
				///	<dt>transfer_rank</dt><dd>transfer rank in the transfer place</dd>
				///	<dt>subscreen_x</dt><dd>id of the xth sub screen (x replaced by numeric value 0, 1, 2, ... The cms must lauch itself the display for each subscreen.</dd>
				/// </dl>
				static void DisplayDepartureBoardTrandferDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					const graph::ServicePointer& object,
					std::size_t localTransferRank,
					const DisplayScreen& screen
				);

			//@}

			//! @name Static methods to set CMS page variables for route planning board
			//@{
				static const std::string DATA_WITH_TRANSFER;
			public:
				//////////////////////////////////////////////////////////////////////////
				/// Display of a route planning board.
				/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Journey-planner-based-departure-board-main-page
				static void DisplayRoutePlanningBoard(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> rowPage,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					const std::string& title,
					int wiringCode,
					bool displayServiceNumber,
					bool displayTrackNumber,
					bool withTransfer,
					boost::posix_time::time_duration blinkingDelay,
					bool displayClock,
					const pt::StopArea& place,
					const RoutePlanningListWithAlarm& rows,
					const DisplayScreen::ChildrenType& subscreens
				);
			private:


				static const std::string DATA_SECOND_;
				static const std::string DATA_SECOND_TRACK;
				static const std::string DATA_SECOND_SERVICE_NUMBER;
				static const std::string DATA_SECOND_TIME;
				static const std::string DATA_SECOND_TRANSPORT_MODE;
				static const std::string DATA_TRANSFER_STOP_NAME;
				
				//////////////////////////////////////////////////////////////////////////
				/// Displays a row of a route planning board.
				/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Departure_board_content_generator#Destination-of-journey-planner-based-departure-board
				static void DisplayRoutePlanningBoardRow(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					boost::shared_ptr<const cms::Webpage> destinationPage,
					std::size_t rowId,
					bool displayQuaiNumber,
					bool displayServiceNumber,
					boost::posix_time::time_duration blinkingDelay,
					bool withTransfer,
					const pt::StopArea& origin,
					const RoutePlanningRow& row,
					const DisplayScreen::ChildrenType& subscreens
				);


				//////////////////////////////////////////////////////////////////////////
				/// Variables sent to the cms :
				/// <dl>
				///	<dt>city_name</dt><dd>City name</dd>
				///	<dt>stop_name</dt><dd>Place name</dd>
				///	<dt>stop_name_13</dt><dd>Place name (13 chars)</dd>
				///	<dt>stop_name_26</dt><dd>Place name (26 chars)</dd>
				/// </dl>
				static void DisplayRoutePlanningBoardDestination(
					std::ostream& stream,
					const server::Request& request,
					boost::shared_ptr<const cms::Webpage> page,
					const pt::StopArea& place
				);
			//@}
		};
	}
}
#endif // SYNTHESE_DisplayScreenContentRequest_H__
