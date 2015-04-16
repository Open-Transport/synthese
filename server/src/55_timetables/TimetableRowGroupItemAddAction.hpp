
//////////////////////////////////////////////////////////////////////////
/// TimetableRowGroupItemAddAction class header.
///	@file TimetableRowGroupItemAddAction.hpp
///	@author Hugues Romain
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

#ifndef SYNTHESE_TimetableRowGroupItemAddAction_H__
#define SYNTHESE_TimetableRowGroupItemAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace timetables
	{
		class TimetableRowGroup;

		//////////////////////////////////////////////////////////////////////////
		/// 55.15 Action : TimetableRowGroupItemAddAction.
		/// @ingroup m55Actions refActions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : TimetableRowGroupItemAddAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class TimetableRowGroupItemAddAction:
			public util::FactorableTemplate<server::Action, TimetableRowGroupItemAddAction>
		{
		public:
			static const std::string PARAMETER_ROWGROUP_ID;
			static const std::string PARAMETER_PLACE_NAME;
			static const std::string PARAMETER_RANK;

		private:
			boost::shared_ptr<TimetableRowGroup> _rowGroup;
			boost::shared_ptr<pt::StopArea> _stopArea;
			size_t _rank;

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
			TimetableRowGroupItemAddAction():
			  _rank(0) {}


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
				void setRowGroup(boost::shared_ptr<TimetableRowGroup> value) { _rowGroup = value; }
				void setRank(size_t value) { _rank = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_TimetableRowGroupItemAddAction_H__

