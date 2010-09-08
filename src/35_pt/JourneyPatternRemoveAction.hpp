
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternRemoveAction class header.
///	@file JourneyPatternRemoveAction.hpp
///	@author Hugues Romain
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

#ifndef SYNTHESE_JourneyPatternRemoveAction_H__
#define SYNTHESE_JourneyPatternRemoveAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : JourneyPatternRemoveAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : JourneyPatternRemoveAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the journey pattern to remove</li>
		///	</ul>
		class JourneyPatternRemoveAction:
			public util::FactorableTemplate<server::Action, JourneyPatternRemoveAction>
		{
		public:
			static const std::string PARAMETER_JOURNEY_PATTERN_ID;

		private:
			boost::shared_ptr<const JourneyPattern> _journeyPattern;

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
				void setJourneyPattern(boost::shared_ptr<const JourneyPattern> value) { _journeyPattern = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_JourneyPatternRemoveAction_H__
