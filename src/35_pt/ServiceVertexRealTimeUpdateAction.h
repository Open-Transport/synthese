
/** ServiceVertexRealTimeUpdateAction class header.
	@file ServiceVertexRealTimeUpdateAction.h
	@author Hugues Romain
	@date 2009

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

#ifndef SYNTHESE_ServiceVertexRealTimeUpdateAction_H__
#define SYNTHESE_ServiceVertexRealTimeUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace pt
	{
		class ScheduledService;
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		/// ServiceVertexRealTimeUpdateAction action class.
		/// https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_vertex_served_by_a_service
		///	@ingroup m35Actions refActions
		class ServiceVertexRealTimeUpdateAction:
			public util::FactorableTemplate<server::Action, ServiceVertexRealTimeUpdateAction>
		{
		public:
			static const std::string PARAMETER_DATASOURCE_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LINE_STOP_RANK;
			static const std::string PARAMETER_LINE_STOP_METRIC_OFFSET;
			static const std::string PARAMETER_STOP_ID;
			static const std::string PARAMETER_PROPAGATE;
			static const std::string PARAMETER_RESTORE_PLANNED_STOP;

		private:
			boost::shared_ptr<ScheduledService> _service;
			boost::shared_ptr<impex::DataSource> _dataSource;
			boost::shared_ptr<pt::StopPoint> _physicalStop;
			std::size_t _lineStopRank;
			bool _propagate;
			bool _restorePlannedStop;
			std::string _serviceCodeBySource;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_vertex_served_by_a_service#Request
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_vertex_served_by_a_service#Request
			///	Removes the used parameters from the map.
			///	@param map Parameters map to interpret
			///	@exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			ServiceVertexRealTimeUpdateAction();

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			virtual bool _isSessionRequired() const { return false; }

			//! @name Setters
			//@{
				void setService(boost::shared_ptr<ScheduledService> service){ _service = service; }
				void setLineStopRank(std::size_t value);
				void setPhysicalStop(boost::shared_ptr<StopPoint> value);
			//@}
		};
	}
}

#endif // SYNTHESE_ServiceVertexRealTimeUpdateAction_H__
