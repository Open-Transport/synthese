
//////////////////////////////////////////////////////////////////////////
/// InterfacePageAddAction class header.
///	@file InterfacePageAddAction.h
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

#ifndef SYNTHESE_InterfacePageAddAction_H__
#define SYNTHESE_InterfacePageAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;

		//////////////////////////////////////////////////////////////////////////
		/// InterfacePageAddAction action class.
		/// @ingroup m11Actions refActions
		class InterfacePageAddAction:
			public util::FactorableTemplate<server::Action, InterfacePageAddAction>
		{
		public:
			static const std::string PARAMETER_INTERFACE_ID;
			static const std::string PARAMETER_CLASS;
			static const std::string PARAMETER_VARIANT;

		private:
			boost::shared_ptr<Interface> _interface;
			std::string _class;
			std::string _variant;

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
				void setInterface(boost::shared_ptr<Interface> value) { _interface = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_InterfacePageAddAction_H__
