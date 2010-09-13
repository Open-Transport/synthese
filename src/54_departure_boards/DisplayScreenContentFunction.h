
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

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	class graph::ServicePointer;
	class pt::StopPoint;

	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayType;

		/** Arrival/departures table generator function.
			@ingroup m54Functions refFunctions
			@author Hugues Romain
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
		};
	}
}
#endif // SYNTHESE_DisplayScreenContentRequest_H__
