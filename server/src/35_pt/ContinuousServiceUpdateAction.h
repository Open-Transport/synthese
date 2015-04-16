
//////////////////////////////////////////////////////////////////////////
/// ContinuousServiceUpdateAction class header.
///	@file ContinuousServiceUpdateAction.h
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_ContinuousServiceUpdateAction_H__
#define SYNTHESE_ContinuousServiceUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class ContinuousService;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Continuous service specific properties update.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		///
		/// Parameters :
		///	<ul>
		///		<li>id : id of the service to update</li>
		///		<li>wa : waiting duration (minutes)</li>
		///		<li>et : time of the end of the period of the continuous service</li>
		///	</ul>
		class ContinuousServiceUpdateAction:
			public util::FactorableTemplate<server::Action, ContinuousServiceUpdateAction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_WAITING_DURATION;
			static const std::string PARAMETER_END_TIME;

		private:
			boost::shared_ptr<ContinuousService> _service;
			boost::posix_time::time_duration _duration;
			boost::posix_time::time_duration _range;

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
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setService(boost::shared_ptr<ContinuousService> value) { _service = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_ContinuousServiceUpdateAction_H__
