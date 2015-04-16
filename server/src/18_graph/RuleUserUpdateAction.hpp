
//////////////////////////////////////////////////////////////////////////
/// RuleUserUpdateAction class header.
///	@file RuleUserUpdateAction.hpp
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

#ifndef SYNTHESE_RuleUserUpdateAction_H__
#define SYNTHESE_RuleUserUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "RuleUser.h"

namespace synthese
{
	namespace db
	{
		class DBTableSync;
	}

	namespace graph
	{
		class RuleUser;

		//////////////////////////////////////////////////////////////////////////
		/// 18.15 Action : Generic rule user update.
		/// @ingroup m18Actions refActions
		///	@author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		/// @todo activate right control
		//////////////////////////////////////////////////////////////////////////
		/// Key : RuleUserUpdateAction
		///
		///	Parameters
		///	<ul>
		///		<li>actionParamid : id of the object to update</li>
		///		<li>actionParamvaxx : value for xx field (if 0, undefined)</li>
		///</ul>
		class RuleUserUpdateAction:
			public util::FactorableTemplate<server::Action, RuleUserUpdateAction>
		{
		public:
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_VALUE_PREFIX;

			typedef std::map<std::string, util::RegistryKeyType> Values;

		private:
			boost::shared_ptr<db::DBTableSync> _tableSync;
			util::RegistryKeyType _id;
			Values _values;


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
				void setId(util::RegistryKeyType value) { _id = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_RuleUserUpdateAction_H__
