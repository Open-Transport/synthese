
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternAddAction class header.
///	@file JourneyPatternAddAction.hpp
///	@author Hugues
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

#ifndef SYNTHESE_LineAddAction_H__
#define SYNTHESE_LineAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
		class JourneyPattern;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Route creation.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.15
		//////////////////////////////////////////////////////////////////////////
		/// Key : JourneyPatternAddAction
		///
		/// A route can be created in two ways :
		///	<ul>
		///		<li>from scratch</li>
		///		<li>from a template</li>
		///	</ul>
		///
		/// <h2>Creation from scratch</h2>
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamcl : id of the line which the route belongs to</li>
		///		<li>actionParamna (optional) : name of the route</li>
		///	</ul>
		///
		///	<h2>Creation from a template</h2>
		///
		/// Parameters :
		///	<ul>
		/// 	<li>actionParamte : id of the template to read</li>
		///		<li>actionParamre (optional) : reverse copy</li>
		///		<li>actionParamna (optional) : name of the route</li>
		///	</ul>
		///
		/// The following elements are copied into the new route :
		///	<ul>
		///		<li>the list of served stops in the same order if reverse copy deactivated, or in the opposite order if reverse copy activated</li>
		///		<li>the line which the route belongs to</li>
		///		<li>the wayback attribute (inverted if reverse copy)</li>
		///		<li>the direction only if no reverse copy</li>
		///		<li>all transport attributes except direction if re</li>
		///	</ul>
		class JourneyPatternAddAction:
			public util::FactorableTemplate<server::Action, JourneyPatternAddAction>
		{
		public:
			static const std::string PARAMETER_COMMERCIAL_LINE_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_REVERSE_COPY;

		private:
			boost::shared_ptr<pt::CommercialLine> _commercialLine;
			boost::shared_ptr<const pt::JourneyPattern> _template;
			bool _reverse;
			std::string _name;

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



			void setCommercialLine(boost::shared_ptr<pt::CommercialLine> value);
		};
	}
}

#endif // SYNTHESE_LineAddAction_H__
