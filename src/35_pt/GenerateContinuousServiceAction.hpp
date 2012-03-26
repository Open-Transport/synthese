
//////////////////////////////////////////////////////////////////////////
/// GenerateContinuousServiceAction class header.
///	@file GenerateContinuousServiceAction.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_GenerateContinuousServiceAction_H__
#define SYNTHESE_GenerateContinuousServiceAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : GenerateContinuousServiceAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : GenerateContinuousServiceAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class GenerateContinuousServiceAction:
			public util::FactorableTemplate<server::Action, GenerateContinuousServiceAction>
		{
		public:
			static const std::string PARAMETER_LINE;
			static const std::string PARAMETER_WAITING_TIME;
			static const std::string PARAMETER_MIN_NUMBER;
			static const std::string PARAMETER_MIN_WAITING_DIFFERENCE;

		private:
			boost::shared_ptr<CommercialLine> _line;
			boost::posix_time::time_duration _waitingTime;
			std::size_t _minNumber;
			boost::posix_time::time_duration _minWaitingDifference;

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
				void setLine(boost::shared_ptr<CommercialLine> value) { _line = value; }
			//@}
		};
}	}

#endif // SYNTHESE_GenerateContinuousServiceAction_H__
