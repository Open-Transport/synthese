
//////////////////////////////////////////////////////////////////////////
/// ServiceCalendarLinkUpdateAction class header.
///	@file ServiceCalendarLinkUpdateAction.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_ServiceCalendarLinkUpdateAction_H__
#define SYNTHESE_ServiceCalendarLinkUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class NonPermanentService;
		class ServiceCalendarLink;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Service calendar link update.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : ServiceCalendarLinkUpdateAction
		class ServiceCalendarLinkUpdateAction:
			public util::FactorableTemplate<server::Action, ServiceCalendarLinkUpdateAction>
		{
		public:
			static const std::string PARAMETER_LINK_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_CALENDAR_TEMPLATE_ID;

		private:
			boost::shared_ptr<ServiceCalendarLink> _link;
			boost::optional<boost::shared_ptr<NonPermanentService> > _service;
			boost::optional<boost::gregorian::date> _minDate;
			boost::optional<boost::gregorian::date> _maxDate;
			boost::optional<boost::shared_ptr<calendar::CalendarTemplate> > _calendar;

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
				void setLink(boost::shared_ptr<ServiceCalendarLink> value) { _link = value; }
				void setService(boost::optional<boost::shared_ptr<NonPermanentService> > value) { _service = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_ServiceUpdateAction_H__
