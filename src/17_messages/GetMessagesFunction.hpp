
//////////////////////////////////////////////////////////////////////////////////////////
/// GetMessagesFunction class header.
///	@file GetMessagesFunction.hpp
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

	namespace util
	{
		class Registrable;
	}

	namespace messages
	{
		class AlarmRecipient;
		class SentAlarm;

		//////////////////////////////////////////////////////////////////////////
		///	17.15 Service : Messages list.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Messages_list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m17Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		/// Key : messages
		class GetMessagesFunction:
			public util::FactorableTemplate<server::Function,GetMessagesFunction>
		{
		public:
			static const std::string PARAMETER_RECIPIENT_KEY;
			static const std::string PARAMETER_RECIPIENT_ID;
			static const std::string PARAMETER_MAX_MESSAGES_NUMBER;
			static const std::string PARAMETER_BEST_PRIORITY_ONLY;
			static const std::string PARAMETER_PRIORITY_ORDER;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_CMS_TEMPLATE_ID;

		protected:
			static const std::string DATA_MESSAGES;
			static const std::string DATA_MESSAGE;
			static const std::string DATA_RANK;
			static const std::string ATTR_PARAMETER;

			//! \name Page parameters
			//@{
				util::Registrable* _recipient;
				boost::shared_ptr<AlarmRecipient> _recipientClass;
				boost::optional<std::size_t> _maxMessagesNumber;
				bool _bestPriorityOnly;
				bool _priorityOrder;
				boost::posix_time::ptime _date;
				boost::posix_time::ptime _endDate;
				boost::shared_ptr<const cms::Webpage> _cmsTemplate;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Messages_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Messages_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



		public:
			GetMessagesFunction(
				util::Registrable* recipient = NULL,
				boost::shared_ptr<AlarmRecipient> _recipientClass = boost::shared_ptr<AlarmRecipient>(),
				boost::optional<size_t> maxMessagesNumber = boost::optional<size_t>(),
				bool bestPriorityOnly = true,
				bool priorityOrder = true,
				boost::posix_time::ptime date = boost::posix_time::second_clock::local_time(),
				boost::posix_time::ptime endDate = boost::posix_time::second_clock::local_time()
			);



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



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
