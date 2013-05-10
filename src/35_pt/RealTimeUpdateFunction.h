
//////////////////////////////////////////////////////////////////////////////////////////
/// RealTimeUpdateFunction class header.
///	@file RealTimeUpdateFunction.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_RealTimeUpdateFunction_H__
#define SYNTHESE_RealTimeUpdateFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class StopArea;
		class ScheduledService;
		class LineStop;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Function : display of service details.
		///	@author Hugues Romain
		///	@date 2009
		///	@ingroup m35Functions refFunctions
		///
		///	Key : rtu
		///
		///	Parameters :
		/// <dl>
		///	<dt>se</dt><dd>service (optional)</dd>
		///	<dt>ls</dt><dd>linestop (optional)</dd>
		///	<dt>t</dt><dd>CMS template to use for the display. The template will become the parameters produced by RealTimeUpdateFunction::_display.</dd>
		/// </dl>
		class RealTimeUpdateFunction:
			public util::FactorableTemplate<server::Function, RealTimeUpdateFunction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LINE_STOP_RANK;
			static const std::string PARAMETER_CMS_TEMPLATE_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const ScheduledService>	_service;
				std::size_t _lineStopRank;
				boost::shared_ptr<const cms::Webpage> _cmsTemplate;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2009
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2009
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);

		private:

			static const std::string DATA_LOCATION_;
			static const std::string DATA_DESTINATION_;
			static const std::string DATA_SERVICE_NUMBER;
			static const std::string DATA_REALTIME_QUAY;
			static const std::string DATA_TRANSPORT_MODE_ID;
			static const std::string DATA_PLANNED_SCHEDULE;
			static const std::string DATA_REALTIME_SCHEDULE;
			static const std::string DATA_DELAY;
			static const std::string DATA_DELAY_UPDATE_URL;
			static const std::string DATA_QUAY_UPDATE_URL;


			//////////////////////////////////////////////////////////////////////////
			/// Display of a service at a sto point.
			/// Variables sent to the CMS template :
			/// <dl>
			/// <dt>location_stop_id</dt><dd>stop area id</dd>
			///	<dt>location_stop_name</dt><dd>stop area name in the city</dd>
			///	<dt>location_city_id</dt><dd>id of the city where the stop area is located</dd>
			///	<dt>location_city_name</dt><dd>name of the city where the stop area is located</dd>
			///	<dt>location_stop_name_13</dt><dd>stop area short alias (13 characters max) designed for departure boards</dd>
			///	<dt>location_stop_name_26</dt><dd>stop area middle sized alias (26 characters max) designed for departure boards</dd>
			///	<dt>location_stop_name_for_timetables</dt><dd>stop area alias designed for timetables</dd>
			/// <dt>destination_stop_id</dt><dd>stop area id</dd>
			///	<dt>destination_stop_name</dt><dd>stop area name in the city</dd>
			///	<dt>destination_city_id</dt><dd>id of the city where the stop area is located</dd>
			///	<dt>destination_city name</dt><dd>name of the city where the stop area is located</dd>
			///	<dt>destination_stop_name_13</dt><dd>stop area short alias (13 characters max) designed for departure boards</dd>
			///	<dt>destination_stop_name_26</dt><dd>stop area middle sized alias (26 characters max) designed for departure boards</dd>
			///	<dt>destination_stop_name_for_timetables</dt><dd>stop area alias designed for timetables</dd>
			/// <dt>line_id</dt><dd>line id</dd>
			///	<dt>line_short_name</dt><dd>line short name (in most of cases the line number)</dd>
			///	<dt>line_long_name</dt><dd>line long name</dd>
			///	<dt>line_color</dt><dd>line RGB color</dd>
			///	<dt>line_style</dt><dd>line CSS class</dd>
			/// <dt>line_image</dt><dd>line image url</dd>
			/// <dt>service_number</dt><dd>service number</dd>
			/// <dt>realtime_quay_name</dt><dd>real time quay name</dd>
			/// <dt>transport_mode_id</dt><dd>transport mode id</dd>
			/// <dt>planned_schedule</dt><dd>planned schedule</dd>
			/// <dt>realtime_schedule</dt><dd>real-time schedule</dd>
			/// <dt>delay</dt><dd>delay (minutes)</dd>
			/// <dt>delay_update_url</dt><dd>beginning of the URL which calls an update of the delay</dd>
			/// <dt>quay_update_url</dt><dd>beginning of the URL which calls an update of the quay</dd>
			/// </dl>
			void _display(
				std::ostream& stream,
				const server::Request& request,
				const ScheduledService& service,
				const LineStop& lineStop
			) const;

		public:
			RealTimeUpdateFunction();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @author Hugues
			/// @date 2009
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2009
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2009
			virtual std::string getOutputMimeType() const;


			void setService(boost::shared_ptr<const ScheduledService> value);
			void setLineStopRank(std::size_t value);
		};
	}
}

#endif // SYNTHESE_RealTimeUpdateFunction_H__
