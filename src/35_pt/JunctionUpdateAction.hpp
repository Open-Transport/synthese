
//////////////////////////////////////////////////////////////////////////
/// JunctionUpdateAction class header.
///	@file JunctionUpdateAction.hpp
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

#ifndef SYNTHESE_JunctionUpdateAction_H__
#define SYNTHESE_JunctionUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class Junction;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : JunctionUpdateAction.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Junction_update
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		class JunctionUpdateAction:
			public util::FactorableTemplate<server::Action, JunctionUpdateAction>
		{
		public:
			static const std::string PARAMETER_JUNCTION_ID;
			static const std::string PARAMETER_FROM_ID;
			static const std::string PARAMETER_TO_ID;
			static const std::string PARAMETER_LENGTH;
			static const std::string PARAMETER_TIME;
			static const std::string PARAMETER_BIDIRECTIONAL;

		private:
			boost::shared_ptr<Junction> _junction;
			boost::optional<bool> _bidirectional;
			boost::optional<boost::shared_ptr<StopPoint> > _from;
			boost::optional<boost::shared_ptr<StopPoint> > _to;
			boost::optional<double> _length;
			boost::optional<boost::posix_time::time_duration> _duration;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Junction_update#Request
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Junction_update#Request
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
				void setJunction(boost::shared_ptr<Junction> value) { _junction = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_JunctionUpdateAction_H__
