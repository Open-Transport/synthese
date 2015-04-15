
/** ServiceVertexUpdateAction class header.
	@file ServiceVertexUpdateAction.h
	@author Gael Sauvenet
	@date 2012

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

#ifndef SYNTHESE_ServiceVertexUpdateAction_H__
#define SYNTHESE_ServiceVertexUpdateAction_H__

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
		/// ServiceVertexUpdateAction action class.
		///	@ingroup m35Actions refActions
		class ServiceVertexUpdateAction:
			public util::FactorableTemplate<server::Action, ServiceVertexUpdateAction>
		{
		public:
			static const std::string PARAMETER_IS_CONTINUOUS;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LINE_STOP_RANK;
			static const std::string PARAMETER_STOP_ID;

		private:
			boost::shared_ptr<ScheduledService> _service;
			boost::shared_ptr<pt::StopPoint> _physicalStop;
			std::size_t _lineStopRank;
			std::string _serviceCodeBySource;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	Removes the used parameters from the map.
			///	@param map Parameters map to interpret
			///	@exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			ServiceVertexUpdateAction();

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

#endif // SYNTHESE_ServiceVertexUpdateAction_H__
