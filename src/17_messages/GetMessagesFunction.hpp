
//////////////////////////////////////////////////////////////////////////////////////////
/// GetMessagesFunction class header.
///	@file GetMessagesFunction.hpp
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

#ifndef SYNTHESE_GetMessagesFunction_H__
#define SYNTHESE_GetMessagesFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace messages
	{
		class SentAlarm;

		//////////////////////////////////////////////////////////////////////////
		///	17.15 Function : GetMessagesFunction.
		///	@ingroup m17Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : messages
		///
		/// Parameters :
		///	<dl>
		///	<dt>r</dt><dd>id of the recipient object.</dd>
		///	<dt>n</dt><dd>max number of messages to display (optional)</dd>
		///	<dt>b</dt><dd>1|0 : if 1, display only the messages with the best priority level (optional, default=1)</dd>
		///	<dt>o</dt><dd>1|0 : if 1, messages order is from best priority to lower. In the same priority level, order is from latest beginning broadcast date to oldest. 0 is the inverse order.</dd>
		/// <dt>d</dt><dd>date</dd>
		///	<dt>t</dt><dd>id of the CMS template to use for the display of each message (optional, default is to display big content). The CMS template is called by the GetMessagesFunction::_display method.</dd>
		///	</dl>
		class GetMessagesFunction:
			public util::FactorableTemplate<server::Function,GetMessagesFunction>
		{
		public:
			static const std::string PARAMETER_RECIPIENT_ID;
			static const std::string PARAMETER_MAX_MESSAGES_NUMBER;
			static const std::string PARAMETER_BEST_PRIORITY_ONLY;
			static const std::string PARAMETER_PRIORITY_ORDER;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_CMS_TEMPLATE_ID;

		protected:
			//! \name Page parameters
			//@{
				util::RegistryKeyType _recipientId;
				boost::optional<std::size_t> _maxMessagesNumber;
				bool _bestPriorityOnly;
				bool _priorityOrder;
				boost::posix_time::ptime _date;
				boost::shared_ptr<const cms::Webpage> _cmsTemplate;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
			
			static const std::string DATA_CONTENT;
			static const std::string DATA_PRIORITY;
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_SCENARIO_NAME;
			static const std::string DATA_TITLE;

			//////////////////////////////////////////////////////////////////////////
			/// Message display.
			/// Values sent to the CMS template :
			/// <dl>
			/// <dt>roid</dt><dd>id of the message</dd>
			/// <dt>title</dt><dd>title of the message</dd>
			/// <dt>content</dt><dd>content of the message</d<>
			/// <dt>priority</dt><dd>priority level</d<>
			/// <dt>scenario_id</dt><dd>id of the scenario</dd>
			/// <dt>scenario_name</dt><dd>name of the scenario</dd>
			/// </dl>
			//////////////////////////////////////////////////////////////////////////
			/// @param stream stream to write the result on
			/// @param request current request
			/// @param message message to display
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			void _display(
				std::ostream& stream,
				const server::Request& request,
				const SentAlarm& message
			) const;
			
		public:
			GetMessagesFunction();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
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

#endif // SYNTHESE_GetMessagesFunction_H__
