
//////////////////////////////////////////////////////////////////////////
/// CopyGeometriesAction class header.
///	@file CopyGeometriesAction.hpp
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

#ifndef SYNTHESE_CopyGeometriesAction_H__
#define SYNTHESE_CopyGeometriesAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class LineStop;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : CopyGeometriesAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : CopyGeometriesAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class CopyGeometriesAction:
			public util::FactorableTemplate<server::Action, CopyGeometriesAction>
		{
		public:
			static const std::string PARAMETER_STARTING_STOP;
			static const std::string PARAMETER_ENDING_STOP;
			static const std::string PARAMETER_EDGE_ID;

		private:
			boost::shared_ptr<const StopPoint> _startingStop;
			boost::shared_ptr<const StopPoint> _endingStop;
			boost::shared_ptr<const LineStop> _edgeTemplate;

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
				void setStartingStop(boost::shared_ptr<const StopPoint> value) { _startingStop = value; }
				void setEndingStop(boost::shared_ptr<const StopPoint> value) { _endingStop = value; }
				void setEdgeTemplate(boost::shared_ptr<const LineStop> value) { _edgeTemplate= value; }
			//@}
		};
	}
}

#endif // SYNTHESE_CopyGeometriesAction_H__
