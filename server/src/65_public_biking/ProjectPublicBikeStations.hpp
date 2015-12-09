
//////////////////////////////////////////////////////////////////////////
/// ProjectPublicBikeStationsAction class header.
///	@file ProjectPublicBikeStationsAction.hpp
///	@author Camille Hue
///	@date 2015
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

#ifndef SYNTHESE_ProjectPublicBikeStationsAction_H__
#define SYNTHESE_ProjectPublicBikeStationsAction_H__

#include "Action.h"
#include "EdgeProjector.hpp"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace road
	{
		class RoadChunk;
	}

	namespace public_biking
	{
		//////////////////////////////////////////////////////////////////////////
		/// 65.15 Action : ProjectPublicBikeStationsAction.
		/// @ingroup m65Actions refActions
		///	@author Camille Hue
		///	@date 2015
		/// @since 3.10.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ProjectPublicBikeStationsAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>md</dt><dd>maximal distance between a public bike station and its projection road</dd>
		///	</dl>
		class ProjectPublicBikeStationsAction:
			public util::FactorableTemplate<server::Action, ProjectPublicBikeStationsAction>
		{
		public:
			static const std::string PARAMETER_MAX_DISTANCE;

		private:
			double _maxDistance;

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
			ProjectPublicBikeStationsAction(): _maxDistance(100) {}

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_ProjectPublicBikeStationsAction_H__
