
//////////////////////////////////////////////////////////////////////////////////////////
/// ForumMessagesFunction class header.
///	@file ForumMessagesFunction.hpp
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

#ifndef SYNTHESE_ForumMessagesFunction_H__
#define SYNTHESE_ForumMessagesFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace forum
	{
		class ForumTopic;

		//////////////////////////////////////////////////////////////////////////
		///	52.15 Function : ForumMessagesFunction.
		///	@ingroup m52Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		//////////////////////////////////////////////////////////////////////////
		/// Key : forum_messages
		///
		/// Parameters :
		///	<ul>
		///		<li>topic_id : id of the topic</li>
		///		<li>page : id of the display template to use for the display of each topic (the display
		///			is done by ForumInterfacePage::DisplayMessage)</li>
		///		<li>date_page (optional) : id of the display template to use for the display of dates (the display
		///			is done by DateTimeInterfacePage::Display). If not defined, then a default output is generated.</li>
		///	</ul>
		class ForumMessagesFunction:
			public util::FactorableTemplate<server::Function,ForumMessagesFunction>
		{
		public:
			static const std::string PARAMETER_TOPIC_ID;
			static const std::string PARAMETER_DISPLAY_TEMPLATE;
			static const std::string PARAMETER_DATE_DISPLAY_TEMPLATE;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const ForumTopic> _topic;
				boost::shared_ptr<const cms::Webpage> _displayTemplate;
				boost::shared_ptr<const cms::Webpage> _dateDisplayTemplate;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_ForumMessagesFunction_H__
