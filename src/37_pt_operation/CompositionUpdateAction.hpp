
//////////////////////////////////////////////////////////////////////////
/// CompositionUpdateAction class header.
///	@file CompositionUpdateAction.hpp
///	@author RCSobility
///	@date 2011
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

#ifndef SYNTHESE_CompositionUpdateAction_H__
#define SYNTHESE_CompositionUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace pt
	{
		class ScheduledService;
		class StopPoint;
	}

	namespace pt_operation
	{
		class Composition;

		//////////////////////////////////////////////////////////////////////////
		/// 37.15 Action : CompositionUpdateAction.
		/// @ingroup m37Actions refActions
		///	@author RCSobility
		///	@date 2011
		/// @since 3.2.1
		//////////////////////////////////////////////////////////////////////////
		/// Key : CompositionUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class CompositionUpdateAction:
			public util::FactorableTemplate<server::Action, CompositionUpdateAction>
		{
		public:
			static const std::string PARAMETER_COMPOSITION_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_FIRST_QUAY;

		private:
			boost::shared_ptr<Composition> _composition;
			boost::shared_ptr<pt::ScheduledService> _service;
			boost::optional<boost::gregorian::date> _date;
			boost::shared_ptr<pt::StopPoint> _firstQuay;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

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
				// void setObject(boost::shared_ptr<Object> value) { _object = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_CompositionUpdateAction_H__
