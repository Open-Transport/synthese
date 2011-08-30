
//////////////////////////////////////////////////////////////////////////
/// DeviateServiceAction class header.
///	@file DeviateServiceAction.h
///	@author Hugues
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_DeviateServiceAction_H__
#define SYNTHESE_DeviateServiceAction_H__

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

		//////////////////////////////////////////////////////////////////////////
		/// DeviateServiceAction action class.
		/// @ingroup m35Actions refActions
		class DeviateServiceAction:
			public util::FactorableTemplate<server::Action, DeviateServiceAction>
		{
		public:
			static const std::string PARAMETER_DATASOURCE_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_CHAINAGE;

		private:
			boost::shared_ptr<impex::DataSource> _dataSource;
			boost::shared_ptr<const ScheduledService> _service;
			std::size_t _chainage;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			DeviateServiceAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			void setService(boost::shared_ptr<const ScheduledService> value){ _service = value; }
		};
	}
}

#endif // SYNTHESE_DeviateServiceAction_H__
