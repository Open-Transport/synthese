
//////////////////////////////////////////////////////////////////////////
/// ServiceQuotaUpdateAction class header.
///	@file ServiceQuotaUpdateAction.hpp
///	@author Gael Sauvanet
///	@date 2012
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

#ifndef SYNTHESE_ServiceQuotaUpdateAction_H__
#define SYNTHESE_ServiceQuotaUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace pt
	{
		class ServiceQuota;
		class ScheduledService;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : ServiceQuotaUpdateAction.
		/// @ingroup m35Actions refActions
		///	@author Gael Sauvanet
		///	@date 2012
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ServiceQuotaUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class ServiceQuotaUpdateAction:
			public util::FactorableTemplate<server::Action, ServiceQuotaUpdateAction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_QUOTA;
			static const std::string PARAMETER_NB_QUOTAS;

		private:
			boost::shared_ptr<ScheduledService> _service;
			boost::shared_ptr<ServiceQuota> _serviceQuota;
			int _nbQuotas;
			std::map<boost::gregorian::date, int> _quotas;


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
				void setService(boost::shared_ptr<ScheduledService> value) { _service = value; }
			//@}
		};
}	}

#endif // SYNTHESE_ServiceQuotaUpdateAction_H__
